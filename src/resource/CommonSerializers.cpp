#include "CommonSerializers.h"

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
    return glm::vec3(json[0], json[1], json[2]);
}

template <> 
nlohmann::json Serializer::GeneralCompose(glm::vec4 object) {
  nlohmann::json json = {
      object.x, object.y, object.z, object.w};
  return json;
}

template <>
glm::vec4 Serializer::GeneralParse(nlohmann::json json) {
    return glm::vec4(json[0], json[1], json[2], json[3]);
}

template <> 
nlohmann::json Serializer::GeneralCompose(glm::quat object) {
  nlohmann::json json = {
      object.x, object.y, object.z, object.w};
  return json;
}

template <>
glm::quat Serializer::GeneralParse(nlohmann::json json) {
    return glm::quat(json[0], json[1], json[2], json[3]);
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
            {"Left ", object.OrthographicData.Left},
            {"Right ", object.OrthographicData.Right},
            {"Bottom", object.OrthographicData.Bottom},
            {"Top", object.OrthographicData.Top},
        };
        break;
    }

    return json;
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
    return json;
}



void CommonSerializers::CommonParsers() {
    Resource &R = Resource::getInstance();
    Serializer &S = Serializer::getInstance();

    ////----------- Node
    {
        ComposeFunction nodeCompose = [&](Node* node) {
            nlohmann::json json;

            json["Id"] = node->GetId();
            json["Transform"] = S.GeneralCompose<Transform>(node->transform);
            json["name"] = node->getName();

            unsigned int parentId = node->GetParentId();
            if (parentId == -1) {
                // this will show -1 in the json instead of the unsigned equivilent
                json["parent"] = -1; 
            }
            else {
                json["parent"] = parentId;
            }

            nlohmann::json childrenArray = nlohmann::json::array();
            for (int i = 0; i < node->getNumberOfChildren(); i++) {
                Node* child = node->child(i);
                childrenArray.push_back(child->GetId());
            }
            json["children"] = childrenArray;
            return json;
        };
        ParseFunction nodeParse = [&](nlohmann::json data, Node* node) -> Node* {
            Node* newNode = new (node) Node(
                data["Id"].get<unsigned int>(),
                "Node",
                NodeType::Node);

            node->setName(data["name"].get<std::string>());
            node->transform = S.GeneralParse<Transform>(data["transform"]);
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
        ParseFunction entityParse = [&](nlohmann::json data, Node* node) -> Node* {
            Entity* entity = new (node) Entity();
            return nullptr;
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
        ParseFunction cameraParse = [&](nlohmann::json data) -> Node* {
            return nullptr;
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
        ParseFunction parse = [&](nlohmann::json data) -> Node* {
            return nullptr;
        };
        S.RegisterParser("PointLight", compose, parse);
    }

}
