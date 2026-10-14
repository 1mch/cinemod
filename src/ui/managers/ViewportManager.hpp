#pragma once

#include <imgui.h>
#include "renderer/ViewportRenderer.hpp"
#include "scene/Scene.hpp"

namespace CineMod {

    class CommandManager;

    class ViewportManager {
    public:
        ViewportManager();
        void Render(ViewportRenderer& renderer, Scene& scene, ToolMode currentTool, CommandManager& commandManager);

    private:
        void HandleInput(ViewportRenderer& renderer, Scene& scene, const ImVec2& viewPos, const ImVec2& viewSize, CommandManager& commandManager);
        void RenderHudOverlay(ViewportRenderer& renderer);
        void RenderGizmo(ViewportRenderer& renderer, Scene& scene, ToolMode currentTool, const ImVec2& viewPos, const ImVec2& viewSize, CommandManager& commandManager);

        bool m_isInteracting = false;
        ImVec2 m_lastMousePos{ 0.0f, 0.0f };
        bool m_isFourView = false;
        CoordSystem m_coordSystem = CoordSystem::World;

        // Gizmo Undo tracking
        bool m_wasUsingGizmo = false;
        uint64_t m_gizmoObjId = 0;
        glm::vec3 m_gizmoStartPos{ 0.0f };
        glm::vec3 m_gizmoStartRot{ 0.0f };
        glm::vec3 m_gizmoStartScale{ 1.0f };
    };

} // namespace CineMod
