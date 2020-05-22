#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <sstream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include <loguru.hpp>

#include "OBJLoader.h"
#include "Utilities.h"

using namespace std;

bool compileshader(const char* file, GLuint id) {
	GLint result = GL_FALSE;
	int infoLogLength;
	LOG_F(INFO,"Compiling shader: %s", file);
	string content = Utilities::readFile(file);
	if (content.compare("") == 0) { return NULL; }
	char const * src = content.c_str();
	const GLint length = content.size();
	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &errormessage[0]);
		LOG_F(INFO, "%s compile error:\n\t%s", file, &errormessage[0]);
		return false;
	}
	return true;
}

GLuint loadshaders(const char *vertexfile, const char *fragmentfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }

	LOG_F(INFO, "linking program");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		LOG_F(WARNING, "link error:\n%s", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	return programID;
}
GLuint loadshaders(const char *vertexfile, const char *fragmentfile, const char *geometryfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryID = glCreateShader(GL_GEOMETRY_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }
	if (!compileshader(geometryfile, geometryID)) { return NULL; }

	LOG_F(INFO, "linking program");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glAttachShader(programID, geometryID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		LOG_F(WARNING, "link error:\n%s", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);
	glDetachShader(programID, geometryID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	glDeleteShader(geometryID);

	return programID;
}
	Texture Resource::addTexture(string name, const char *tex) {
		string fn = (path + texturePath + string(tex));
		const char *file = fn.c_str();
		Texture t = LoadGLTexture(file);
		textures.insert(make_pair(name, t));
		return t;
	}

	void Resource::addTextures(string name, const char *tar, int sub_width, int sub_height, int ir, int ic) {
		const char *file = (path + texturePath + tar).c_str();
		for (int i = 0;i < ic;i++)
			for (int c = 0;c < ir;c++) {
				stringstream n;
				n << name << "-"<<i<<","<<c;
				Texture t = LoadGLsubTexture(file, c*sub_width, i*sub_height, sub_width, sub_height);
				textures.insert(make_pair(n.str(), t));
			}

	}
	Texture Resource::getTexture(string name) {
		return textures[name];
	}

	GLint Resource::bindTexture(std::string name, GLuint sample){
		return getTexture(name).activate(sample);
	}

	Shader Resource::addShader(string file) {
		GLuint type;
		string ext = file.substr(file.size() - 4, 4);
		if (ext.compare("vert") == 0)
			type = GL_VERTEX_SHADER;
		else if (ext.compare("frag") == 0)
			type = GL_FRAGMENT_SHADER;
		else if (ext.compare("geom") == 0)
			type = GL_GEOMETRY_SHADER;
		else return Shader();
		Shader sh(path + shaderPath + file, type);
		shaders.insert({ file,sh });
		return sh;
	}
	Shader Resource::getShader(string name) {
		return shaders[name];
	}

	void Resource::addGeometry(std::string name, Geometry* geom){
		geometries.insert(make_pair(name,geom));
	}

	Geometry* Resource::getGeometry(std::string name){
		return geometries[name];
	}

	Texture Resource::LoadGLTexture(const char *filename) {
		//glEnable(GL_TEXTURE_2D);

		Texture t;
		t.bind();
		t.load();
		int w, h, bpp;
		unsigned char *data = stbi_load(filename, &w, &h, &bpp, 0);
		if (data == NULL) {
			LOG_F(FATAL, "error loading texture %s!", filename);
			getchar();
			exit(-1);
		}
		t.setSize(w, h);
		if (bpp == 3) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(t.params.target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (bpp == 4)
			glTexImage2D(t.params.target, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		Texture::unbind();
		return t;
	}
	Texture Resource::LoadGLsubTexture(const char *filename, int sub_x, int sub_y, int sub_width, int sub_height) {
		glEnable(GL_TEXTURE_2D);

		unsigned char *data;
		int width, height;

		Texture t;
		t.bind();
		t.load();

		data = stbi_load(filename, &width, &height, 0, STBI_rgb_alpha);
		if (data == NULL) {
			LOG_F(FATAL, "error loading texture %s!", filename);
			getchar();
			exit(-1);
		}
		t.setSize(width, height);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		char *subimg = (char*)data + (sub_x + sub_y * width) * 4;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sub_width, sub_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, subimg);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		stbi_image_free(data);
		Texture::unbind();
		return t;
	}

	void Resource::setPath(string texturePath, string shaderPath, string path="assets/"){
		this->path = path;
		this->texturePath = texturePath;
		this->shaderPath = shaderPath;
	}

	string Resource::getShaderPath(std::string file){
		return (path+shaderPath+file);
	}

	void Resource::cleanup() {
		map<string,Texture>::iterator textit = textures.begin();
		map<string,Shader>::iterator shadit = shaders.begin();
		map<string,Geometry*>::iterator geomit = geometries.begin();
		for (;textit != textures.end();textit++) {
			textit->second.cleanup();
		}
		for (;shadit != shaders.end();shadit++) {
			shadit->second.cleanup();
		}
		for (; geomit != geometries.end(); geomit++) {
			geomit->second->cleanup();
			delete geomit->second;
		}
		textures.clear();
		shaders.clear();
		geometries.clear();
	}

	std::string Resource::GetGeometryName(Geometry* geom) {
		std::vector<std::string> values;
		if (Utilities::findByValue(values, geometries, geom)) {
			return values[0];
		}
		return "";
	}

	std::string Resource::GetTextureName(Texture tex) {
		std::vector<std::string> values;
		if (Utilities::findByValue(values, textures, tex)) {
			return values[0];
		}
		return "";
	}

	std::string Resource::GetShaderName(Shader shader) {
		std::vector<std::string> values;
		if (Utilities::findByValue(values, shaders, shader)) {
			return values[0];
		}
		return "";
	}

	/*
		manifest:
			
			model.obj:model;
			text.png:image;
			vertex.vert:vertex;
	
	*/

	bool isImage(std::string s) {
		return s.compare("png") == 0 ||
			s.compare("bmp") == 0 ||
			s.compare("jpg") == 0;
	}

	bool isShader(std::string s) {
		return s.compare("vert") == 0 ||
			s.compare("frag") == 0 ||
			s.compare("geom") == 0;
	}

	void Resource::batchLoad(std::string manifest) {

		OBJLoader loader;

		size_t pos = 0;
		std::string line;
		while ((pos = manifest.find(";")) != std::string::npos) {
			line = manifest.substr(0, pos);

			std::string file,name;
			size_t posFile = line.find(":");
			file = line.substr(0, posFile);
			name = line.substr(posFile + 1, pos);

			size_t posExt = file.find(".");
			std::string extension = file.substr(posExt + 1, posFile);

			if (extension.compare("obj") == 0) {
				LOG_F(INFO, "loading geometry %s as %s", file.c_str(),name.c_str());
				addGeometry(name, loader.load(path + file));
			}
			else if (isImage(extension)) {
				LOG_F(INFO, "loading image %s as %s", file.c_str(), name.c_str());
				addTexture(name, file.c_str());
			}
			else if(isShader(extension)){
				LOG_F(INFO, "loading shader %s", file.c_str());
				addShader(file);
			}
			manifest.erase(0, pos + 1);
		}
	}