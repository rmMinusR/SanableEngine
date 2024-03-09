#include "Mesh.hpp"

#include <ofbx.h>
#include <GL/glew.h>

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

Mesh::Mesh() :
	VAO(0),
	VBO(0),
	EBO(0),
	dynamic(false)
{
}

Mesh::~Mesh()
{
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(Mesh&& mov) :
	Mesh()
{
	*this = std::move(mov);
}

Mesh& Mesh::operator=(Mesh&& mov)
{
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);

	this->VAO = mov.VAO; mov.VAO = 0;
	this->VBO = mov.VBO; mov.VBO = 0;
	this->EBO = mov.EBO; mov.EBO = 0;

	this->dynamic = mov.dynamic;

	//this->vertices .resize(mov.vertices .size()); memcpy(this->vertices .data(), mov.vertices .data(), mov.vertices .size()*sizeof(decltype(mov.vertices )::value_type));
	//this->triangles.resize(mov.triangles.size()); memcpy(this->triangles.data(), mov.triangles.data(), mov.triangles.size()*sizeof(decltype(mov.triangles)::value_type));
	this->vertices  = std::move(mov.vertices );
	this->triangles = std::move(mov.triangles);

	return *this;
}

bool Mesh::load(const std::filesystem::path& path)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, path.u8string().c_str(), "rb");

	assert(!err);
	if (err) return false;

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ofbx::u8* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)ofbx::LoadFlags::NONE);

	assert(scene);
	if (!scene) return false;

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

	//printf("Verts: %i predicted / %i real\n", vertCount, out.vertices.size());
	//printf("Tris: %i predicted / %i real\n", nTris, out.faces.size());

	uploadToGPU();

	return true;
}

void Mesh::markDynamic()
{
	assert(!VAO && !VBO && !EBO && "Cannot alter a buffer's usagespec once it has already been uploaded");
	dynamic = true;
}

void Mesh::uploadToGPU()
{
	auto usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	if (!VAO) glGenVertexArrays(1, &VAO);
	if (!VBO) glGenBuffers(1, &VBO);
	if (!EBO) glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(decltype(triangles)::value_type), &triangles[0], usage);
	
	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//UVs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}

void Mesh::renderImmediate() const
{
	GLenum err = glGetError();
	
	glColor4f(1, 1, 1, 1);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

	//Clear state
	glBindVertexArray(0);
}

Mesh Mesh::createQuad0WH(float w, float h)
{
	Mesh mesh;

#define EMIT_VERTEX(x, y) mesh.vertices.push_back(Mesh::Vertex { glm::vec3(x*w, y*h, 0), glm::vec3(0, 0, 1), glm::vec2(x, y) })

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
