#pragma once

#include <map>
#include <functional>
#include <string>

#include <json.hpp>

#include "scene/Node.h"

#define ComposeFunction std::function<nlohmann::json(Node*)>
#define ParseFunction std::function<Node*(nlohmann::json,Node*,Node*)>

class SceneManager;

struct ParseIdentity {
	ComposeFunction Compose;
	ParseFunction Parse;
};

class Serializer{

	std::map<std::string, ParseIdentity> ParseMap;

public:

	static std::string AsciiExtention;

	Serializer();

	void Initialize();

	nlohmann::json Compose(Node* node);
	nlohmann::json Compose(std::string type, Node* node);
	Node* Parse(std::string type, nlohmann::json json); // should use json pointer maybe?
	Node* Parse(std::string type, nlohmann::json json, Node* node, Node* parent);


	void SaveFile(std::string fileName, SceneManager* manager);

	void RegisterParser(std::string name, ComposeFunction compose, ParseFunction parse);

	template<typename T>
	T GeneralParse(nlohmann::json data);

	template<typename T>
	nlohmann::json GeneralCompose(T object);

	static Serializer& getInstance() {
		static Serializer instance;
		return instance;
	}
	Serializer(Serializer const&) = delete;
	void operator=(Serializer const&) = delete;
};

/* new structure

	{
		"Manifest": [
			{
				"asset": "whatever.txt",
				"name": "whatever", // optional, if empty use filename
				"metadata": "this is extra args maybe" // optional, maybe more json
			},


		],

		"Scene": [
			{
				"Parsers":[ "Node", "Entity" ], // not going to do bad idea
				"Transform":{
					"position":{"x":0.0, "y":0.0, "z":0.0},
					"scale":{"x":0.0, "y":0.0, "z":0.0},
					"rotation":{"x":0.0, "y":0.0, "z":0.0, "w":0.0},
				},
				"name":"bob",
				"parent": <id> // got noped
				"children":[<id>, <id>, <id>, ...]
				"flags":0b0,
				"Geometry": "name of geometry",
				"Material"{
					"diffuseMap": "name",
					"shader": "name",
					"resource":"mat from manifest", // optional, if here use mat from manifest
					<other values>
				}
			}
		]
	
	
	}

*/

/* scene file structure

{
	"manifest":"mesh.obj:meshName;",
	"root": {
		
	}
}

		{
			Name:"Node",
			
				"Transform":{
					"position":{"x":0.0, "y":0.0, "z":0.0},
					"scale":{"x":0.0, "y":0.0, "z":0.0},
					"rotation":{"x":0.0, "y":0.0, "z":0.0, "w":0.0},
				},
				"name":"bob",
				"parent": <id>
				"children":[<id>, <id>, <id>, ...]
			
		},
		"Light":{
			<Node stuff>
			"LightType": "Point",
			"PointData":{
				...
			}
		},
		"Entity":{
			<Node stuff>
			"flags":0b0,
			"Geometry": "name of geometry",
			"Material"{
				"diffuseMap": "name",
				"shader": "name",
				<other values>
			}
		},
		"Camera":{
			<Node stuff>
			"Mode": "perspective",
			"persective data":{
				"NearPlane":0,
				"FarPlane":0,
				"FOV":0
			}
		}

*/



