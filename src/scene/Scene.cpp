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

    void Scene::RemoveObject(uint64_t id) {
        if (m_selectedObject && m_selectedObject->GetId() == id) {
            m_selectedObject = nullptr;
        }

        // Remove from root
        m_rootObjects.erase(
            std::remove_if(m_rootObjects.begin(), m_rootObjects.end(),
                [id](const std::unique_ptr<BaseObject>& obj) { return obj->GetId() == id; }),
            m_rootObjects.end());
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

} // namespace CineMod
