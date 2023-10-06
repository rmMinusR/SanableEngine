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

void Mesh::load(Mesh& out, const std::filesystem::path& path)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, path.u8string().c_str(), "rb");

	assert(!err);
	if (!err) return;

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ofbx::u8* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	ofbx::LoadFlags flags = (ofbx::LoadFlags) ~(ofbx::u16)(ofbx::LoadFlags::IGNORE_MODELS | ofbx::LoadFlags::IGNORE_MESHES);
	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);
	assert(scene);

	delete[] content;
	fclose(fp);


	const ofbx::Mesh* mesh = scene->getMesh(0);
	int vertCount = mesh->getGeometryData().getPositions().count;
	out.vertices.clear();
	out.vertices.reserve(vertCount);
	for (int i = 0; i < vertCount; ++i)
	{
		out.vertices.push_back(
			{
				toGlm(mesh->getGeometryData().getPositions().get(i)),
				toGlm(mesh->getGeometryData().getNormals().get(i)),
				toGlm(mesh->getGeometryData().getUVs().get(i))
			}
		);
	}

	out.uploadToGPU();
}

void Mesh::uploadToGPU()
{
	if (!VAO) glGenVertexArrays(1, &VAO);
	if (!VBO) glGenBuffers(1, &VBO);
	if (!EBO) glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(decltype(faces)::value_type), &faces[0], GL_STATIC_DRAW);
	
	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//UVs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}

void Mesh::render()
{
	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0);

	//Clear state
	glBindVertexArray(0);
}