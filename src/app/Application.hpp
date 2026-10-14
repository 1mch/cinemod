#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "core/Types.hpp"
#include "scene/Scene.hpp"
#include "renderer/ViewportRenderer.hpp"
#include "ui/managers/ObjectManager.hpp"
#include "ui/managers/AttributeManager.hpp"
#include "ui/managers/CoordinatesManager.hpp"
#include "ui/managers/MaterialManager.hpp"
#include "ui/managers/TimelineManager.hpp"
#include "ui/managers/ViewportManager.hpp"
#include "ui/Commander.hpp"
#include "commands/CommandManager.hpp"

namespace CineMod {

    class Application {
    public:
        Application();
        ~Application();

        bool Init();
        void Run();

    private:
        void ProcessInput();
        void RenderUI();
        void RenderMainMenuBar();
        void RenderTopToolBar();
        void RenderLeftModeBar();
        void SetupDefaultDockSpace(ImGuiID dockspaceId);

        GLFWwindow* m_window = nullptr;
        int m_windowWidth = 1600;
        int m_windowHeight = 900;

        Scene m_scene;
        ViewportRenderer m_renderer;

        // Managers
        ObjectManager m_objectManager;
        AttributeManager m_attributeManager;
        CoordinatesManager m_coordinatesManager;
        MaterialManager m_materialManager;
        TimelineManager m_timelineManager;
        ViewportManager m_viewportManager;
        CommandManager m_commandManager;
        Commander m_commander;

        // Current tool & modes
        ToolMode m_currentTool = ToolMode::Move;
        EditMode m_editMode = EditMode::Model;

        bool m_firstFrameDocking = true;
    };

} // namespace CineMod
