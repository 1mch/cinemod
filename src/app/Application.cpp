#include "Application.hpp"
#include "ui/theme/C4DTheme.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include "commands/SceneCommands.hpp"
#include <iostream>

namespace CineMod {

    Application::Application() {
    }

    Application::~Application() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    bool Application::Init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return false;
        }

        // OpenGL 4.5 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "CineMod - Cinema 4D Clone [Modern C++20 / OpenGL 4.5]", nullptr, nullptr);
        if (!m_window) {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // Enable V-Sync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD OpenGL loader!" << std::endl;
            return false;
        }

        std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

        // Init ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Apply Cinema 4D Dark Theme
        C4DTheme::ApplyTheme();

        ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Init Viewport Renderer
        m_renderer.Init();

        return true;
    }

    void Application::Run() {
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();

            double currentTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            ProcessInput();

            // Update scene and animation
            m_timelineManager.Update(m_scene, deltaTime);

            // Render 3D Viewport framebuffer
            m_renderer.Render(m_scene, m_currentTool);

            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();

            RenderUI();

            // Render ImGui draw data
            ImGui::Render();
            int displayW, displayH;
            glfwGetFramebufferSize(m_window, &displayW, &displayH);
            glViewport(0, 0, displayW, displayH);
            glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(m_window);
        }
    }

    void Application::ProcessInput() {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantTextInput) return;

        // Quick Commander (Shift+C)
        if (io.KeyShift && !io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C, false)) {
            m_commander.Toggle();
        }

        // Undo & Redo shortcuts
        if (io.KeyCtrl && !io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
            m_commandManager.Undo();
        }
        if ((io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) ||
            (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false))) {
            m_commandManager.Redo();
        }

        // Duplicate active object (Ctrl+D)
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false)) {
            if (m_scene.GetSelectedObject()) {
                m_commandManager.ExecuteCommand(std::make_unique<DuplicateObjectCommand>(m_scene, m_scene.GetSelectedObject()->GetId()));
            }
        }

        // C4D Tool Shortcuts
        if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
            m_currentTool = ToolMode::Move;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            m_currentTool = ToolMode::Rotate;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_T, false)) {
            m_currentTool = ToolMode::Scale;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_9, false)) {
            m_currentTool = ToolMode::LiveSelection;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_0, false)) {
            m_currentTool = ToolMode::RectangleSelection;
        }

        // Spacebar toggles between Move and Selection
        if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
            if (m_currentTool == ToolMode::LiveSelection) {
                m_currentTool = ToolMode::Move;
            } else {
                m_currentTool = ToolMode::LiveSelection;
            }
        }

        // F8 Play / Pause
        if (ImGui::IsKeyPressed(ImGuiKey_F8, false)) {
            m_scene.SetPlaying(!m_scene.IsPlaying());
        }

        // Delete active object (Delete or Backspace)
        if (ImGui::IsKeyPressed(ImGuiKey_Delete, false) || ImGui::IsKeyPressed(ImGuiKey_Backspace, false)) {
            if (m_scene.GetSelectedObject()) {
                m_commandManager.ExecuteCommand(std::make_unique<DeleteObjectCommand>(m_scene, m_scene.GetSelectedObject()->GetId()));
            }
        }
    }

    void Application::SetupDefaultDockSpace(ImGuiID dockspaceId) {
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

        ImGuiID dockMain = dockspaceId;
        // Split right for Object and Attribute managers (30% width)
        ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.28f, nullptr, &dockMain);
        // Split bottom of right for Attribute Manager (50% of right height)
        ImGuiID dockRightDown = ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.50f, nullptr, &dockRight);

        // Split bottom for Materials, Coordinates and Timeline (28% height)
        ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.28f, nullptr, &dockMain);
        // Split Timeline at the top of the bottom area
        ImGuiID dockTimeline = ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Up, 0.35f, nullptr, &dockBottom);
        // Split remaining bottom into Material Manager (Left) and Coordinates Manager (Right)
        ImGuiID dockCoords = ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Right, 0.45f, nullptr, &dockBottom);
        ImGuiID dockMaterials = dockBottom;

        ImGuiID dockCenter = dockMain;

        // Dock windows
        ImGui::DockBuilderDockWindow("Perspective View", dockCenter);
        ImGui::DockBuilderDockWindow("Object Manager", dockRight);
        ImGui::DockBuilderDockWindow("Attribute Manager", dockRightDown);
        ImGui::DockBuilderDockWindow("Timeline", dockTimeline);
        ImGui::DockBuilderDockWindow("Material Manager", dockMaterials);
        ImGui::DockBuilderDockWindow("Coordinates", dockCoords);

        ImGui::DockBuilderFinish(dockspaceId);
    }

    void Application::RenderMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Project", "Ctrl+N")) { m_scene.CreateDefaultScene(); }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Quit", "Ctrl+Q")) { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                std::string undoLabel = "Undo";
                if (m_commandManager.CanUndo()) {
                    undoLabel += " " + m_commandManager.GetUndoName();
                }
                if (ImGui::MenuItem(undoLabel.c_str(), "Ctrl+Z", false, m_commandManager.CanUndo())) {
                    m_commandManager.Undo();
                }

                std::string redoLabel = "Redo";
                if (m_commandManager.CanRedo()) {
                    redoLabel += " " + m_commandManager.GetRedoName();
                }
                if (ImGui::MenuItem(redoLabel.c_str(), "Ctrl+Y", false, m_commandManager.CanRedo())) {
                    m_commandManager.Redo();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Project Settings...", "Ctrl+D")) {}
                if (ImGui::MenuItem("Preferences...", "Ctrl+E")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Cube")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Cube", ObjectType::Cube)); }
                if (ImGui::MenuItem("Sphere")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Sphere", ObjectType::Sphere)); }
                if (ImGui::MenuItem("Cylinder")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Cylinder", ObjectType::Cylinder)); }
                if (ImGui::MenuItem("Plane")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Plane", ObjectType::Plane)); }
                ImGui::Separator();
                if (ImGui::MenuItem("Camera")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Camera", ObjectType::Camera)); }
                if (ImGui::MenuItem("Light")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Light", ObjectType::Light)); }
                if (ImGui::MenuItem("Null Object")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Null", ObjectType::Null)); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Select")) {
                if (ImGui::MenuItem("Live Selection", "9", m_currentTool == ToolMode::LiveSelection)) { m_currentTool = ToolMode::LiveSelection; }
                if (ImGui::MenuItem("Rectangle Selection", "0", m_currentTool == ToolMode::RectangleSelection)) { m_currentTool = ToolMode::RectangleSelection; }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Move", "E", m_currentTool == ToolMode::Move)) { m_currentTool = ToolMode::Move; }
                if (ImGui::MenuItem("Rotate", "R", m_currentTool == ToolMode::Rotate)) { m_currentTool = ToolMode::Rotate; }
                if (ImGui::MenuItem("Scale", "T", m_currentTool == ToolMode::Scale)) { m_currentTool = ToolMode::Scale; }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Render View", "Ctrl+R")) {}
                if (ImGui::MenuItem("Render to Picture Viewer", "Shift+R")) {}
                if (ImGui::MenuItem("Render Settings...", "Ctrl+B")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Object Manager", nullptr, true);
                ImGui::MenuItem("Attribute Manager", nullptr, true);
                ImGui::MenuItem("Timeline", nullptr, true);
                ImGui::MenuItem("Material Manager", nullptr, true);
                ImGui::MenuItem("Coordinates", nullptr, true);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("Cinema 4D Documentation (Ctrl+F1)")) {}
                if (ImGui::MenuItem("About CineMod")) {}
                ImGui::EndMenu();
            }

            // Quick Commander shortcut in menu bar (Shift+C)
            ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
            if (ImGui::SmallButton("Search: Shift+C")) {
                m_commander.Open();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void Application::RenderTopToolBar() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight()));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 34.0f));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 4));
        ImGui::Begin("##TopToolBar", nullptr, flags);

        // Undo / Redo
        if (!m_commandManager.CanUndo()) ImGui::BeginDisabled();
        if (ImGui::Button("Undo")) {
            m_commandManager.Undo();
        }
        if (!m_commandManager.CanUndo()) ImGui::EndDisabled();
        else if (ImGui::IsItemHovered()) ImGui::SetTooltip("Undo %s (Ctrl+Z)", m_commandManager.GetUndoName().c_str());

        ImGui::SameLine();

        if (!m_commandManager.CanRedo()) ImGui::BeginDisabled();
        if (ImGui::Button("Redo")) {
            m_commandManager.Redo();
        }
        if (!m_commandManager.CanRedo()) ImGui::EndDisabled();
        else if (ImGui::IsItemHovered()) ImGui::SetTooltip("Redo %s (Ctrl+Y)", m_commandManager.GetRedoName().c_str());

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        // Selection & Transform buttons
        auto ToolButton = [this](const char* label, ToolMode mode) {
            bool active = (m_currentTool == mode);
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, C4DTheme::ColorOrangeAccent);
            if (ImGui::Button(label)) m_currentTool = mode;
            if (active) ImGui::PopStyleColor();
            ImGui::SameLine();
        };

        ToolButton("Select (9)", ToolMode::LiveSelection);
        ToolButton("Move (E)", ToolMode::Move);
        ToolButton("Rotate (R)", ToolMode::Rotate);
        ToolButton("Scale (T)", ToolMode::Scale);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        // Quick Primitives
        if (ImGui::Button("+ Cube")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Cube", ObjectType::Cube)); }
        ImGui::SameLine();
        if (ImGui::Button("+ Sphere")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Sphere", ObjectType::Sphere)); }
        ImGui::SameLine();
        if (ImGui::Button("+ Plane")) { m_commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(m_scene, "Plane", ObjectType::Plane)); }
        ImGui::SameLine();

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        // Render buttons
        if (ImGui::Button("Render (Ctrl+R)")) {}
        ImGui::SameLine();
        if (ImGui::Button("Pic Viewer (Shift+R)")) {}

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Application::RenderLeftModeBar() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float topBarH = ImGui::GetFrameHeight() + 34.0f;
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + topBarH));
        ImGui::SetNextWindowSize(ImVec2(40.0f, viewport->Size.y - topBarH));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::Begin("##LeftModeBar", nullptr, flags);

        auto ModeButton = [this](const char* label, EditMode mode, const char* tooltip) {
            bool active = (m_editMode == mode);
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, C4DTheme::ColorOrangeAccent);
            if (ImGui::Button(label, ImVec2(32, 28))) m_editMode = mode;
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", tooltip);
            if (active) ImGui::PopStyleColor();
        };

        ModeButton("Mod", EditMode::Model, "Model Mode");
        ModeButton("Pnt", EditMode::Points, "Points Mode");
        ModeButton("Edg", EditMode::Edges, "Edges Mode");
        ModeButton("Pol", EditMode::Polygons, "Polygons Mode");

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Application::RenderUI() {
        RenderMainMenuBar();
        RenderTopToolBar();
        RenderLeftModeBar();

        // Central DockSpace Container
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float topOffset = ImGui::GetFrameHeight() + 34.0f;
        float leftOffset = 40.0f;

        ImVec2 dockspacePos(viewport->Pos.x + leftOffset, viewport->Pos.y + topOffset);
        ImVec2 dockspaceSize(viewport->Size.x - leftOffset, viewport->Size.y - topOffset);

        ImGui::SetNextWindowPos(dockspacePos);
        ImGui::SetNextWindowSize(dockspaceSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("C4DDockSpaceHost", nullptr, hostFlags);
        ImGui::PopStyleVar();

        ImGuiID dockspaceId = ImGui::GetID("C4DMainDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        if (m_firstFrameDocking) {
            m_firstFrameDocking = false;
            SetupDefaultDockSpace(dockspaceId);
        }

        ImGui::End();

        // Render all Cinema 4D Managers
        m_viewportManager.Render(m_renderer, m_scene, m_currentTool, m_commandManager);
        m_objectManager.Render(m_scene, m_commandManager);
        m_attributeManager.Render(m_scene);
        m_coordinatesManager.Render(m_scene);
        m_materialManager.Render(m_scene);
        m_timelineManager.Render(m_scene);

        // Cinema 4D Quick Commander Popup (Shift+C)
        m_commander.Render(m_scene, m_renderer, m_currentTool, m_commandManager);
    }

} // namespace CineMod
