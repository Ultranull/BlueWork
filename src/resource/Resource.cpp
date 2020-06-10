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
#include "Utilities/Utilities.h"
#include "Serializer.h"
#include "scene/SceneManager.h"

using namespace std;
	
Resource::Resource():
	Manifest(""){

}

bool Resource::ContainsName(std::string name) {
	return
		MapContains(textures, name) ||
		MapContains(shaders, name) ||
		MapContains(geometries, name);
}

void Resource::QueueLoadTask(Task<std::string>* task) {
	LoadTasks.Queue(task);
}

void Resource::SetLoadSucessCallback(std::function<void(void)> loadCall) {
	OnLoadSucess = loadCall;
}

Texture Resource::addTexture(string name, const char *tex) {
	string fn = (path + texturePath + string(tex));
	const char *file = fn.c_str();
	Texture t = LoadGLTexture(file);
	textures.insert(make_pair(name, t)); // in critical section
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
	else if (ext.compare("comp") == 0)
		type = GL_COMPUTE_SHADER;
	else return Shader();
	Shader sh(path + shaderPath + file, type);
	shaders.insert({ file,sh }); // in critical section
	return sh;
}
Shader Resource::getShader(string name) {
	if (shaders.find(name) != shaders.end()) {
		return shaders[name];
	}
	LOG_F(ERROR, "Could not find shader with name %s!", name.c_str());
	return Shader();
}

void Resource::addGeometry(std::string name, Geometry* geom){
	geometries.insert(make_pair(name,geom)); // in critical section
}

Geometry* Resource::getGeometry(std::string name){
	return geometries[name].get();
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

std::string Resource::GetManifest() {
	return Manifest;
}

void Resource::cleanup() {
	map<string,Texture>::iterator textit = textures.begin();
	map<string,Shader>::iterator shadit = shaders.begin();
	map<string,std::unique_ptr<Geometry>>::iterator geomit = geometries.begin();
	for (;textit != textures.end();textit++) {
		textit->second.cleanup();
	}
	for (;shadit != shaders.end();shadit++) {
		shadit->second.cleanup();
	}
	for (; geomit != geometries.end(); geomit++) {
		geomit->second->cleanup();
	}
	textures.clear();
	shaders.clear();
	geometries.clear();
}

std::string Resource::GetGeometryName(Geometry* geom) {
	map<string, std::unique_ptr<Geometry>>::iterator geomit = geometries.begin();
	for (; geomit != geometries.end(); geomit++) {
		if (geomit->second.get() == geom) {
			return geomit->first;
		}
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

void Resource::LoadAssetTask(std::string line) {
	OBJLoader loader;
	std::string file, name;
	size_t posFile = line.find(":");
	file = line.substr(0, posFile);
	name = line.substr(posFile + 1);

	size_t posExt = file.find(".");
	std::string extension = file.substr(posExt + 1, posFile);

	if (extension.compare("obj") == 0) {
		LOG_F(INFO, "loading geometry %s as %s", file.c_str(), name.c_str());
		addGeometry(name, loader.load(path + file)); // if ever threaded inserting should be in critical sections
	}
	else if (isImage(extension)) {
		LOG_F(INFO, "loading image %s as %s", file.c_str(), name.c_str());
		addTexture(name, file.c_str());
	}
	else if (isShader(extension)) {
		LOG_F(INFO, "loading shader %s", file.c_str());
		addShader(file);
	}
}

void Resource::batchLoad(std::string manifest, bool queue) {

	Manifest += manifest;

	size_t pos = 0;
	std::string line;
	while ((pos = manifest.find(";")) != std::string::npos) {
		line = manifest.substr(0, pos);
		if (queue) {
			if (!LoadTasks.Contains(line)) {
				Task<std::string>* task = new Task<std::string>();
				task->Data = line;
				task->Notify = Task<std::string>::BindTask<Resource>(&Resource::LoadAssetTask, this);
				LoadTasks.Queue(task);
			}
		}
		else{
			LoadAssetTask(line);
		}
		
		manifest.erase(0, pos + 1);
	}
}

int Resource::LoadQueueSize() {
	return LoadTasks.NumberOfTasks();
}

void Resource::ProcessNextLoadTask() {
	if (LoadTasks.NumberOfTasks() != 0) {
		LoadTasks.ProcessNext();
	}
	else {
		OnLoadSucess();
	}
}

std::string Resource::GetNextLoadString() {
	return LoadTasks.GetNextTaskData();
}

void Resource::ImmediateLoadScene(std::string filename, SceneManager* scene) {
	LoadManifest(filename);
	LoadTasks.Process();
	LoadScene(filename, scene);
}

void Resource::LoadManifest(std::string fileName) {
	std::string contents = Utilities::readFile(fileName.c_str());
	nlohmann::json json = nlohmann::json::parse(contents);

	std::stringstream ss;
	nlohmann::json manifestArray = json["Manifest"];

	for (int i = 0; i < manifestArray.size(); i++) {
		std::string name = manifestArray[i]["name"].get<std::string>();
		std::string assetFile = manifestArray[i]["asset"].get<std::string>();
		if (!ContainsName(name) && !ContainsName(assetFile)) {
			ss << assetFile;
			ss << ":";
			ss << name;
			ss << ";";
		}
	}
	LOG_F(INFO + 1, "loaded manifest: %s", ss.str().c_str());
	batchLoad(ss.str(), true);
}

void Resource::LoadScene(std::string fileName, SceneManager* manager) {
	Serializer& S = Serializer::getInstance();
	std::string contents = Utilities::readFile(fileName.c_str());
	nlohmann::json json = nlohmann::json::parse(contents);

	Node* root = manager->GetRoot();
	S.Parse(root->GetTypeName(), json["Scene"], root, nullptr);
}
