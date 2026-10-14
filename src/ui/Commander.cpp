#include "Commander.hpp"
#include "scene/Scene.hpp"
#include "renderer/ViewportRenderer.hpp"
#include "commands/CommandManager.hpp"
#include "commands/SceneCommands.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>
#include <algorithm>
#include <cctype>

namespace CineMod {

    Commander::Commander() {
    }

    void Commander::Open() {
        m_isOpen = true;
        m_searchQuery[0] = '\0';
        m_selectedIndex = 0;
        m_focusSearch = true;
    }

    void Commander::Close() {
        m_isOpen = false;
    }

    void Commander::Toggle() {
        if (m_isOpen) Close();
        else Open();
    }

    void Commander::PopulateItems(Scene& scene, ViewportRenderer& renderer, ToolMode& currentTool, CommandManager& commandManager) {
        m_items.clear();

        // Primitives & Objects (Create)
        m_items.push_back({ "Cube", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Cube", ObjectType::Cube));
        }});
        m_items.push_back({ "Sphere", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Sphere", ObjectType::Sphere));
        }});
        m_items.push_back({ "Plane", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Plane", ObjectType::Plane));
        }});
        m_items.push_back({ "Cylinder", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Cylinder", ObjectType::Cylinder));
        }});
        m_items.push_back({ "Camera", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Camera", ObjectType::Camera));
        }});
        m_items.push_back({ "Light", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Light", ObjectType::Light));
        }});
        m_items.push_back({ "Null Object", "", "Create", [&scene, &commandManager]() {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Null", ObjectType::Null));
        }});

        // Tools
        m_items.push_back({ "Move Tool", "E", "Tools", [&currentTool]() {
            currentTool = ToolMode::Move;
        }});
        m_items.push_back({ "Rotate Tool", "R", "Tools", [&currentTool]() {
            currentTool = ToolMode::Rotate;
        }});
        m_items.push_back({ "Scale Tool", "T", "Tools", [&currentTool]() {
            currentTool = ToolMode::Scale;
        }});
        m_items.push_back({ "Live Selection", "9", "Tools", [&currentTool]() {
            currentTool = ToolMode::LiveSelection;
        }});
        m_items.push_back({ "Rectangle Selection", "0", "Tools", [&currentTool]() {
            currentTool = ToolMode::RectangleSelection;
        }});

        // Edit
        m_items.push_back({ "Undo", "Ctrl+Z", "Edit", [&commandManager]() {
            commandManager.Undo();
        }});
        m_items.push_back({ "Redo", "Ctrl+Y", "Edit", [&commandManager]() {
            commandManager.Redo();
        }});
        m_items.push_back({ "Duplicate Object", "Ctrl+D", "Edit", [&scene, &commandManager]() {
            if (scene.GetSelectedObject()) {
                commandManager.ExecuteCommand(std::make_unique<DuplicateObjectCommand>(scene, scene.GetSelectedObject()->GetId()));
            }
        }});
        m_items.push_back({ "Delete Object", "Del", "Edit", [&scene, &commandManager]() {
            if (scene.GetSelectedObject()) {
                commandManager.ExecuteCommand(std::make_unique<DeleteObjectCommand>(scene, scene.GetSelectedObject()->GetId()));
            }
        }});

        // View & Navigation
        m_items.push_back({ "Frame All", "H", "View", [&renderer]() {
            renderer.GetCamera().FrameAll();
        }});
        m_items.push_back({ "Frame Selected", "S", "View", [&renderer, &scene]() {
            if (scene.GetSelectedObject()) {
                renderer.GetCamera().FrameSelected(scene.GetSelectedObject()->GetPosition());
            }
        }});

        // Materials & Scene
        m_items.push_back({ "New Material", "Ctrl+M", "Material", [&scene]() {
            scene.AddMaterial("Mat");
        }});
        m_items.push_back({ "Play / Pause Animation", "F8", "Animation", [&scene]() {
            scene.SetPlaying(!scene.IsPlaying());
        }});
        m_items.push_back({ "New Project", "Ctrl+N", "File", [&scene]() {
            scene.CreateDefaultScene();
        }});
    }

    void Commander::Render(Scene& scene, ViewportRenderer& renderer, ToolMode& currentTool, CommandManager& commandManager) {
        if (!m_isOpen) return;

        PopulateItems(scene, renderer, currentTool, commandManager);

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        ImVec2 center(mainViewport->Pos.x + mainViewport->Size.x * 0.5f,
                      mainViewport->Pos.y + mainViewport->Size.y * 0.35f);

        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(480, 360));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, C4DTheme::ColorOrangeAccent);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

        if (ImGui::Begin("##CommanderPopup", &m_isOpen, flags)) {
            // Header
            ImGui::TextColored(C4DTheme::ColorOrangeAccent, "COMMANDER");
            ImGui::SameLine();
            ImGui::TextDisabled("| Shift+C to toggle, Enter to execute, Esc to exit");
            ImGui::Separator();

            // Search input
            if (m_focusSearch) {
                ImGui::SetKeyboardFocusHere();
                m_focusSearch = false;
            }

            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##CommanderSearchInput", "Search commands, objects, tools...", m_searchQuery, sizeof(m_searchQuery));
            ImGui::PopItemWidth();

            // Filter items based on query
            std::string query = m_searchQuery;
            std::transform(query.begin(), query.end(), query.begin(), ::tolower);

            std::vector<const CommanderItem*> filtered;
            for (const auto& item : m_items) {
                std::string nameLower = item.name;
                std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                std::string catLower = item.category;
                std::transform(catLower.begin(), catLower.end(), catLower.begin(), ::tolower);

                if (query.empty() || nameLower.find(query) != std::string::npos || catLower.find(query) != std::string::npos) {
                    filtered.push_back(&item);
                }
            }

            // Keyboard navigation
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, true)) {
                if (!filtered.empty()) {
                    m_selectedIndex = (m_selectedIndex + 1) % static_cast<int>(filtered.size());
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, true)) {
                if (!filtered.empty()) {
                    m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(filtered.size())) % static_cast<int>(filtered.size());
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
                Close();
            }

            bool enterPressed = ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false);
            if (enterPressed && !filtered.empty()) {
                if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(filtered.size())) {
                    filtered[m_selectedIndex]->action();
                    Close();
                }
            }

            if (m_selectedIndex >= static_cast<int>(filtered.size())) {
                m_selectedIndex = 0;
            }

            ImGui::Separator();

            // Results list
            ImGui::BeginChild("CommanderResults", ImVec2(0, 0), false);
            for (int i = 0; i < static_cast<int>(filtered.size()); ++i) {
                const CommanderItem* item = filtered[i];
                bool isSelected = (i == m_selectedIndex);

                ImGui::PushID(i);

                if (isSelected) {
                    ImGui::PushStyleColor(ImGuiCol_Header, C4DTheme::ColorSelection);
                }

                // Category tag
                ImVec4 catColor(0.24f, 0.28f, 0.35f, 1.0f);
                if (item->category == "Create") catColor = ImVec4(0.18f, 0.42f, 0.25f, 1.0f);
                else if (item->category == "Tools") catColor = ImVec4(0.20f, 0.35f, 0.55f, 1.0f);
                else if (item->category == "Edit") catColor = ImVec4(0.50f, 0.30f, 0.15f, 1.0f);

                ImGui::PushStyleColor(ImGuiCol_Text, catColor);
                ImGui::Text("[%s]", item->category.c_str());
                ImGui::PopStyleColor();

                ImGui::SameLine();

                // Item name
                std::string selectLabel = item->name + "##Selectable";
                if (ImGui::Selectable(item->name.c_str(), isSelected, ImGuiSelectableFlags_None, ImVec2(ImGui::GetWindowWidth() - 120.0f, 0))) {
                    item->action();
                    Close();
                }

                if (isSelected) {
                    ImGui::PopStyleColor();
                }

                // Shortcut aligned to the right
                if (!item->shortcut.empty()) {
                    ImGui::SameLine(ImGui::GetWindowWidth() - 80.0f);
                    ImGui::TextDisabled("%s", item->shortcut.c_str());
                }

                ImGui::PopID();
            }
            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();
    }

} // namespace CineMod
