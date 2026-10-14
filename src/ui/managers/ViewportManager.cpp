#include "ViewportManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>
#include <iostream>

namespace CineMod {

    ViewportManager::ViewportManager() {
    }

    void ViewportManager::Render(ViewportRenderer& renderer, Scene& scene, ToolMode /*currentTool*/) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Perspective View", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleVar();

        // Viewport Top Menu Bar (View, Cameras, Display, Options, Filter)
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Frame Selected", "S")) {
                    if (scene.GetSelectedObject()) {
                        renderer.GetCamera().FrameSelected(scene.GetSelectedObject()->GetPosition());
                    }
                }
                if (ImGui::MenuItem("Frame All", "H")) {
                    renderer.GetCamera().FrameAll();
                }
                if (ImGui::MenuItem("Redraw Viewport", "A")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Cameras")) {
                if (ImGui::MenuItem("Perspective", nullptr, true)) {}
                if (ImGui::MenuItem("Top (F2)")) {}
                if (ImGui::MenuItem("Right (F3)")) {}
                if (ImGui::MenuItem("Front (F4)")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Display")) {
                if (ImGui::MenuItem("Gouraud Shading", "N~A", true)) {}
                if (ImGui::MenuItem("Wireframe", "N~G")) {}
                if (ImGui::MenuItem("Isoparms")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Textures", nullptr, true)) {}
                if (ImGui::MenuItem("Shadows", nullptr, true)) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Filter")) {
                if (ImGui::MenuItem("Grid", nullptr, true)) {}
                if (ImGui::MenuItem("Horizon", nullptr, true)) {}
                if (ImGui::MenuItem("World Axis", nullptr, true)) {}
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Viewport Size & Texture Display
        ImVec2 viewSize = ImGui::GetContentRegionAvail();
        if (viewSize.x > 0 && viewSize.y > 0) {
            renderer.Resize(static_cast<int>(viewSize.x), static_cast<int>(viewSize.y));
        }

        ImTextureID texId = (ImTextureID)(uintptr_t)renderer.GetTextureId();

        // Render OpenGL texture (flipping UV vertically for OpenGL)
        ImGui::Image(texId, viewSize, ImVec2(0, 1), ImVec2(1, 0));

        // Handle C4D mouse and keyboard camera navigation
        HandleInput(renderer, scene);

        // Render C4D HUD overlay (top right icons for Pan, Zoom, Orbit, 4-Split)
        RenderHudOverlay(renderer);

        ImGui::End();
    }

    void ViewportManager::HandleInput(ViewportRenderer& renderer, Scene& scene) {
        if (!ImGui::IsItemHovered() && !m_isInteracting) return;

        ImGuiIO& io = ImGui::GetIO();
        Camera& camera = renderer.GetCamera();

        // Mouse Wheel Zoom
        if (io.MouseWheel != 0.0f) {
            camera.Zoom(-io.MouseWheel * 15.0f);
        }

        // Shortcut Keys
        bool key1 = ImGui::IsKeyDown(ImGuiKey_1); // Pan
        bool key2 = ImGui::IsKeyDown(ImGuiKey_2); // Zoom
        bool key3 = ImGui::IsKeyDown(ImGuiKey_3); // Orbit
        bool altDown = io.KeyAlt;

        bool lmbHeld = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        bool mmbHeld = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
        bool rmbHeld = ImGui::IsMouseDown(ImGuiMouseButton_Right);

        // Quick Frame shortcut keys
        if (ImGui::IsKeyPressed(ImGuiKey_S, false) && scene.GetSelectedObject()) {
            camera.FrameSelected(scene.GetSelectedObject()->GetPosition());
        }
        if (ImGui::IsKeyPressed(ImGuiKey_H, false)) {
            camera.FrameAll();
        }

        ImVec2 mousePos = io.MousePos;
        ImVec2 delta = ImVec2(mousePos.x - m_lastMousePos.x, mousePos.y - m_lastMousePos.y);

        if (lmbHeld || mmbHeld || rmbHeld) {
            if (m_isInteracting) {
                // Pan (1 + LMB or MMB or Alt + MMB)
                if (key1 || mmbHeld || (altDown && mmbHeld)) {
                    camera.Pan(delta.x, delta.y);
                }
                // Zoom (2 + LMB or Alt + RMB)
                else if (key2 || (altDown && rmbHeld)) {
                    camera.Zoom(delta.x + delta.y);
                }
                // Orbit (3 + LMB or Alt + LMB)
                else if (key3 || (altDown && lmbHeld)) {
                    camera.Orbit(delta.x, delta.y);
                }
            }
            m_isInteracting = true;
        } else {
            m_isInteracting = false;
        }

        m_lastMousePos = mousePos;
    }

    void ViewportManager::RenderHudOverlay(ViewportRenderer& renderer) {
        ImVec2 p = ImGui::GetItemRectMin();
        ImVec2 sz = ImGui::GetItemRectSize();

        // Place HUD icons in top-right corner inside viewport
        float iconSize = 22.0f;
        float spacing = 4.0f;
        float totalWidth = 4 * iconSize + 3 * spacing;

        ImVec2 hudPos = ImVec2(p.x + sz.x - totalWidth - 12.0f, p.y + 8.0f);

        ImGui::SetCursorScreenPos(hudPos);
        ImGui::BeginGroup();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 0.75f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C4DTheme::ColorOrangeAccent);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        // Pan icon [1]
        if (ImGui::Button("[1]##HudPan", ImVec2(iconSize, iconSize))) {}
        if (ImGui::IsItemActive()) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            renderer.GetCamera().Pan(delta.x, delta.y);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pan Camera (1 + Drag)");

        ImGui::SameLine(0, spacing);

        // Zoom icon [2]
        if (ImGui::Button("[2]##HudZoom", ImVec2(iconSize, iconSize))) {}
        if (ImGui::IsItemActive()) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            renderer.GetCamera().Zoom(delta.x + delta.y);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Zoom Camera (2 + Drag)");

        ImGui::SameLine(0, spacing);

        // Orbit icon [3]
        if (ImGui::Button("[3]##HudOrbit", ImVec2(iconSize, iconSize))) {}
        if (ImGui::IsItemActive()) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            renderer.GetCamera().Orbit(delta.x, delta.y);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Orbit Camera (3 + Drag)");

        ImGui::SameLine(0, spacing);

        // 4-View Switch icon [4]
        if (ImGui::Button("[#]##HudSplit", ImVec2(iconSize, iconSize))) {
            m_isFourView = !m_isFourView;
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle 4-View (F5)");

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::EndGroup();
    }

} // namespace CineMod
