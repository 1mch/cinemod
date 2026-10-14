#include "ViewportRenderer.hpp"
#include <vector>
#include <iostream>

namespace CineMod {

    static GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compile error: " << infoLog << std::endl;
        }
        return shader;
    }

    static GLuint CreateProgram(const char* vsSrc, const char* fsSrc) {
        GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc);
        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);
        return program;
    }

    ViewportRenderer::ViewportRenderer() {
    }

    ViewportRenderer::~ViewportRenderer() {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_colorTexture) glDeleteTextures(1, &m_colorTexture);
        if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);

        if (m_cubeVao) glDeleteVertexArrays(1, &m_cubeVao);
        if (m_cubeVbo) glDeleteBuffers(1, &m_cubeVbo);
        if (m_cubeEbo) glDeleteBuffers(1, &m_cubeEbo);

        if (m_sphereVao) glDeleteVertexArrays(1, &m_sphereVao);
        if (m_sphereVbo) glDeleteBuffers(1, &m_sphereVbo);
        if (m_sphereEbo) glDeleteBuffers(1, &m_sphereEbo);

        if (m_planeVao) glDeleteVertexArrays(1, &m_planeVao);
        if (m_planeVbo) glDeleteBuffers(1, &m_planeVbo);
        if (m_planeEbo) glDeleteBuffers(1, &m_planeEbo);

        if (m_gridVao) glDeleteVertexArrays(1, &m_gridVao);
        if (m_gridVbo) glDeleteBuffers(1, &m_gridVbo);

        if (m_shaderBasic) glDeleteProgram(m_shaderBasic);
        if (m_shaderLine) glDeleteProgram(m_shaderLine);
    }

    void ViewportRenderer::Init() {
        CreateFramebuffer();
        CreateShaders();
        CreateCubeMesh();
        CreateSphereMesh();
        CreatePlaneMesh();
        CreateGridMesh();
    }

    void ViewportRenderer::CreateFramebuffer() {
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            glDeleteTextures(1, &m_colorTexture);
            glDeleteRenderbuffers(1, &m_rbo);
        }

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // Color texture
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

        // Depth & Stencil Renderbuffer
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ViewportRenderer::Resize(int width, int height) {
        if (width <= 0 || height <= 0) return;
        if (m_width == width && m_height == height) return;

        m_width = width;
        m_height = height;
        m_camera.SetViewportSize(static_cast<float>(width), static_cast<float>(height));
        CreateFramebuffer();
    }

    void ViewportRenderer::CreateShaders() {
        // Basic 3D Mesh Shader with simple lighting
        const char* vsBasic = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;

            uniform mat4 uModel;
            uniform mat4 uViewProj;

            out vec3 vNormal;
            out vec3 vFragPos;

            void main() {
                vec4 worldPos = uModel * vec4(aPos, 1.0);
                vFragPos = worldPos.xyz;
                vNormal = mat3(transpose(inverse(uModel))) * aNormal;
                gl_Position = uViewProj * worldPos;
            }
        )";

        const char* fsBasic = R"(
            #version 330 core
            in vec3 vNormal;
            in vec3 vFragPos;
            out vec4 FragColor;

            uniform vec3 uColor;
            uniform vec3 uLightDir;
            uniform vec3 uViewPos;

            void main() {
                vec3 norm = normalize(vNormal);
                vec3 lightDir = normalize(uLightDir);

                // Ambient
                float ambient = 0.25;

                // Diffuse
                float diff = max(dot(norm, lightDir), 0.0);

                // Specular
                vec3 viewDir = normalize(uViewPos - vFragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * 0.3;

                vec3 result = (ambient + diff * 0.75 + spec) * uColor;
                FragColor = vec4(result, 1.0);
            }
        )";

        m_shaderBasic = CreateProgram(vsBasic, fsBasic);

        // Line / Grid Shader
        const char* vsLine = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;

            uniform mat4 uViewProj;
            uniform mat4 uModel;

            out vec3 vColor;

            void main() {
                vColor = aColor;
                gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
            }
        )";

        const char* fsLine = R"(
            #version 330 core
            in vec3 vColor;
            out vec4 FragColor;

            void main() {
                FragColor = vec4(vColor, 1.0);
            }
        )";

        m_shaderLine = CreateProgram(vsLine, fsLine);
    }

    void ViewportRenderer::CreateCubeMesh() {
        // Standard Unit Cube [-0.5, 0.5] with 24 vertices for hard normals
        float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            // Back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            // Top face
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            // Right face
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            // Left face
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f
        };

        unsigned int indices[] = {
            0, 1, 2,  2, 3, 0,
            4, 5, 6,  6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        glGenVertexArrays(1, &m_cubeVao);
        glGenBuffers(1, &m_cubeVbo);
        glGenBuffers(1, &m_cubeEbo);

        glBindVertexArray(m_cubeVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void ViewportRenderer::CreateSphereMesh() {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 32;
        const unsigned int Y_SEGMENTS = 16;
        const float PI = 3.14159265359f;

        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                // Position
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);
                // Normal
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);
            }
        }

        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
            }
        }

        m_sphereIndexCount = static_cast<int>(indices.size());

        glGenVertexArrays(1, &m_sphereVao);
        glGenBuffers(1, &m_sphereVbo);
        glGenBuffers(1, &m_sphereEbo);

        glBindVertexArray(m_sphereVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphereVbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void ViewportRenderer::CreatePlaneMesh() {
        float vertices[] = {
            // Pos              // Normal
            -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f
        };

        unsigned int indices[] = {
            0, 1, 2,  2, 3, 0
        };

        m_planeIndexCount = 6;

        glGenVertexArrays(1, &m_planeVao);
        glGenBuffers(1, &m_planeVbo);
        glGenBuffers(1, &m_planeEbo);

        glBindVertexArray(m_planeVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_planeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_planeEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void ViewportRenderer::CreateGridMesh() {
        std::vector<float> gridData;

        float size = 1000.0f;
        float step = 50.0f;
        glm::vec3 defaultGridColor(0.24f, 0.24f, 0.24f); // #3D3D3D
        glm::vec3 centerAxisColor(0.35f, 0.35f, 0.35f);

        for (float i = -size; i <= size; i += step) {
            glm::vec3 col = (i == 0.0f) ? centerAxisColor : defaultGridColor;

            // Line parallel to Z (X constant)
            gridData.push_back(i); gridData.push_back(0.0f); gridData.push_back(-size);
            gridData.push_back(col.r); gridData.push_back(col.g); gridData.push_back(col.b);

            gridData.push_back(i); gridData.push_back(0.0f); gridData.push_back(size);
            gridData.push_back(col.r); gridData.push_back(col.g); gridData.push_back(col.b);

            // Line parallel to X (Z constant)
            gridData.push_back(-size); gridData.push_back(0.0f); gridData.push_back(i);
            gridData.push_back(col.r); gridData.push_back(col.g); gridData.push_back(col.b);

            gridData.push_back(size); gridData.push_back(0.0f); gridData.push_back(i);
            gridData.push_back(col.r); gridData.push_back(col.g); gridData.push_back(col.b);
        }

        // Highlight central axes: X Axis (Red) and Z Axis (Blue) in C4D!
        // X-Axis (Red: -size to +size)
        gridData.push_back(-size); gridData.push_back(0.0f); gridData.push_back(0.0f);
        gridData.push_back(0.85f); gridData.push_back(0.25f); gridData.push_back(0.25f);
        gridData.push_back(size);  gridData.push_back(0.0f); gridData.push_back(0.0f);
        gridData.push_back(0.85f); gridData.push_back(0.25f); gridData.push_back(0.25f);

        // Z-Axis (Blue: -size to +size)
        gridData.push_back(0.0f); gridData.push_back(0.0f); gridData.push_back(-size);
        gridData.push_back(0.25f); gridData.push_back(0.45f); gridData.push_back(0.85f);
        gridData.push_back(0.0f); gridData.push_back(0.0f); gridData.push_back(size);
        gridData.push_back(0.25f); gridData.push_back(0.45f); gridData.push_back(0.85f);

        m_gridVertexCount = static_cast<int>(gridData.size() / 6);

        glGenVertexArrays(1, &m_gridVao);
        glGenBuffers(1, &m_gridVbo);

        glBindVertexArray(m_gridVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_gridVbo);
        glBufferData(GL_ARRAY_BUFFER, gridData.size() * sizeof(float), gridData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void ViewportRenderer::RenderGrid(const glm::mat4& viewProj) {
        glUseProgram(m_shaderLine);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLine, "uViewProj"), 1, GL_FALSE, &viewProj[0][0]);
        glm::mat4 identity(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLine, "uModel"), 1, GL_FALSE, &identity[0][0]);

        glBindVertexArray(m_gridVao);
        glDrawArrays(GL_LINES, 0, m_gridVertexCount);
        glBindVertexArray(0);
    }

    void ViewportRenderer::RenderGizmo(const BaseObject* selectedObj, const glm::mat4& viewProj, ToolMode currentTool) {
        if (!selectedObj || currentTool == ToolMode::LiveSelection) return;

        glm::vec3 pos = selectedObj->GetPosition();
        float len = 100.0f;

        std::vector<float> gizmoLines;
        // X Axis (Red)
        gizmoLines.push_back(pos.x); gizmoLines.push_back(pos.y); gizmoLines.push_back(pos.z);
        gizmoLines.push_back(0.95f); gizmoLines.push_back(0.2f);  gizmoLines.push_back(0.2f);
        gizmoLines.push_back(pos.x + len); gizmoLines.push_back(pos.y); gizmoLines.push_back(pos.z);
        gizmoLines.push_back(0.95f); gizmoLines.push_back(0.2f);  gizmoLines.push_back(0.2f);

        // Y Axis (Green)
        gizmoLines.push_back(pos.x); gizmoLines.push_back(pos.y); gizmoLines.push_back(pos.z);
        gizmoLines.push_back(0.2f);  gizmoLines.push_back(0.9f);  gizmoLines.push_back(0.2f);
        gizmoLines.push_back(pos.x); gizmoLines.push_back(pos.y + len); gizmoLines.push_back(pos.z);
        gizmoLines.push_back(0.2f);  gizmoLines.push_back(0.9f);  gizmoLines.push_back(0.2f);

        // Z Axis (Blue)
        gizmoLines.push_back(pos.x); gizmoLines.push_back(pos.y); gizmoLines.push_back(pos.z);
        gizmoLines.push_back(0.25f); gizmoLines.push_back(0.4f);  gizmoLines.push_back(0.95f);
        gizmoLines.push_back(pos.x); gizmoLines.push_back(pos.y); gizmoLines.push_back(pos.z + len);
        gizmoLines.push_back(0.25f); gizmoLines.push_back(0.4f);  gizmoLines.push_back(0.95f);

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, gizmoLines.size() * sizeof(float), gizmoLines.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDisable(GL_DEPTH_TEST);
        glLineWidth(3.0f);

        glUseProgram(m_shaderLine);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLine, "uViewProj"), 1, GL_FALSE, &viewProj[0][0]);
        glm::mat4 id(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLine, "uModel"), 1, GL_FALSE, &id[0][0]);

        glDrawArrays(GL_LINES, 0, 6);

        glLineWidth(1.0f);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void ViewportRenderer::RenderSceneObjects(const Scene& scene, const glm::mat4& viewProj) {
        glUseProgram(m_shaderBasic);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderBasic, "uViewProj"), 1, GL_FALSE, &viewProj[0][0]);

        glm::vec3 lightDir(0.5f, 1.0f, 0.7f);
        glUniform3fv(glGetUniformLocation(m_shaderBasic, "uLightDir"), 1, &lightDir[0]);

        glm::vec3 camPos = m_camera.GetPosition();
        glUniform3fv(glGetUniformLocation(m_shaderBasic, "uViewPos"), 1, &camPos[0]);

        auto RenderNode = [&](auto& self, BaseObject* obj) -> void {
            if (!obj || obj->GetEditorVisibility() == VisibilityMode::Hidden) return;

            const Material* mat = scene.FindMaterialById(obj->GetMaterialId());
            glm::vec3 baseColor = mat ? mat->color : glm::vec3(0.75f, 0.75f, 0.75f);

            bool isSelected = (scene.GetSelectedObject() == obj);
            glm::vec3 finalColor = isSelected ? glm::mix(baseColor, glm::vec3(0.95f, 0.65f, 0.12f), 0.35f) : baseColor;
            glUniform3fv(glGetUniformLocation(m_shaderBasic, "uColor"), 1, &finalColor[0]);

            glm::mat4 model = obj->GetWorldMatrix();

            if (obj->GetType() == ObjectType::Cube) {
                model = glm::scale(model, obj->cubeSize);
                glUniformMatrix4fv(glGetUniformLocation(m_shaderBasic, "uModel"), 1, GL_FALSE, &model[0][0]);
                glBindVertexArray(m_cubeVao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            } else if (obj->GetType() == ObjectType::Sphere) {
                model = glm::scale(model, glm::vec3(obj->sphereRadius));
                glUniformMatrix4fv(glGetUniformLocation(m_shaderBasic, "uModel"), 1, GL_FALSE, &model[0][0]);
                glBindVertexArray(m_sphereVao);
                glDrawElements(GL_TRIANGLES, m_sphereIndexCount, GL_UNSIGNED_INT, 0);
            } else if (obj->GetType() == ObjectType::Plane) {
                model = glm::scale(model, glm::vec3(obj->cubeSize.x, 1.0f, obj->cubeSize.z));
                glUniformMatrix4fv(glGetUniformLocation(m_shaderBasic, "uModel"), 1, GL_FALSE, &model[0][0]);
                glBindVertexArray(m_planeVao);
                glDrawElements(GL_TRIANGLES, m_planeIndexCount, GL_UNSIGNED_INT, 0);
            }

            for (const auto& child : obj->GetChildren()) {
                self(self, child.get());
            }
        };

        for (const auto& root : scene.GetRootObjects()) {
            RenderNode(RenderNode, root.get());
        }

        glBindVertexArray(0);
    }

    void ViewportRenderer::Render(const Scene& scene, ToolMode /*currentTool*/) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);

        // Cinema 4D Viewport Background Color: #1E1E1E / #222222
        glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glm::mat4 view = m_camera.GetViewMatrix();
        glm::mat4 proj = m_camera.GetProjectionMatrix();
        glm::mat4 viewProj = proj * view;

        // Render Grid
        RenderGrid(viewProj);

        // Render Meshes
        RenderSceneObjects(scene, viewProj);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace CineMod
