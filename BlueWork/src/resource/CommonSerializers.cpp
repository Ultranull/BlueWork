#include "CommonSerializers.h"

#include <loguru.hpp>

#include "resource/Serializer.h"
#include "resource/Resource.h"

#include "scene/Entity.h"
#include "scene/Camera.h"
#include "scene/Light.h"

template <> 
nlohmann::json Serializer::GeneralCompose(glm::vec3 object) {
  nlohmann::json json = {object.x, object.y, object.z};
  return json;
}

template <>
glm::vec3 Serializer::GeneralParse(nlohmann::json json) {
    return glm::vec3(json[0].get<double>(), json[1].get<double>(), json[2].get<double>());
}

template <> 
nlohmann::json Serializer::GeneralCompose(glm::vec4 object) {
  nlohmann::json json = {
      object.x, object.y, object.z, object.w};
  return json;
}

template <>
glm::vec4 Serializer::GeneralParse(nlohmann::json json) {
    return glm::vec4(json[0].get<double>(), json[1].get<double>(), json[2].get<double>(), json[3].get<double>());
}

template <> 
nlohmann::json Serializer::GeneralCompose(glm::quat object) {
  nlohmann::json json = {
      object.x, object.y, object.z, object.w};
  return json;
}

template <>
glm::quat Serializer::GeneralParse(nlohmann::json json) {
    return glm::quat(json[3].get<double>() , json[0].get<double>(), json[1].get<double>(), json[2].get<double>());
}

template<>
nlohmann::json Serializer::GeneralCompose(Program object) {
    nlohmann::json json;
    Shader vertex = object.GetShader(GL_VERTEX_SHADER),
        fragment = object.GetShader(GL_FRAGMENT_SHADER),
        geom = object.GetShader(GL_GEOMETRY_SHADER);

    if (vertex.id != -1) {
        json.push_back(Resource::getInstance().GetShaderName(vertex));
    }
    if (fragment.id != -1) {
        json.push_back(Resource::getInstance().GetShaderName(fragment));
    }
    if (geom.id != -1) {
        json.push_back(Resource::getInstance().GetShaderName(geom));
    }

    return json;
}

template <>
Program Serializer::GeneralParse(nlohmann::json json) {
    if (json.size() == 2) {
        return Program(
            Resource::getInstance().getShader(json[0]),
            Resource::getInstance().getShader(json[1]));
    }
    else if (json.size() == 3) {
        return Program(
            Resource::getInstance().getShader(json[0]),
            Resource::getInstance().getShader(json[1]),
            Resource::getInstance().getShader(json[2]));
    }
    LOG_F(ERROR, "Could not parse program: %s", json.dump().c_str());
    return Program();
}

template<>
nlohmann::json Serializer::GeneralCompose(Material object) {
    nlohmann::json json;
    json["DiffuseMap"] = Resource::getInstance()
        .GetTextureName(object.diffuseMap);

    json["Color"] = Serializer::GeneralCompose(object.color);
    json["Specular"] = Serializer::GeneralCompose(object.specular);
    json["Shininess"] = object.shininess;


    json["Shaders"] = Serializer::GeneralCompose(object.shader);

    return json;
}

template <>
Material Serializer::GeneralParse(nlohmann::json json) {
    Material mat;
    mat.diffuseMap = Resource::getInstance().getTexture(json["DiffuseMap"]);
    mat.color = Serializer::GeneralParse<glm::vec4>(json["Color"]);
    mat.specular = Serializer::GeneralParse<glm::vec4>(json["Specular"]);
    mat.shininess = json["Shininess"].get<float>();
    mat.shader = Serializer::GeneralParse<Program>(json["Shaders"]);
    return mat;
}

template<>
nlohmann::json Serializer::GeneralCompose(CameraSettings object) {
    nlohmann::json json;

    switch (object.mode)
    {
    case CameraSettings::Mode::Perspective:
        json["Mode"] = "Perspective";
        json["Settings"] = {
            {"NearPlane", object.PerspecitveData.NearPlane},
            {"FarPlane", object.PerspecitveData.FarPlane},
            {"FOV", object.PerspecitveData.FOV}
        };
        break;

    case CameraSettings::Mode::Orthographic:
        json["Mode"] = "Orthographic";
        json["Settings"] = {
            {"NearPlane", object.OrthographicData.NearPlane},
            {"FarPlane", object.OrthographicData.FarPlane},
            {"Left", object.OrthographicData.Left},
            {"Right", object.OrthographicData.Right},
            {"Bottom", object.OrthographicData.Bottom},
            {"Top", object.OrthographicData.Top},
        };
        break;
    }

    return json;
}

template <>
CameraSettings Serializer::GeneralParse(nlohmann::json json) {
    CameraSettings settings;
    std::string modeStr = json["Mode"].get<std::string>();
    if (modeStr.compare("Orthographic") == 0) {
        settings.mode = CameraSettings::Mode::Orthographic;
    }
    else if (modeStr.compare("Perspective") == 0) {
        settings.mode = CameraSettings::Mode::Perspective;
    }

    switch (settings.mode)
    {
    case CameraSettings::Mode::Perspective:
        settings.PerspecitveData.NearPlane = json["Settings"]["NearPlane"].get<float>();
        settings.PerspecitveData.FarPlane = json["Settings"]["FarPlane"].get<float>();
        settings.PerspecitveData.FOV = json["Settings"]["FOV"].get<float>();
        break;

    case CameraSettings::Mode::Orthographic:
        settings.OrthographicData.NearPlane = json["Settings"]["NearPlane"].get<float>();
        settings.OrthographicData.FarPlane = json["Settings"]["FarPlane"].get<float>();
        settings.OrthographicData.Left = json["Settings"]["Left"].get<float>();
        settings.OrthographicData.Right = json["Settings"]["Right"].get<float>();
        settings.OrthographicData.Bottom = json["Settings"]["Bottom"].get<float>();
        settings.OrthographicData.Top = json["Settings"]["Top"].get<float>();
        break;
    }

    return settings;
}

template<>
nlohmann::json Serializer::GeneralCompose(Transform object) {
    nlohmann::json json;
    json["position"] = Serializer::GeneralCompose(object.Position());
    json["scale"] = Serializer::GeneralCompose(object.Scale());
    json["rotation"] = Serializer::GeneralCompose(object.Rotation());
    return json;
}

template <>
Transform Serializer::GeneralParse(nlohmann::json json) {
    Transform transform;
    transform.Position() = Serializer::GeneralParse<glm::vec3>(json["position"]);
    transform.Scale() = Serializer::GeneralParse<glm::vec3>(json["scale"]);
    transform.Rotation() = Serializer::GeneralParse<glm::quat>(json["rotation"]);
    return transform;
}

template<>
nlohmann::json Serializer::GeneralCompose(Light::attunation object) {
    nlohmann::json json;
    json["quadratic"] = object.quadratic;
    json["linear"] = object.linear;
    json["constant"] = object.constant;
    json["intensity"] = object.intensity;
    return json;
}

template <>
Light::attunation Serializer::GeneralParse(nlohmann::json json) {
    Light::attunation att;
    att.quadratic = json["quadratic"].get<float>();
    att.linear = json["linear"].get<float>();
    att.constant = json["constant"].get<float>();
    att.intensity = json["intensity"].get<float>();
    return att;
}

void CommonSerializers::CommonParsers() {
    Resource &R = Resource::getInstance();
    Serializer &S = Serializer::getInstance();

    ////----------- Node
    {
        ComposeFunction nodeCompose = [&](Node* node) {
            nlohmann::json json;
            json["Type"] = node->GetTypeName();
            json["Transform"] = S.GeneralCompose<Transform>(node->transform);
            json["name"] = node->getName();

            nlohmann::json childrenArray = nlohmann::json::array();
            for (int i = 0; i < node->getNumberOfChildren(); i++) {
                Node* child = node->child(i);
                childrenArray.push_back(S.Compose(child));
            }
            json["children"] = childrenArray;
            return json;
        };
        ParseFunction nodeParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
            if (node == nullptr) {
                node = new Node();
            }

            node->setName(data["name"].get<std::string>());
            node->transform = S.GeneralParse<Transform>(data["Transform"]);

            if (parent != nullptr) {
                parent->add(node);
            }   

            nlohmann::json childrenArray = data["children"];
            for (int i = 0; i < childrenArray.size(); i++) {
                nlohmann::json childJson = childrenArray[i];
                std::string type = childJson["Type"].get<std::string>();
                Node* child = S.Parse(type, childJson, nullptr, node);
                if (child == nullptr) {
                    LOG_F(ERROR, "got nullptr with %s: %s", type.c_str(), childJson.dump().c_str());
                }
            }
            return node;
        };
        S.RegisterParser("Node", nodeCompose, nodeParse);
    }
    ////----------- Entity
    {
        ComposeFunction entityCompose = [&](Node* node) {
            Entity* entity = (Entity*)node;
            nlohmann::json json;

            json["flags"] = entity->flags;
            json["Geometry"] = R.GetGeometryName(entity->geometry);
            json["Material"] = S.GeneralCompose<Material>(entity->material);

            json.merge_patch(S.Compose("Node", node));

            return json;
        };
        ParseFunction entityParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
            Entity* entity = (Entity*) node;
            if (entity == nullptr) {
                entity = new Entity();
            }

            entity->flags = data["flags"].get<unsigned char>();
            entity->geometry = R.getGeometry(data["Geometry"].get<std::string>());
            entity->material = S.GeneralParse<Material>(data["Material"]);
            
            S.Parse("Node", data, entity, parent);

            return entity;
        };
        S.RegisterParser("Entity", entityCompose, entityParse);
    }
    ////----------- Camera
    {
        ComposeFunction cameraCompose = [&](Node* node) {
            Camera* camera = (Camera*)node;
            nlohmann::json json;

            json["Settings"] = S.GeneralCompose(camera->GetSettings());

            json.merge_patch(S.Compose("Node", node));

            return json;
        };
        ParseFunction cameraParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
            Camera* camera = (Camera*)node;
            if (camera == nullptr) {
                camera = new Camera();
            }

            camera->SetSettings(S.GeneralParse<CameraSettings>(data["Settings"]));

            S.Parse("Node", data, camera, parent);

            return camera;
        };
        S.RegisterParser("Camera", cameraCompose, cameraParse);
    }
    ////----------- PointLight
    {
        ComposeFunction compose = [&](Node* node) {
            PointLight* light = (PointLight*)node;
            nlohmann::json json;

            json["Ambient"] = S.GeneralCompose(light->data.ambient);
            json["Color"] = S.GeneralCompose(light->data.color);
            json["Specular"] = S.GeneralCompose(light->data.specular);
            json["Att"] = S.GeneralCompose(light->data.att);

            json.merge_patch(S.Compose("Node", node));

            return json;
        };
        ParseFunction parse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
            PointLight* light = (PointLight*)node;
            if (light == nullptr) {
                light = new PointLight();
            }

            light->data.ambient = S.GeneralParse<glm::vec4>(data["Ambient"]);
            light->data.color = S.GeneralParse<glm::vec4>(data["Color"]);
            light->data.specular = S.GeneralParse<glm::vec4>(data["Specular"]);
            light->data.att = S.GeneralParse<Light::attunation>(data["Att"]);

            S.Parse("Node", data, light, parent);

            return light;
        };
        S.RegisterParser("PointLight", compose, parse);
    }

}
