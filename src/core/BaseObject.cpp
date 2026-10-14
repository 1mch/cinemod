#include "BaseObject.hpp"

namespace CineMod {

    uint64_t BaseObject::s_nextId = 1;

    BaseObject::BaseObject(const std::string& name, ObjectType type)
        : m_id(s_nextId++), m_name(name), m_type(type)
    {
    }

    void BaseObject::AddChild(std::unique_ptr<BaseObject> child) {
        if (!child) return;
        child->m_parent = this;
        m_children.push_back(std::move(child));
    }

    glm::mat4 BaseObject::GetLocalMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
        glm::mat4 rotation = HPBToMatrix(m_rotationHPB);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);
        return translation * rotation * scale;
    }

    glm::mat4 BaseObject::GetWorldMatrix() const {
        if (m_parent) {
            return m_parent->GetWorldMatrix() * GetLocalMatrix();
        }
        return GetLocalMatrix();
    }

    void BaseObject::CycleEditorVisibility() {
        switch (m_editorVisibility) {
            case VisibilityMode::Default: m_editorVisibility = VisibilityMode::Visible; break;
            case VisibilityMode::Visible: m_editorVisibility = VisibilityMode::Hidden; break;
            case VisibilityMode::Hidden:  m_editorVisibility = VisibilityMode::Default; break;
        }
    }

    void BaseObject::CycleRenderVisibility() {
        switch (m_renderVisibility) {
            case VisibilityMode::Default: m_renderVisibility = VisibilityMode::Visible; break;
            case VisibilityMode::Visible: m_renderVisibility = VisibilityMode::Hidden; break;
            case VisibilityMode::Hidden:  m_renderVisibility = VisibilityMode::Default; break;
        }
    }

} // namespace CineMod
