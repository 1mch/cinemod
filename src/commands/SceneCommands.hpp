#pragma once

#include "CommandManager.hpp"
#include "scene/Scene.hpp"

namespace CineMod {

    // Command for interactive Transformation changes (Move, Rotate, Scale)
    class TransformCommand : public ICommand {
    public:
        TransformCommand(Scene& scene, uint64_t objectId,
                         const glm::vec3& oldPos, const glm::vec3& oldRot, const glm::vec3& oldScale,
                         const glm::vec3& newPos, const glm::vec3& newRot, const glm::vec3& newScale)
            : m_scene(scene), m_objectId(objectId),
              m_oldPos(oldPos), m_oldRot(oldRot), m_oldScale(oldScale),
              m_newPos(newPos), m_newRot(newRot), m_newScale(newScale)
        {
        }

        void Execute() override {
            BaseObject* obj = m_scene.FindObjectById(m_objectId);
            if (obj) {
                obj->SetPosition(m_newPos);
                obj->SetRotationHPB(m_newRot);
                obj->SetScale(m_newScale);
            }
        }

        void Undo() override {
            BaseObject* obj = m_scene.FindObjectById(m_objectId);
            if (obj) {
                obj->SetPosition(m_oldPos);
                obj->SetRotationHPB(m_oldRot);
                obj->SetScale(m_oldScale);
            }
        }

        std::string GetName() const override { return "Transform Object"; }

    private:
        Scene& m_scene;
        uint64_t m_objectId;
        glm::vec3 m_oldPos, m_oldRot, m_oldScale;
        glm::vec3 m_newPos, m_newRot, m_newScale;
    };

    // Command for creating an object
    class AddObjectCommand : public ICommand {
    public:
        AddObjectCommand(Scene& scene, const std::string& name, ObjectType type)
            : m_scene(scene), m_name(name), m_type(type), m_createdId(0)
        {
        }

        void Execute() override {
            BaseObject* obj = m_scene.AddObject(m_name, m_type);
            if (obj) {
                m_createdId = obj->GetId();
                if (m_type == ObjectType::Cube || m_type == ObjectType::Sphere || m_type == ObjectType::Plane) {
                    obj->AddTag({ "Phong", "Phong" });
                    obj->AddTag({ "Material", "Material" });
                }
            }
        }

        void Undo() override {
            if (m_createdId != 0) {
                m_scene.RemoveObject(m_createdId);
                m_createdId = 0;
            }
        }

        std::string GetName() const override { return "New " + m_name; }

    private:
        Scene& m_scene;
        std::string m_name;
        ObjectType m_type;
        uint64_t m_createdId;
    };

    // Command for duplicating an object
    class DuplicateObjectCommand : public ICommand {
    public:
        DuplicateObjectCommand(Scene& scene, uint64_t sourceId)
            : m_scene(scene), m_sourceId(sourceId), m_duplicateId(0)
        {
        }

        void Execute() override {
            BaseObject* src = m_scene.FindObjectById(m_sourceId);
            if (src) {
                BaseObject* dup = m_scene.DuplicateObject(src);
                if (dup) {
                    m_duplicateId = dup->GetId();
                }
            }
        }

        void Undo() override {
            if (m_duplicateId != 0) {
                m_scene.RemoveObject(m_duplicateId);
                m_duplicateId = 0;
            }
        }

        std::string GetName() const override { return "Duplicate Object"; }

    private:
        Scene& m_scene;
        uint64_t m_sourceId;
        uint64_t m_duplicateId;
    };

    // Command for deleting an object with Undo support
    class DeleteObjectCommand : public ICommand {
    public:
        DeleteObjectCommand(Scene& scene, uint64_t objectId)
            : m_scene(scene), m_objectId(objectId)
        {
            BaseObject* obj = m_scene.FindObjectById(m_objectId);
            if (obj) {
                m_objectName = obj->GetName();
            }
        }

        void Execute() override {
            m_detachedObj = m_scene.DetachObject(m_objectId);
        }

        void Undo() override {
            if (m_detachedObj) {
                m_scene.InsertObject(std::move(m_detachedObj));
            }
        }

        std::string GetName() const override { return "Delete " + m_objectName; }

    private:
        Scene& m_scene;
        uint64_t m_objectId;
        std::string m_objectName = "Object";
        std::unique_ptr<BaseObject> m_detachedObj;
    };

} // namespace CineMod
