#include "Mesh.hpp"

#include <ofbx.h>
#include <GL/glew.h>
#include "Renderer.hpp"

glm::vec3 toGlm(ofbx::Vec3 _v)
{
	glm::vec3 v;
	v.x = _v.x;
	v.y = _v.y;
	v.z = _v.z;
	return v;
}

glm::vec2 toGlm(ofbx::Vec2 _v)
{
	glm::vec2 v;
	v.x = _v.x;
	v.y = _v.y;
	return v;
}

CMesh::CMesh()
{
}

CMesh::CMesh(const std::filesystem::path& path)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, path.u8string().c_str(), "rb");
	assert(!err);

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ofbx::u8* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)ofbx::LoadFlags::NONE);
	assert(scene);

	delete[] content;
	fclose(fp);

	vertices.clear();
	triangles.clear();

	//Extract verts
	const ofbx::Mesh* mesh = scene->getMesh(0);
	const ofbx::GeometryData& data = mesh->getGeometryData();
	int vertCount = data.getPositions().count;
	assert(data.getPositions().count);
	vertices.reserve(vertCount);
	for (int i = 0; i < vertCount; ++i)
	{
		Vertex vert;
		                              vert.position = toGlm(data.getPositions().get(i));
		if (data.getNormals().values) vert.normal   = toGlm(data.getNormals  ().get(i));
		if (data.getUVs    ().values) vert.texCoord = toGlm(data.getUVs      ().get(i));
		vertices.push_back(vert);
	}
	
	//Triangulate
	int nTris = 0;
	for (int m = 0; m < data.getPartitionCount(); ++m) nTris += data.getPartition(m).triangles_count;
	triangles.reserve(nTris);
	for (int m = 0; m < data.getPartitionCount(); ++m)
	{
		const ofbx::GeometryPartition& part = data.getPartition(m);
		for (int p = 0; p < part.polygon_count; ++p)
		{
			int buf[64];
			int nToAdd = ofbx::triangulate(data, part.polygons[p], buf);
			for (int i = 0; i < nToAdd; ++i) triangles.push_back(buf[i]);
		}
	}
}

CMesh::~CMesh()
{
}

GMesh::GMesh() :
	VAO(0),
	VBO(0),
	EBO(0),
	nTriangles(0)
{
}

GMesh::GMesh(const CMesh& src) :
	VAO(0),
	VBO(0),
	EBO(0),
	nTriangles(0)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, src.vertices.size() * sizeof(CMesh::Vertex), &src.vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, src.triangles.size() * sizeof(decltype(src.triangles)::value_type), &src.triangles[0], GL_STATIC_DRAW);
	nTriangles = src.triangles.size();
	
	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, position));
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, normal));
	//UVs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, texCoord));

	glBindVertexArray(0);
}

GMesh::~GMesh()
{
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
}

void GMesh::renderImmediate() const
{
	glColor4f(1, 1, 1, 1);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nTriangles, GL_UNSIGNED_INT, 0);

	//Clear state
	glBindVertexArray(0);
}
