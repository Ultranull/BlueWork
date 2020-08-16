#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "Engine.h"
#include "graphics/Geometry.h"

class MeshProcessing{

public:

	static Geometry* BuildGeometry(std::vector<Engine::Vertex> vertices){
		Geometry* geom = new Geometry();
		geom->vaObject = std::unique_ptr<VertexArray>(new VertexArray());
		Buffer* vbuffer = geom->vaObject->bindBuffer<Engine::Vertex>("vertexes", GL_ARRAY_BUFFER);
		vbuffer->setData(vertices, GL_STATIC_DRAW);
		vbuffer->bindPointer(Engine::POSITION, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Position));
		vbuffer->bindPointer(Engine::TEXTURECOORD, 2, GL_FLOAT, (void*)offsetof(Engine::Vertex, TextureCoord));
		vbuffer->bindPointer(Engine::NORMAL, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Normal));
		vbuffer->bindPointer(Engine::TANGENT, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Tangent));
		geom->size = vertices.size();
		geom->topology =  GL_TRIANGLES;
		return geom;
	}

	static void CalcTangents(std::vector<Engine::Vertex>& verts) {
		for (int i = 0; i < verts.size(); i += 3) {
			// Shortcuts for vertices
			glm::vec3& v0 = verts[i + 0].Position;
			glm::vec3& v1 = verts[i + 1].Position;
			glm::vec3& v2 = verts[i + 2].Position;

			// Shortcuts for UVs
			glm::vec2& uv0 = verts[i + 0].TextureCoord;
			glm::vec2& uv1 = verts[i + 1].TextureCoord;
			glm::vec2& uv2 = verts[i + 2].TextureCoord;

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			verts[i + 0].Tangent = tangent;
			verts[i + 1].Tangent = tangent;
			verts[i + 2].Tangent = tangent;
		}
	}

};

