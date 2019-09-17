#include "OBJLoader.h"

Geometry* OBJLoader::load(std::string fn) {
	Geometry* geom = new Geometry();
	int numberOfFaces;
	std::vector<Engine::Vertex> vertices = parseOBJ(fn, numberOfFaces);
	geom->vaObject = new VertexArray();
	Buffer* vbuffer = geom->vaObject->bindBuffer<Engine::Vertex>("vertexes", GL_ARRAY_BUFFER);
	vbuffer->setData(vertices, GL_STATIC_DRAW);
	vbuffer->bindPointer(Engine::POSITION, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, pos));
	vbuffer->bindPointer(Engine::COLOR, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, color));
	vbuffer->bindPointer(Engine::TEXTURECOORD, 2, GL_FLOAT, (void*)offsetof(Engine::Vertex, UV));
	vbuffer->bindPointer(Engine::NORMAL, 3, GL_FLOAT, (void*)offsetof(Engine::Vertex, normal));
	geom->size = vertices.size();
	geom->topology = numberOfFaces == 4 ? GL_QUADS :
		numberOfFaces == 3 ? GL_TRIANGLES : GL_LINES;

	return geom;
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
				verts.push_back(Engine::Vertex{ verts_[vert],{ 1,1,1 } ,uv_[uv], norms_[norm] });
				faces++;
			}
			numberOfFaces = faces;
		}
	}
	return verts;
}
