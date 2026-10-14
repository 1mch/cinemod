#pragma once

#include <imgui.h>
#include "renderer/ViewportRenderer.hpp"
#include "scene/Scene.hpp"

namespace CineMod {

    class ViewportManager {
    public:
        ViewportManager();
        void Render(ViewportRenderer& renderer, Scene& scene, ToolMode currentTool);

    private:
        void HandleInput(ViewportRenderer& renderer, Scene& scene, const ImVec2& viewPos, const ImVec2& viewSize);
        void RenderHudOverlay(ViewportRenderer& renderer);
        void RenderGizmo(ViewportRenderer& renderer, Scene& scene, ToolMode currentTool, const ImVec2& viewPos, const ImVec2& viewSize);

        bool m_isInteracting = false;
        ImVec2 m_lastMousePos{ 0.0f, 0.0f };
        bool m_isFourView = false;
        CoordSystem m_coordSystem = CoordSystem::World;
    };

} // namespace CineMod
