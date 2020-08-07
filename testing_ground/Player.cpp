#include "Player.h"

#include <string>

#include <loguru.hpp>

#include "componentSystem/SystemManager.h"
#include "resource/Serializer.h"
#include "Utilities/DebugGui.h"

const unsigned int FORWARD = GLFW_KEY_W;//FIX! move to input class
const unsigned int BACKWARD = GLFW_KEY_S;
const unsigned int STRAFERIGHT = GLFW_KEY_D;
const unsigned int STRAFELEFT = GLFW_KEY_A;
const unsigned int JUMP = GLFW_KEY_SPACE;
const unsigned int CROUCH = GLFW_KEY_LEFT_SHIFT;

Player::Player(Geometry* geometry, Material mat) :
	Entity("Player", geometry, mat) {

	speed = 3.f;
	mouseSpeed = 0.005f;
	xpos = 0;
	ypos = 0;
	oldxpos = 0;
	oldypos = 0;
	hangle = 0;
	vangle = 0;

	input = SystemManager::getInstance().CreateComponent<InputComponent>();
	input->SetKeyEvent(&Player::OnKeyEvent, this);
	input->SetMouseMoveEvent(&Player::OnMouseMove, this);

}

void Player::OnMouseMove(double mouseX, double mouseY) {
	oldxpos = xpos;
	oldypos = ypos;
	xpos = mouseX;
	ypos = mouseY;

	if (captureMouse) {
		input->SetCursorMode(CursorInputMode::Disabled);

		hangle += mouseSpeed * float(oldxpos - xpos);//theta
		vangle += mouseSpeed * float(oldypos - ypos);//phi

		constexpr float piHalves = glm::half_pi<float>();

		glm::vec3 direction = glm::vec3(
			cos(vangle) * sin(hangle),
			sin(vangle),
			cos(vangle) * cos(hangle));

		glm::vec3 right(
			sin(hangle - piHalves),
			0,
			cos(hangle - piHalves));

		glm::vec3 up = glm::cross(right, direction);

		glm::vec3 xaxis = glm::cross(up, direction);
		xaxis = glm::normalize(xaxis);

		glm::mat3 rotation(xaxis, up, direction);

		transform.Rotation() = glm::quat_cast(rotation);
	}
}

void Player::OnKeyEvent(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
		captureMouse = !captureMouse;
		input->SetCursorMode(CursorInputMode::Normal);
	}
}

void Player::movement(float delta, int width, int height) {

	if (captureMouse) {

		glm::vec3 right = glm::cross(transform.Forward(), transform.Up());
		glm::vec3 front = glm::cross(glm::vec3(0, 1, 0), right);

		if (input->GetKeyState(FORWARD) == InputState::Pressed) {
			transform.Position() += front * delta * speed;
		}
		if (input->GetKeyState(BACKWARD) == InputState::Pressed) {
			transform.Position() -= front * delta * speed;
		}
		if (input->GetKeyState(STRAFERIGHT) == InputState::Pressed) {
			transform.Position() += right * delta * speed;
		}
		if (input->GetKeyState(STRAFELEFT) == InputState::Pressed) {
			transform.Position() -= right * delta * speed;
		}
		if (input->GetKeyState(JUMP) == InputState::Pressed) {
			transform.Position() += glm::vec3(0, 1, 0) * delta * speed;
		}
		if (input->GetKeyState(CROUCH) == InputState::Pressed) {
			transform.Position() -= glm::vec3(0, 1, 0) * delta * speed;
		}
	}
}


void Player::RegisterSerializer() {
	Serializer& S = Serializer::getInstance();
	ComposeFunction entityCompose = [&](Node* node) {
		Player* entity = (Player*)node;
		nlohmann::json json;

		json["info"] = "some player information";

		json.merge_patch(S.Compose("Entity", node));

		return json;
	};
	ParseFunction entityParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
		Player* player = (Player*)node;
		if (player == nullptr) {
			player = new Player(nullptr, Material());
		}

		LOG_F(INFO, "info from the player: %s", data["info"].dump().c_str());

		S.Parse("Entity", data, player, parent);

		return player;
	};
	S.RegisterParser("Player", entityCompose, entityParse);
}

/*


	  {
		"Geometry": "monkey",
		"Material": {
		  "Color": [ 1.0, 1.0, 1.0, 1.0 ],
		  "DiffuseMap": "uvmap",
		  "Shaders": [ "vertex.vert", "fragment.frag" ],
		  "Shininess": 25.0,
		  "Specular": [ 0.5, 0.5, 0.5, 0.5 ]
		},
		"Transform": {
		  "position": [ 0.0, 0.0, 0.0 ],
		  "rotation": [ 0.0, 1.0, 0.0, 0.0 ],
		  "scale": [ 1.0, 1.0, 1.0 ]
		},
		"Type": "Player",
		"children": [
		  {
			"Settings": {
			  "Mode": "Perspective",
			  "Settings": {
				"FOV": 45.0,
				"FarPlane": 100.0,
				"NearPlane": 0.10000000149011612
			  }
			},
			"Transform": {
			  "position": [ 0.0, 3.0, -5.0 ],
			  "rotation": [ 0.0, 0.9659258127212524, 0.258819043636322, 0.0 ],
			  "scale": [ 1.0, 1.0, 1.0 ]
			},
			"Type": "Camera",
			"children": [],
			"name": "camera 1"
		  }
		],
		"flags": 0,
		"info": "some player information",
		"name": "player"
	  },

*/