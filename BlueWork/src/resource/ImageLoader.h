#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "resource/AbstractLoader.h"
#include "graphics/Material.h"


class ImageLoader : public AbstractLoader<Texture> {

	struct {
		int width;
		int height;
		int bytesPerPixel;
		int Size() { return width * height * bytesPerPixel; }
	}ImageInfo;

public:

	ImageLoader() {
		Extentions = {
			".png",
			".bmp",
			".PNG",
			".BMP"
		};
	}

	std::vector<unsigned char> LoadFile(std::string fileName, std::string name = "", std::string metaData = "") override{
		DefaultName = name;
		auto rawData =  stbi_load(fileName.c_str(), &ImageInfo.width, &ImageInfo.height, &ImageInfo.bytesPerPixel, 0);
		std::vector<unsigned char> data(rawData, rawData + ImageInfo.Size());
		stbi_image_free(rawData);
		return data;
	}

	std::map<std::string, Texture> Parse(std::vector<unsigned char> data) override {
		std::map<std::string, Texture> output;
		Texture t;
		t.bind();
		t.load();
		t.setSize(ImageInfo.width, ImageInfo.height);
		if (ImageInfo.bytesPerPixel == 3) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(t.params.target, 0, GL_RGB, ImageInfo.width, ImageInfo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
		}
		else if (ImageInfo.bytesPerPixel == 4)
			glTexImage2D(t.params.target, 0, GL_RGBA, ImageInfo.width, ImageInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		Texture::unbind();

		output.insert(std::make_pair(DefaultName, t));
		return output;
	}
};

