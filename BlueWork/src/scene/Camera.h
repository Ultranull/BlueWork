#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../graphics/Buffer.h"
#include "../graphics/ShaderProgram.h"

#include "Node.h"

struct CameraSettings {
	enum class Mode {
		Unknown,
		Orthographic,
		Perspective
	};
	Mode mode;
	union {
		struct {
			float NearPlane; 
			float FarPlane;
			float FOV;
		}PerspecitveData;

		struct {
			float NearPlane;
			float FarPlane;
			float Left;
			float Right;
			float Bottom;
			float Top;
		}OrthographicData;
	};
};

class Camera: public Node{

	glm::mat4 view;		  
	glm::mat4 projection; 

	UniformBuffer* buffer;

	CameraSettings settings;

public:
	Camera();
	Camera(CameraSettings s);

	static std::shared_ptr<UniformBuffer> buildCamera(void);

	void bindCamera(UniformBuffer* buf, Program shader);
	void updateBuffer();

	glm::mat4 P();
	glm::mat4 V();

	void UpdateProjection(int height, int width);
	void UpdateProjection(int height, int width, CameraSettings newSettings);

	glm::vec3 getDirection();
	glm::vec3 getPosition();

	CameraSettings GetSettings();
	void SetSettings(CameraSettings cs);

	void cleanup();
	void guidraw();
};
