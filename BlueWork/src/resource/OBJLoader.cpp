#include "OBJLoader.h"

#include <loguru.hpp> 

Geometry* OBJLoader::load(std::string fn) {
	LOG_F(INFO + 1, "loading %s", fn.c_str());
	Geometry* geom = new Geometry();
	int numberOfFaces;
	std::vector<Engine::Vertex> vertices = parseOBJ(fn, numberOfFaces);
	geom->vaObject = std::unique_ptr<VertexArray>(new VertexArray());
	Buffer* vbuffer = geom->vaObject->bindBuffer<Engine::Vertex>("vertexes", GL_ARRAY_BUFFER);
	vbuffer->setData(vertices, GL_STATIC_DRAW);
	vbuffer->bindPointer(Engine::POSITION, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Position));
	vbuffer->bindPointer(Engine::TEXTURECOORD, 2, GL_FLOAT, (void*)offsetof(Engine::Vertex, TextureCoord));
	vbuffer->bindPointer(Engine::NORMAL, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Normal));
	vbuffer->bindPointer(Engine::TANGENT, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, Tangent));
	geom->size = vertices.size();
	geom->topology = numberOfFaces == 4 ? GL_QUADS :
		numberOfFaces == 3 ? GL_TRIANGLES : GL_LINES;

	return geom;
}

void CalcTangents(std::vector<Engine::Vertex>& verts) {
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

std::vector<Engine::Vertex> OBJLoader::parseOBJ(std::string fn, int& numberOfFaces) {

	std::vector<glm::vec3> verts_;
	std::vector<glm::vec3> norms_;
	std::vector<glm::vec2> uv_;
	std::vector<Engine::Vertex> verts;

	std::ifstream in;
	in.open(fn.c_str(), std::ifstream::in);
	if (in.fail()) return verts;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			glm::vec3 v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			glm::vec3 n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			glm::vec2 uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			uv_.push_back(glm::vec2(uv.x, 1 - uv.y));
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<glm::ivec3> f;
			int vert, uv, norm, faces = 0;
			iss >> trash;
			while (iss >> vert >> trash >> uv >> trash >> norm) {
				vert--; uv--; norm--;
				verts.push_back(Engine::Vertex{ verts_[vert], uv_[uv], norms_[norm] });
				faces++;
			}
			numberOfFaces = faces;
		}
	}
	CalcTangents(verts);
	return verts;
}