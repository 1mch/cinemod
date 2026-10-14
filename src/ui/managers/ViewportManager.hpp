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
        void HandleInput(ViewportRenderer& renderer, Scene& scene);
        void RenderHudOverlay(ViewportRenderer& renderer);

        bool m_isInteracting = false;
        ImVec2 m_lastMousePos{ 0.0f, 0.0f };
        bool m_isFourView = false;
    };

} // namespace CineMod
