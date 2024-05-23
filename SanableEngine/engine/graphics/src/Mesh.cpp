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

Mesh::~Mesh()
{
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

	scene->destroy();
}

CMesh::~CMesh()
{
}

void CMesh::renderImmediate() const
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < triangles.size(); i += 3)
	{
		const Vertex& v0 = vertices[triangles[i+0]];
		glTexCoord2f(v0.texCoord.x, v0.texCoord.y);
		glNormal3f(v0.normal  .x, v0.normal  .y, v0.normal  .z);
		glVertex3f(v0.position.x, v0.position.y, v0.position.z);

		const Vertex& v1 = vertices[triangles[i+1]];
		glTexCoord2f(v1.texCoord.x, v1.texCoord.y);
		glNormal3f(v1.normal  .x, v1.normal  .y, v1.normal  .z);
		glVertex3f(v1.position.x, v1.position.y, v1.position.z);

		const Vertex& v2 = vertices[triangles[i+2]];
		glTexCoord2f(v2.texCoord.x, v2.texCoord.y);
		glNormal3f(v2.normal  .x, v2.normal  .y, v2.normal  .z);
		glVertex3f(v2.position.x, v2.position.y, v2.position.z);
	}
	glEnd();
}

CMesh::CMesh(CMesh&& mov)
{
	*this = std::move(mov); //Defer
}

CMesh& CMesh::operator=(CMesh&& mov)
{
	this->vertices  = std::move(mov.vertices);
	this->triangles = std::move(mov.triangles);
	return *this;
}

CMesh CMesh::createQuad0WH(float w, float h)
{
	CMesh mesh;

#define EMIT_VERTEX(x, y) mesh.vertices.push_back(CMesh::Vertex { glm::vec3(x*w, y*h, 0), glm::vec3(0, 0, 1), glm::vec2(x, y) })

	//Tri 0: Verts 0-2
	EMIT_VERTEX(0, 0);
	EMIT_VERTEX(1, 0);
	EMIT_VERTEX(0, 1);

	//Tri 1: Verts 1-3
	EMIT_VERTEX(1, 1);
#undef EMIT_VERTEX

	mesh.triangles = {
		0, 1, 2,
		2, 1, 3
	};
	return mesh;
}

CMesh CMesh::createUnitQuad(Rect<float> uvs)
{
	CMesh mesh;

#define EMIT_VERTEX(x, y) mesh.vertices.push_back(CMesh::Vertex { glm::vec3(x, y, 0), glm::vec3(0, 0, 1), uvs.topLeft+uvs.size*Vector2f(x,y) })

	//Tri 0: Verts 0-2
	EMIT_VERTEX(0, 0);
	EMIT_VERTEX(1, 0);
	EMIT_VERTEX(0, 1);

	//Tri 1: Verts 1-3
	EMIT_VERTEX(1, 1);
#undef EMIT_VERTEX
	
	mesh.triangles = {
		0, 1, 2,
		2, 1, 3
	};
	return mesh;
}

GMesh::GMesh() :
	VAO(0),
	VBO(0),
	EBO(0),
	nTriangles(0)
{
}

GMesh::GMesh(const CMesh& src, bool dynamic) :
	VAO(0),
	VBO(0),
	EBO(0),
	nTriangles(0)
{
	auto usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, src.vertices.size() * sizeof(CMesh::Vertex), &src.vertices[0], usage);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, src.triangles.size() * sizeof(decltype(src.triangles)::value_type), &src.triangles[0], usage);
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

void GMesh::updateFrom(const CMesh& src)
{
	updateFrom(src, true, true);
}

void GMesh::updateFrom(const CMesh& src, bool vertices, bool triangles)
{
	assert(*this);

	if (vertices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, src.vertices.size() * sizeof(CMesh::Vertex), &src.vertices[0]);
	}

	if (triangles)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, src.triangles.size() * sizeof(decltype(src.triangles)::value_type), &src.triangles[0]);
	}
}

void GMesh::renderImmediate() const
{
	assert(*this);

	glColor4f(1, 1, 1, 1);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nTriangles, GL_UNSIGNED_INT, 0);

	//Clear state
	glBindVertexArray(0);
}

GMesh::GMesh(GMesh&& mov)
{
	*this = std::move(mov); //Defer
}

GMesh& GMesh::operator=(GMesh&& mov)
{
	this->VAO = mov.VAO;
	this->VBO = mov.VBO;
	this->EBO = mov.EBO;
	this->nTriangles = mov.nTriangles;

	mov.VAO = 0;
	mov.VBO = 0;
	mov.EBO = 0;
	mov.nTriangles = 0;

	return *this;
}

GMesh::operator bool() const
{
	return VAO && VBO && EBO;
}
