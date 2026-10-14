#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/Types.hpp"

namespace CineMod {

    class Camera {
    public:
        Camera();

        void SetViewportSize(float width, float height);
        
        // Cinema 4D Navigation
        void Pan(float deltaX, float deltaY);     // Shortcut 1 or MMB drag
        void Zoom(float delta);                  // Shortcut 2 or Alt+RMB
        void Orbit(float deltaX, float deltaY);   // Shortcut 3 or Alt+LMB

        void FrameAll();                          // Shortcut 'H'
        void FrameSelected(const glm::vec3& targetPos); // Shortcut 'S'

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::vec3 GetPosition() const;
        const glm::vec3& GetTarget() const { return m_target; }

        Ray GetRay(float pixelX, float pixelY, float width, float height) const;

        float GetFov() const { return m_fov; }
        float GetDistance() const { return m_distance; }

    private:
        void UpdateVectors();

        glm::vec3 m_target{ 0.0f, 0.0f, 0.0f }; // Pivot point
        float m_yaw = -45.0f;                   // Heading in degrees
        float m_pitch = 30.0f;                  // Elevation in degrees
        float m_distance = 600.0f;              // Distance to pivot

        float m_fov = 45.0f;
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearPlane = 1.0f;
        float m_farPlane = 50000.0f;

        glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };
    };

} // namespace CineMod
