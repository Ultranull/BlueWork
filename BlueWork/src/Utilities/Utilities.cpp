#include "Utilities.h"

#include <fstream>
#include <sstream>

#include <loguru.hpp>

std::string Utilities::readFile(const char* file) {
	std::string content;
	std::ifstream stream(file);
	if (stream.is_open()) {
		std::stringstream ss;
		ss << stream.rdbuf();
		content = ss.str();
		stream.close();
	}
	else {
		LOG_F(ERROR, "Failed to open %s", file);
		return "";
	}
	return content;
}

std::vector<unsigned char> Utilities::readFileBytes(const char* file) {
	std::string content;
	std::ifstream stream(file);
	if (stream.is_open()) {
		std::stringstream ss;
		ss << stream.rdbuf();
		content = ss.str();
		stream.close();
	}
	else {
		LOG_F(ERROR, "Failed to open %s", file);
		return std::vector<unsigned char>();
	}
	return std::vector<unsigned char>(content.begin(), content.end());
}

void Utilities::SaveFile(const char* file, const char* content) {
	std::ofstream stream(file);
	if (stream.is_open()) {
		stream << content;
		stream.close();
	}
	else {
		LOG_F(ERROR, "Failed to open %s", file);
	}
}