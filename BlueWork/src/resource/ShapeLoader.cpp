#include "ShapeLoader.h"

#include <glm/glm.hpp>



ShapeLoader::ShapeLoader() {
	Extentions = {
		".shape"
	};
}

std::vector<unsigned char> ShapeLoader::LoadFile(std::string fileName, std::string name, std::string metaData) {
	// if given the filename ".shape", it will assume the metadata is the actual data.
	int endOfPath = fileName.find_last_of("/") + 1;
	std::string noExtention = fileName.substr(endOfPath, endOfPath - fileName.find_last_of("."));
	if (noExtention.empty()) {
		DefaultName = name;
		return std::vector<unsigned char>(metaData.begin(), metaData.end());
	}
	return AbstractLoader<std::shared_ptr<Geometry>>::LoadFile(fileName, name, metaData);
}

std::map<std::string, std::shared_ptr<Geometry>> ShapeLoader::Parse(std::vector<unsigned char> data) {
	std::map<std::string, std::shared_ptr<Geometry>> output;
	nlohmann::json shapeFile = nlohmann::json::parse(data);

	if (shapeFile.is_array()) {
		for (auto& shape : shapeFile) {
			Geometry* geom = nullptr;
			if (shape["type"].get<std::string>() == "plane") {
				geom = MakePlane(
					shape["xSegments"].get<unsigned int>(),
					shape["zSegments"].get<unsigned int>());
			}
			else if (shape["type"].get<std::string>() == "zplane") {
				geom = MakeZPlane(
					shape["xSegments"].get<unsigned int>(),
					shape["ySegments"].get<unsigned int>());
			}

			output.insert(std::make_pair(DefaultName + shape["type"].get<std::string>(), geom));
		}
	}
	return output;
}

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
	return MeshProcessing::BuildIndexedGeometry(vertices, indices, GL_TRIANGLE_STRIP);
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
	return MeshProcessing::BuildIndexedGeometry(vertices, indices, GL_TRIANGLE_STRIP);
}