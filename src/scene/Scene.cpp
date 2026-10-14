#include "Scene.hpp"
#include <algorithm>

namespace CineMod {

    Scene::Scene() {
        CreateDefaultScene();
    }

    void Scene::CreateDefaultScene() {
        m_rootObjects.clear();
        m_materials.clear();

        // Add a default Cube
        BaseObject* cube = AddObject("Cube", ObjectType::Cube);
        cube->AddTag({ "Phong", "Phong" });
        cube->AddTag({ "Material", "Material" });

        // Add a default Light
        BaseObject* light = AddObject("Light", ObjectType::Light);
        light->SetPosition(glm::vec3(200.0f, 300.0f, -200.0f));

        // Add a default Camera
        BaseObject* camera = AddObject("Camera", ObjectType::Camera);
        camera->SetPosition(glm::vec3(0.0f, 250.0f, -600.0f));
        camera->SetRotationHPB(glm::vec3(0.0f, -20.0f, 0.0f));

        // Select the cube by default
        m_selectedObject = cube;

        // Default Material
        AddMaterial("Mat");
    }

    BaseObject* Scene::AddObject(const std::string& name, ObjectType type, BaseObject* parent) {
        auto newObj = std::make_unique<BaseObject>(name, type);
        BaseObject* ptr = newObj.get();

        if (parent) {
            parent->AddChild(std::move(newObj));
        } else {
            m_rootObjects.push_back(std::move(newObj));
        }

        m_selectedObject = ptr;
        return ptr;
    }

    std::unique_ptr<BaseObject> Scene::DetachObject(uint64_t id) {
        if (m_selectedObject && m_selectedObject->GetId() == id) {
            m_selectedObject = nullptr;
        }

        for (auto it = m_rootObjects.begin(); it != m_rootObjects.end(); ++it) {
            if ((*it)->GetId() == id) {
                std::unique_ptr<BaseObject> obj = std::move(*it);
                m_rootObjects.erase(it);
                return obj;
            }
        }
        return nullptr;
    }

    BaseObject* Scene::InsertObject(std::unique_ptr<BaseObject> obj, BaseObject* parent) {
        if (!obj) return nullptr;
        BaseObject* ptr = obj.get();
        if (parent) {
            parent->AddChild(std::move(obj));
        } else {
            m_rootObjects.push_back(std::move(obj));
        }
        m_selectedObject = ptr;
        return ptr;
    }

    void Scene::RemoveObject(uint64_t id) {
        DetachObject(id);
    }

    BaseObject* Scene::FindObjectRecursive(BaseObject* current, uint64_t id) {
        if (!current) return nullptr;
        if (current->GetId() == id) return current;
        for (const auto& child : current->GetChildren()) {
            BaseObject* found = FindObjectRecursive(child.get(), id);
            if (found) return found;
        }
        return nullptr;
    }

    BaseObject* Scene::FindObjectById(uint64_t id) {
        for (const auto& root : m_rootObjects) {
            BaseObject* found = FindObjectRecursive(root.get(), id);
            if (found) return found;
        }
        return nullptr;
    }

    Material* Scene::AddMaterial(const std::string& name) {
        static uint64_t s_matId = 1;
        Material mat;
        mat.id = s_matId++;
        mat.name = name;
        mat.color = glm::vec3(0.85f, 0.85f, 0.85f);
        m_materials.push_back(mat);
        m_selectedMaterial = &m_materials.back();
        return m_selectedMaterial;
    }

    const Material* Scene::FindMaterialById(uint64_t id) const {
        for (const auto& mat : m_materials) {
            if (mat.id == id) return &mat;
        }
        return nullptr;
    }

    Material* Scene::FindMaterialById(uint64_t id) {
        for (auto& mat : m_materials) {
            if (mat.id == id) return &mat;
        }
        return nullptr;
    }

    BaseObject* Scene::PickObject(const Ray& ray) {
        BaseObject* closestObj = nullptr;
        float closestDist = std::numeric_limits<float>::max();

        auto CheckObject = [&](auto& self, BaseObject* obj) -> void {
            if (!obj || obj->GetEditorVisibility() == VisibilityMode::Hidden) return;

            glm::mat4 invWorld = glm::inverse(obj->GetWorldMatrix());
            glm::vec3 localOrigin = glm::vec3(invWorld * glm::vec4(ray.origin, 1.0f));
            glm::vec3 localDir = glm::normalize(glm::vec3(invWorld * glm::vec4(ray.direction, 0.0f)));
            Ray localRay{ localOrigin, localDir };

            glm::vec3 minB(-50.0f), maxB(50.0f);
            if (obj->GetType() == ObjectType::Cube) {
                minB = -0.5f * obj->cubeSize;
                maxB =  0.5f * obj->cubeSize;
            } else if (obj->GetType() == ObjectType::Sphere) {
                minB = -glm::vec3(obj->sphereRadius);
                maxB =  glm::vec3(obj->sphereRadius);
            } else if (obj->GetType() == ObjectType::Plane) {
                minB = glm::vec3(-obj->cubeSize.x * 0.5f, -2.0f, -obj->cubeSize.z * 0.5f);
                maxB = glm::vec3( obj->cubeSize.x * 0.5f,  2.0f,  obj->cubeSize.z * 0.5f);
            }

            float tLocal = 0.0f;
            if (localRay.IntersectsAABB(minB, maxB, tLocal)) {
                glm::vec3 hitPointWorld = glm::vec3(obj->GetWorldMatrix() * glm::vec4(localOrigin + localDir * tLocal, 1.0f));
                float dist = glm::distance(ray.origin, hitPointWorld);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestObj = obj;
                }
            }

            for (const auto& child : obj->GetChildren()) {
                self(self, child.get());
            }
        };

        for (const auto& root : m_rootObjects) {
            CheckObject(CheckObject, root.get());
        }

        return closestObj;
    }

    BaseObject* Scene::DuplicateObject(BaseObject* source) {
        if (!source) return nullptr;

        std::string newName = source->GetName() + ".1";
        BaseObject* copy = AddObject(newName, source->GetType());
        copy->SetPosition(source->GetPosition() + glm::vec3(50.0f, 0.0f, 50.0f));
        copy->SetRotationHPB(source->GetRotationHPB());
        copy->SetScale(source->GetScale());
        copy->cubeSize = source->cubeSize;
        copy->cubeSegments = source->cubeSegments;
        copy->cubeFillet = source->cubeFillet;
        copy->cubeFilletRadius = source->cubeFilletRadius;
        copy->sphereRadius = source->sphereRadius;
        copy->sphereSegments = source->sphereSegments;

        for (const auto& tag : source->GetTags()) {
            copy->AddTag(tag);
        }

        m_selectedObject = copy;
        return copy;
    }

} // namespace CineMod
