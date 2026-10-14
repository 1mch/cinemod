#pragma once

#include "core/BaseObject.hpp"
#include <vector>
#include <memory>
#include <string>

namespace CineMod {

    struct Material {
        uint64_t id;
        std::string name;
        glm::vec3 color{ 0.8f, 0.8f, 0.8f };
        float roughness{ 0.2f };
        float reflectance{ 0.5f };
    };

    class Scene {
    public:
        Scene();
        ~Scene() = default;

        // Objects
        BaseObject* AddObject(const std::string& name, ObjectType type, BaseObject* parent = nullptr);
        void RemoveObject(uint64_t id);
        std::unique_ptr<BaseObject> DetachObject(uint64_t id);
        BaseObject* InsertObject(std::unique_ptr<BaseObject> obj, BaseObject* parent = nullptr);
        const std::vector<std::unique_ptr<BaseObject>>& GetRootObjects() const { return m_rootObjects; }

        // Selection & Interaction
        BaseObject* GetSelectedObject() const { return m_selectedObject; }
        void SelectObject(BaseObject* object) { m_selectedObject = object; }
        BaseObject* FindObjectById(uint64_t id);
        BaseObject* PickObject(const Ray& ray);
        BaseObject* DuplicateObject(BaseObject* source);

        // Materials
        Material* AddMaterial(const std::string& name);
        const std::vector<Material>& GetMaterials() const { return m_materials; }
        std::vector<Material>& GetMaterials() { return m_materials; }
        Material* GetSelectedMaterial() { return m_selectedMaterial; }
        void SelectMaterial(Material* mat) { m_selectedMaterial = mat; }
        const Material* FindMaterialById(uint64_t id) const;
        Material* FindMaterialById(uint64_t id);

        // Timeline & Animation
        int GetCurrentFrame() const { return m_currentFrame; }
        void SetCurrentFrame(int frame) { m_currentFrame = frame; }
        int GetMinFrame() const { return m_minFrame; }
        int GetMaxFrame() const { return m_maxFrame; }
        int GetFps() const { return m_fps; }
        bool IsPlaying() const { return m_isPlaying; }
        void SetPlaying(bool playing) { m_isPlaying = playing; }

        // Create default initial scene
        void CreateDefaultScene();

    private:
        BaseObject* FindObjectRecursive(BaseObject* current, uint64_t id);

        std::vector<std::unique_ptr<BaseObject>> m_rootObjects;
        BaseObject* m_selectedObject = nullptr;

        std::vector<Material> m_materials;
        Material* m_selectedMaterial = nullptr;

        int m_currentFrame = 0;
        int m_minFrame = 0;
        int m_maxFrame = 90;
        int m_fps = 30;
        bool m_isPlaying = false;
    };

} // namespace CineMod
