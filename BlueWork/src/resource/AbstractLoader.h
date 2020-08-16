#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

#include <glm/glm.hpp>

#include "Engine.h"
#include "resource/MeshProcessing.h"
#include "Utilities/Utilities.h"

template<typename ResourceType>
class AbstractLoader {
protected:
	std::vector<std::string> Extentions;

public:
	std::string LoadFile(std::string fileName) {
		return Utilities::readFile(fileName.c_str());
	}
	virtual std::map<std::string, ResourceType> Parse(std::string data) = 0;

	bool HasExtention(std::string ext) {
		return std::find(Extentions.begin(), Extentions.end(), ext) != Extentions.end();
	}
};

#include "graphics/Geometry.h"

class ObjLoader2 : public AbstractLoader<Geometry*> {


public:

	ObjLoader2() {
		Extentions = {
			".obj",
			".OBJ"
		};
	}

	std::map<std::string, Geometry*> Parse(std::string data) override {
		std::map<std::string, Geometry*> output;
		std::vector<glm::vec3> verts_;
		std::vector<glm::vec3> norms_;
		std::vector<glm::vec2> uv_;
		std::vector<Engine::Vertex> verts;

		std::stringstream in(data);

		std::string currentName;

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
			}
			else if (!line.compare(0, 2, "o ")) {
				if (verts.size() > 0) {
					output.insert(std::make_pair(currentName, MeshProcessing::BuildGeometry(verts)));
					verts.clear();
				}
				currentName = line.substr(2);
			}
		}
		if (verts.size() > 0) {
			output.insert(std::make_pair(currentName, MeshProcessing::BuildGeometry(verts)));
			verts.clear();
		}
		return output;
	}
};