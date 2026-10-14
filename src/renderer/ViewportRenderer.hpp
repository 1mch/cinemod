#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Camera.hpp"
#include "core/Types.hpp"
#include "scene/Scene.hpp"

namespace CineMod {

    class ViewportRenderer {
    public:
        ViewportRenderer();
        ~ViewportRenderer();

        void Init();
        void Resize(int width, int height);
        void Render(const Scene& scene, ToolMode currentTool);

        GLuint GetTextureId() const { return m_colorTexture; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        Camera& GetCamera() { return m_camera; }
        const Camera& GetCamera() const { return m_camera; }

    private:
        void CreateFramebuffer();
        void CreateShaders();
        void CreateCubeMesh();
        void CreateGridMesh();

        void RenderGrid(const glm::mat4& viewProj);
        void RenderSceneObjects(const Scene& scene, const glm::mat4& viewProj);
        void RenderGizmo(const BaseObject* selectedObj, const glm::mat4& viewProj, ToolMode currentTool);

        int m_width = 1280;
        int m_height = 720;

        GLuint m_fbo = 0;
        GLuint m_colorTexture = 0;
        GLuint m_rbo = 0;

        Camera m_camera;

        // Shaders
        GLuint m_shaderBasic = 0;
        GLuint m_shaderLine = 0;

        // Meshes
        GLuint m_cubeVao = 0, m_cubeVbo = 0, m_cubeEbo = 0;
        GLuint m_cubeLinesVao = 0, m_cubeLinesVbo = 0, m_cubeLinesEbo = 0;
        GLuint m_gridVao = 0, m_gridVbo = 0;
        int m_gridVertexCount = 0;

        // Gizmo
        GLuint m_gizmoVao = 0, m_gizmoVbo = 0;
    };

} // namespace CineMod
