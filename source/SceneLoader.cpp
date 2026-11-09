#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Disk.h"
#include "tinyxml2.h"

#include <iostream>
#include <cmath>

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

    // 4. 加载 Objects
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
