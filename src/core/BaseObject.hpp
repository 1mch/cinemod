#pragma once

#include "Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace CineMod {

    struct Tag {
        std::string name;
        std::string type; // "Phong", "Material", "Protection", etc.
    };

    class BaseObject {
    public:
        BaseObject(const std::string& name, ObjectType type);
        virtual ~BaseObject() = default;

        uint64_t GetId() const { return m_id; }
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }
        ObjectType GetType() const { return m_type; }

        // Hierarchy
        BaseObject* GetParent() const { return m_parent; }
        const std::vector<std::unique_ptr<BaseObject>>& GetChildren() const { return m_children; }
        void AddChild(std::unique_ptr<BaseObject> child);

        // Transforms
        const glm::vec3& GetPosition() const { return m_position; }
        void SetPosition(const glm::vec3& pos) { m_position = pos; }

        const glm::vec3& GetRotationHPB() const { return m_rotationHPB; }
        void SetRotationHPB(const glm::vec3& hpb) { m_rotationHPB = hpb; }

        const glm::vec3& GetScale() const { return m_scale; }
        void SetScale(const glm::vec3& scale) { m_scale = scale; }

        glm::mat4 GetLocalMatrix() const;
        glm::mat4 GetWorldMatrix() const;

        // Visibility ("Traffic light" dots)
        VisibilityMode GetEditorVisibility() const { return m_editorVisibility; }
        void SetEditorVisibility(VisibilityMode mode) { m_editorVisibility = mode; }
        void CycleEditorVisibility();

        VisibilityMode GetRenderVisibility() const { return m_renderVisibility; }
        void SetRenderVisibility(VisibilityMode mode) { m_renderVisibility = mode; }
        void CycleRenderVisibility();

        bool IsEnabled() const { return m_enabled; }
        void SetEnabled(bool enabled) { m_enabled = enabled; }

        // Tags
        const std::vector<Tag>& GetTags() const { return m_tags; }
        void AddTag(const Tag& tag) { m_tags.push_back(tag); }

        // Parametric properties
        // Cube
        glm::vec3 cubeSize{ 200.0f, 200.0f, 200.0f };
        glm::ivec3 cubeSegments{ 1, 1, 1 };
        bool cubeFillet{ false };
        float cubeFilletRadius{ 10.0f };

        // Sphere
        float sphereRadius{ 100.0f };
        int sphereSegments{ 24 };

    private:
        static uint64_t s_nextId;
        uint64_t m_id;
        std::string m_name;
        ObjectType m_type;

        BaseObject* m_parent = nullptr;
        std::vector<std::unique_ptr<BaseObject>> m_children;

        glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_rotationHPB{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };

        VisibilityMode m_editorVisibility = VisibilityMode::Default;
        VisibilityMode m_renderVisibility = VisibilityMode::Default;
        bool m_enabled = true;

        std::vector<Tag> m_tags;
    };

} // namespace CineMod
