#pragma once

#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

#include "dllapi.h"
#include "math/Rect.inl"


class CMesh;
class GMesh;


class Mesh
{
public:
	ENGINEGRAPHICS_API virtual ~Mesh();

	virtual void renderImmediate() const = 0;
};


//CPU-sided mesh
class CMesh : public Mesh
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord; //uv (todo: w?)
	};
	std::vector<Vertex> vertices;

	std::vector<unsigned int> triangles;

	ENGINEGRAPHICS_API CMesh();
	ENGINEGRAPHICS_API CMesh(const std::filesystem::path& path);
	ENGINEGRAPHICS_API virtual ~CMesh();

	ENGINEGRAPHICS_API virtual void renderImmediate() const override;

	//Primitives
	//ENGINEGRAPHICS_API static CMesh createCube(float size);
	ENGINEGRAPHICS_API static CMesh createQuad0WH(float w, float h); //AA rect with one corner at (0,0) and the other at (w,h), normal Z+
	ENGINEGRAPHICS_API static CMesh createUnitQuad(Rect<float> uvs); //AA rect with one corner at (0,0) and the other at (1,1), normal Z+, with given UVs

	ENGINEGRAPHICS_API CMesh(CMesh&& mov);
	ENGINEGRAPHICS_API CMesh& operator=(CMesh&& mov);
	CMesh(const CMesh& cpy) = delete;
	CMesh& operator=(const CMesh& cpy) = delete;
};


//GPU-sided mesh
class GMesh : public Mesh
{
protected:
	ENGINEGRAPHICS_API GMesh();
public:
	ENGINEGRAPHICS_API virtual ~GMesh();

	ENGINEGRAPHICS_API virtual void updateFrom(const CMesh& src) = 0;
	ENGINEGRAPHICS_API virtual void updateFrom(const CMesh& src, bool vertices, bool triangles) = 0;

	GMesh(GMesh&& mov) = delete;
	ENGINEGRAPHICS_API virtual GMesh& operator=(GMesh&& mov) = 0;
	GMesh(const GMesh& cpy) = delete;
	GMesh& operator=(const GMesh& cpy) = delete;

	ENGINEGRAPHICS_API virtual operator bool() const = 0;
};
