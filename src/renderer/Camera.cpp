#include "Camera.hpp"
#include <algorithm>

namespace CineMod {

    Camera::Camera() {
        UpdateVectors();
    }

    void Camera::SetViewportSize(float width, float height) {
        if (height > 0.0f) {
            m_aspectRatio = width / height;
        }
    }

    void Camera::Pan(float deltaX, float deltaY) {
        // C4D Pan speed is proportional to distance
        float speed = m_distance * 0.0015f;
        m_target += -m_right * deltaX * speed + m_up * deltaY * speed;
        UpdateVectors();
    }

    void Camera::Zoom(float delta) {
        // C4D Dolly zoom
        float factor = 1.0f - delta * 0.005f;
        m_distance = std::clamp(m_distance * factor, 10.0f, 20000.0f);
        UpdateVectors();
    }

    void Camera::Orbit(float deltaX, float deltaY) {
        // C4D Orbit navigation
        m_yaw += deltaX * 0.5f;
        m_pitch = std::clamp(m_pitch - deltaY * 0.5f, -89.0f, 89.0f);
        UpdateVectors();
    }

    void Camera::FrameAll() {
        m_target = glm::vec3(0.0f, 0.0f, 0.0f);
        m_distance = 600.0f;
        m_yaw = -45.0f;
        m_pitch = 30.0f;
        UpdateVectors();
    }

    void Camera::FrameSelected(const glm::vec3& targetPos) {
        m_target = targetPos;
        m_distance = 400.0f;
        UpdateVectors();
    }

    void Camera::UpdateVectors() {
        float radYaw = glm::radians(m_yaw);
        float radPitch = glm::radians(m_pitch);

        glm::vec3 offset;
        offset.x = m_distance * cos(radPitch) * sin(radYaw);
        offset.y = m_distance * sin(radPitch);
        offset.z = m_distance * cos(radPitch) * cos(radYaw);

        m_position = m_target + offset;

        glm::vec3 forward = glm::normalize(m_target - m_position);
        m_right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up = glm::normalize(glm::cross(m_right, forward));
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return glm::lookAt(m_position, m_target, m_up);
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    }

    glm::vec3 Camera::GetPosition() const {
        return m_position;
    }

    Ray Camera::GetRay(float pixelX, float pixelY, float width, float height) const {
        if (width <= 0.0f || height <= 0.0f) return Ray{ m_position, glm::vec3(0.0f, 0.0f, -1.0f) };

        float ndcX = (2.0f * pixelX) / width - 1.0f;
        float ndcY = 1.0f - (2.0f * pixelY) / height;

        glm::vec4 clipCoords(ndcX, ndcY, -1.0f, 1.0f);
        glm::mat4 invProj = glm::inverse(GetProjectionMatrix());
        glm::vec4 eyeCoords = invProj * clipCoords;
        eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

        glm::mat4 invView = glm::inverse(GetViewMatrix());
        glm::vec3 rayWorld = glm::normalize(glm::vec3(invView * eyeCoords));

        return Ray{ m_position, rayWorld };
    }

} // namespace CineMod
