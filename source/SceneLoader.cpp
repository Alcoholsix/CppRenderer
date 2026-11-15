/**
 * 这个文件里的内容都是对 XML 文件的解析代码
 * 由 AI 生成
 */
#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Disk.h"
#include "tinyxml2.h"

#include <iostream>
#include <cmath>
#include <cstring>

using namespace tinyxml2;

namespace
{
    // 读取必需的 XML 子节点
    XMLElement *GetRequiredElement(XMLElement *parent, const char *name)
    {
        XMLElement *element = parent ? parent->FirstChildElement(name) : nullptr;

        if (!element)
        {
            std::cerr << "XML Error: Missing element <" << name << ">\n";
        }

        return element;
    }

    // 读取必需的 float 属性
    bool ReadFloat(XMLElement *element, const char *attribute, float &value)
    {
        if (!element || element->QueryFloatAttribute(attribute, &value) != XML_SUCCESS)
        {
            std::cerr << "XML Error: Missing or invalid attribute '" << attribute << "'\n";
            return false;
        }

        return true;
    }

    // 读取三维向量
    bool ReadVector3(XMLElement *element, Vector3f &value)
    {
        if (!element)
        {
            return false;
        }

        return ReadFloat(element, "x", value.x) &&
               ReadFloat(element, "y", value.y) &&
               ReadFloat(element, "z", value.z);
    }

    // 读取颜色
    bool ReadColor(XMLElement *element, Color &color)
    {
        if (!element)
        {
            return false;
        }

        return ReadFloat(element, "r", color.r) &&
               ReadFloat(element, "g", color.g) &&
               ReadFloat(element, "b", color.b);
    }

    // 读取光源衰减系数
    bool ReadAttenuations(XMLElement *element, Vector3f &attenuations)
    {
        return ReadVector3(element, attenuations);
    }

    // 读取 Transform
    bool ReadTransform(XMLElement *object, Vector3f &position, Vector3f &rotation, float &scale)
    {
        XMLElement *transform = GetRequiredElement(object, "Transform");

        if (!transform)
        {
            return false;
        }

        XMLElement *posElement = GetRequiredElement(transform, "Position");
        XMLElement *rotElement = GetRequiredElement(transform, "Rotation");
        XMLElement *scaleElement = GetRequiredElement(transform, "Scale");

        if (!posElement || !rotElement || !scaleElement)
        {
            return false;
        }

        if (!ReadVector3(posElement, position) || !ReadVector3(rotElement, rotation))
        {
            return false;
        }

        if (!ReadFloat(scaleElement, "value", scale))
        {
            return false;
        }

        if (scale <= 0.0f)
        {
            std::cerr << "XML Error: Scale must be greater than zero\n";
            return false;
        }

        return true;
    }

    // 加载光源
    bool LoadLights(Scene *scene, XMLElement *lightsElement)
    {
        if (!lightsElement)
        {
            return true;
        }

        for (XMLElement *lightElement = lightsElement->FirstChildElement("Light");
             lightElement;
             lightElement = lightElement->NextSiblingElement("Light"))
        {
            const char *type = lightElement->Attribute("type");

            if (!type)
            {
                std::cerr << "XML Error: Light is missing type attribute\n";
                return false;
            }

            // ==================== Directional Light ====================
            if (strcmp(type, "Directional") == 0)
            {
                Vector3f direction;
                Color radiance;

                if (!ReadVector3(GetRequiredElement(lightElement, "Direction"), direction) ||
                    !ReadColor(GetRequiredElement(lightElement, "Radiance"), radiance))
                {
                    return false;
                }

                if (glm::length(direction) < 1e-6f)
                {
                    std::cerr << "XML Error: Directional light direction cannot be zero\n";
                    return false;
                }

                scene->CreateLight<DirectionalLight>(direction, radiance);
            }

            // ==================== Point Light ====================
            else if (strcmp(type, "Point") == 0)
            {
                Vector3f position;
                Vector3f attenuations;
                Color intensity;

                if (!ReadVector3(GetRequiredElement(lightElement, "Position"), position) ||
                    !ReadColor(GetRequiredElement(lightElement, "Intensity"), intensity) ||
                    !ReadAttenuations(GetRequiredElement(lightElement, "Attenuations"), attenuations))
                {
                    return false;
                }

                if (attenuations.x < 0.0f || attenuations.y < 0.0f ||
                    attenuations.z <= 0.0f)
                {
                    std::cerr << "XML Error: Invalid point light attenuation coefficients\n";
                    return false;
                }

                scene->CreateLight<PointLight>(position, intensity, attenuations);
            }

            // ==================== Spot Light ====================
            else if (strcmp(type, "Spot") == 0)
            {
                Vector3f position;
                Vector3f direction;
                Vector3f attenuations;
                Color intensity;

                float innerAngle;
                float outerAngle;

                if (!ReadVector3(GetRequiredElement(lightElement, "Position"), position) ||
                    !ReadVector3(GetRequiredElement(lightElement, "Direction"), direction) ||
                    !ReadColor(GetRequiredElement(lightElement, "Intensity"), intensity) ||
                    !ReadFloat(GetRequiredElement(lightElement, "InnerAngle"), "value", innerAngle) ||
                    !ReadFloat(GetRequiredElement(lightElement, "OuterAngle"), "value", outerAngle) ||
                    !ReadAttenuations(GetRequiredElement(lightElement, "Attenuations"), attenuations))
                {
                    return false;
                }

                if (glm::length(direction) < 1e-6f)
                {
                    std::cerr << "XML Error: Spot light direction cannot be zero\n";
                    return false;
                }

                if (innerAngle < 0.0f || outerAngle <= innerAngle || outerAngle >= 180.0f)
                {
                    std::cerr << "XML Error: Invalid spot light angles\n";
                    return false;
                }

                if (attenuations.x < 0.0f || attenuations.y < 0.0f ||
                    attenuations.z <= 0.0f)
                {
                    std::cerr << "XML Error: Invalid spot light attenuation coefficients\n";
                    return false;
                }

                // XML 使用角度，构造函数接收弧度
                scene->CreateLight<SpotLight>(position, direction, intensity,
                    glm::radians(innerAngle), glm::radians(outerAngle), attenuations);
            }

            else
            {
                std::cerr << "XML Error: Unknown light type '" << type << "'\n";
                return false;
            }
        }

        return true;
    }
}

std::unique_ptr<Scene> Scene::LoadFromXML(const std::string &filepath, int W, int H)
{
    XMLDocument doc;

    // 1. 加载 XML 文件
    if (doc.LoadFile(filepath.c_str()) != XML_SUCCESS)
    {
        std::cerr << "Failed to load scene XML: " << filepath << "\n";
        std::cerr << doc.ErrorStr() << "\n";
        return nullptr;
    }

    // 2. 检查根节点
    XMLElement *root = doc.FirstChildElement("Scene");

    if (!root)
    {
        std::cerr << "XML Error: Missing root element <Scene>\n";
        return nullptr;
    }

    auto scene = std::make_unique<Scene>();

    // 3. 加载 Camera
    XMLElement *cameraElement = GetRequiredElement(root, "Camera");

    if (!cameraElement)
    {
        return nullptr;
    }

    Vector3f cameraPosition;
    Vector3f cameraTarget;
    Vector3f cameraUp;

    if (!ReadVector3(GetRequiredElement(cameraElement, "Position"), cameraPosition) ||
        !ReadVector3(GetRequiredElement(cameraElement, "Target"), cameraTarget) ||
        !ReadVector3(GetRequiredElement(cameraElement, "Up"), cameraUp))
    {
        return nullptr;
    }

    float fov;
    float nearPlane;
    float farPlane;

    if (!ReadFloat(GetRequiredElement(cameraElement, "FOV"), "value", fov) ||
        !ReadFloat(GetRequiredElement(cameraElement, "Near"), "value", nearPlane) ||
        !ReadFloat(GetRequiredElement(cameraElement, "Far"), "value", farPlane))
    {
        return nullptr;
    }

    if (fov <= 0.0f || fov >= 180.0f || nearPlane <= 0.0f || farPlane <= nearPlane)
    {
        std::cerr << "XML Error: Invalid camera parameters\n";
        return nullptr;
    }

    Camera camera;
    camera.Initialize(cameraPosition, cameraTarget, cameraUp, glm::radians(fov), nearPlane, farPlane, W, H);
    scene->SetCamera(camera);

    // 4. 加载 Lights
    XMLElement *lightsElement = root->FirstChildElement("Lights");

    if (!LoadLights(scene.get(), lightsElement))
    {
        return nullptr;
    }

    // 5. 加载 Objects
    XMLElement *objectsElement = GetRequiredElement(root, "Objects");

    if (!objectsElement)
    {
        return nullptr;
    }

    for (XMLElement *objectElement = objectsElement->FirstChildElement("Object");
         objectElement;
         objectElement = objectElement->NextSiblingElement("Object"))
    {
        const char *type = objectElement->Attribute("type");

        if (!type)
        {
            std::cerr << "XML Error: Object is missing type attribute\n";
            return nullptr;
        }

        Vector3f position;
        Vector3f rotation;
        float scale;

        if (!ReadTransform(objectElement, position, rotation, scale))
        {
            return nullptr;
        }

        SceneObject *object = scene->CreateSceneObject(position, rotation, scale);

        // Sphere
        if (strcmp(type, "Sphere") == 0)
        {
            float radius;

            if (!ReadFloat(GetRequiredElement(objectElement, "Radius"), "value", radius) || radius <= 0.0f)
            {
                std::cerr << "XML Error: Invalid sphere radius\n";
                return nullptr;
            }

            object->CreatePrimitive<Sphere>(radius);
        }

        // Disk
        else if (strcmp(type, "Disk") == 0)
        {
            float radius;

            if (!ReadFloat(GetRequiredElement(objectElement, "Radius"), "value", radius) || radius <= 0.0f)
            {
                std::cerr << "XML Error: Invalid disk radius\n";
                return nullptr;
            }

            object->CreatePrimitive<Disk>(radius);
        }

        // Triangle
        else if (strcmp(type, "Triangle") == 0)
        {
            Vector3f v0;
            Vector3f v1;
            Vector3f v2;

            if (!ReadVector3(GetRequiredElement(objectElement, "VertexA"), v0) ||
                !ReadVector3(GetRequiredElement(objectElement, "VertexB"), v1) ||
                !ReadVector3(GetRequiredElement(objectElement, "VertexC"), v2))
            {
                return nullptr;
            }

            object->CreatePrimitive<Triangle>(v0, v1, v2);
        }

        else
        {
            std::cerr << "XML Error: Unknown object type '" << type << "'\n";
            return nullptr;
        }
    }

    std::cout << "Scene loaded successfully: " << filepath << "\n";

    return scene;
}
