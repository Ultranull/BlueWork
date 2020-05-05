#include "ShapeLoader.h"

#include <glm/glm.hpp>

Geometry* ShapeLoader::MakePlane(unsigned int xSegments, unsigned int ySegments) {
	std::vector<Engine::Vertex> vertices;
	std::vector<unsigned int> indices;

	bool oddRow = false;
	float dX = 1.0f / xSegments;
	float dY = 1.0f / ySegments;
	
	for (int y = 0; y <= ySegments; ++y) {
		for (int x = 0; x <= xSegments; ++x) {
			glm::vec3 pos(dX * x * 2.0f - 1.0f, 0, dY * y * 2.0f - 1.0f);
			glm::vec2 uv(dX * x, 1.0f - y * dY);
			glm::vec3 norm(0.0f, 0.0f, 1.0f);
			vertices.push_back(Engine::Vertex(pos, glm::vec3(1), uv, norm));
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

Geometry* ShapeLoader::MakeZPlane(unsigned int xSegments, unsigned int zSegments) {
	std::vector<Engine::Vertex> vertices;
	std::vector<unsigned int> indices;

	bool oddRow = false;
	float dX = 1.0f / xSegments;
	float dZ = 1.0f / zSegments;

	for (int z = 0; z <= zSegments; ++z) {
		for (int x = 0; x <= xSegments; ++x) {
			glm::vec3 pos(dX * x * 2.0f - 1.0f, dZ * z * 2.0f - 1.0f, 0);
			glm::vec2 uv(dX * x, 1.0f - z * dZ);
			glm::vec3 norm(0.0f, 0.0f, 1.0f);
			vertices.push_back(Engine::Vertex(pos, glm::vec3(1), uv, norm));
		}
	}
	for (int y = 0; y < zSegments; ++y) {
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

Geometry* ShapeLoader::load(std::vector<Engine::Vertex> vertices, std::vector<unsigned int> indices) {
	Geometry* geom = new Geometry();
	geom->vaObject = new VertexArray();
	Buffer* vbuffer = geom->vaObject->bindBuffer<Engine::Vertex>("vertexes", GL_ARRAY_BUFFER);
	vbuffer->setData(vertices, GL_STATIC_DRAW);
	vbuffer->bindPointer(Engine::POSITION, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Position));
	vbuffer->bindPointer(Engine::COLOR, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Color));
	vbuffer->bindPointer(Engine::TEXTURECOORD, 2, GL_FLOAT, (void*)offsetof(Engine::Vertex, TextureCoord));
	vbuffer->bindPointer(Engine::NORMAL, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Normal));
	geom->size = vertices.size();


	Buffer* ebuffer = geom->vaObject->bindBuffer<unsigned int>("indides", GL_ELEMENT_ARRAY_BUFFER);
	ebuffer->setData(indices, GL_STATIC_DRAW);

	geom->indexed = true;
	geom->topology = GL_TRIANGLE_STRIP;

	return geom;
}