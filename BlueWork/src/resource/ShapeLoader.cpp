#include "ShapeLoader.h"

#include <glm/glm.hpp>

Geometry* ShapeLoader::MakeZPlane(unsigned int xSegments, unsigned int ySegments) {
	std::vector<Engine::Vertex> vertices;
	std::vector<unsigned int> indices;

	bool oddRow = true;
	float dX = 1.0f / xSegments;
	float dY = 1.0f / ySegments;
	
	for (int y = ySegments; y >= 0; --y) {
		for (int x = 0; x <= xSegments; ++x) {
			glm::vec3 pos(dX * x * 2.0f - 1.0f, dY * y * 2.0f - 1.0f, 0);
			glm::vec2 uv(dX * x, y * dY);
			glm::vec3 norm(0.0f, 0.0f, 1.0f);
			vertices.push_back(Engine::Vertex(pos, uv, norm));
		}
	}
	for (int y = 0; y < ySegments; ++y) {
		if (!oddRow) {// even rows: y == 0, y == 2; and so on
			for (int x = 0; x <= xSegments; ++x) {
				indices.push_back(y * (xSegments + 1) + x);
				indices.push_back((y + 1) * (xSegments + 1) + x);
			}
		}
		else {
			for (int x = xSegments; x >= 0; --x) {
				indices.push_back((y + 1) * (xSegments + 1) + x);
				indices.push_back(y * (xSegments + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	return load(vertices, indices);
}

Geometry* ShapeLoader::MakePlane(unsigned int xSegments, unsigned int zSegments) {
	std::vector<Engine::Vertex> vertices;
	std::vector<unsigned int> indices;

	bool oddRow = false;
	float dX = 1.0f / xSegments;
	float dZ = 1.0f / zSegments;

	for (int z = 0; z <= zSegments; ++z) {
		for (int x = 0; x <= xSegments; ++x) {
			glm::vec3 pos(dX * x * 2.0f - 1.0f, 0, dZ * z * 2.0f - 1.0f);
			glm::vec2 uv(dX * x, 1.0f - z * dZ);
			glm::vec3 norm(0.0f, 1.0f, 0.0f);
			vertices.push_back(Engine::Vertex(pos, uv, norm));
		}
	}
	for (int z = 0; z < zSegments; ++z) {
		if (!oddRow) {// even rows: y == 0, y == 2; and so on
			for (int x = 0; x <= xSegments; ++x) {
				indices.push_back(z * (xSegments + 1) + x);
				indices.push_back((z + 1) * (xSegments + 1) + x);
			}
		}
		else {
			for (int x = xSegments; x >= 0; --x) {
				indices.push_back((z + 1) * (xSegments + 1) + x);
				indices.push_back(z * (xSegments + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	return load(vertices, indices);
}

Geometry* ShapeLoader::load(std::vector<Engine::Vertex> vertices, std::vector<unsigned int> indices) {
	Geometry* geom = new Geometry();
	geom->vaObject = std::unique_ptr<VertexArray>(new VertexArray());;
	Buffer* vbuffer = geom->vaObject->bindBuffer<Engine::Vertex>("vertexes", GL_ARRAY_BUFFER);
	vbuffer->setData(vertices, GL_STATIC_DRAW);
	vbuffer->bindPointer(Engine::POSITION, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Position));
	vbuffer->bindPointer(Engine::TEXTURECOORD, 2, GL_FLOAT, (void*)offsetof(Engine::Vertex, TextureCoord));
	vbuffer->bindPointer(Engine::NORMAL, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Normal));

	Buffer* ebuffer = geom->vaObject->bindBuffer<unsigned int>("indides", GL_ELEMENT_ARRAY_BUFFER);
	ebuffer->setData(indices, GL_STATIC_DRAW);

	geom->size = indices.size();
	geom->indexed = true;
	geom->topology = GL_TRIANGLE_STRIP;

	return geom;
}