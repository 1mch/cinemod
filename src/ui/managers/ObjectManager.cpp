#include "ObjectManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include "commands/CommandManager.hpp"
#include "commands/SceneCommands.hpp"
#include <imgui.h>

namespace CineMod {

    void ObjectManager::Render(Scene& scene, CommandManager& commandManager) {
        ImGui::Begin("Object Manager");

        // Top Toolbar inside Object Manager (C4D File, Edit, View, Objects menu)
        if (ImGui::Button("+ Cube")) {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Cube", ObjectType::Cube));
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Sphere")) {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Sphere", ObjectType::Sphere));
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Plane")) {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Plane", ObjectType::Plane));
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Light")) {
            commandManager.ExecuteCommand(std::make_unique<AddObjectCommand>(scene, "Light", ObjectType::Light));
        }
        ImGui::SameLine();
        if (ImGui::Button("Duplicate")) {
            if (scene.GetSelectedObject()) {
                commandManager.ExecuteCommand(std::make_unique<DuplicateObjectCommand>(scene, scene.GetSelectedObject()->GetId()));
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            if (scene.GetSelectedObject()) {
                commandManager.ExecuteCommand(std::make_unique<DeleteObjectCommand>(scene, scene.GetSelectedObject()->GetId()));
            }
        }

        ImGui::Separator();

        // Search Bar (C4D filter)
        static char searchBuf[64] = "";
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##SearchObj", "Search objects...", searchBuf, sizeof(searchBuf));

        ImGui::Separator();

        // Tree List of Objects
        ImGui::BeginChild("ObjectTreeScroll", ImVec2(0, 0), false);

        for (const auto& obj : scene.GetRootObjects()) {
            RenderObjectNode(obj.get(), scene, commandManager);
        }

        ImGui::EndChild();
        ImGui::End();
    }

    void ObjectManager::RenderTrafficLightDot(const char* id, VisibilityMode mode, bool /*isUpper*/) {
        ImVec4 col = C4DTheme::ColorTrafficDefault;
        if (mode == VisibilityMode::Visible) col = C4DTheme::ColorTrafficVisible;
        else if (mode == VisibilityMode::Hidden) col = C4DTheme::ColorTrafficHidden;

        ImGui::PushStyleColor(ImGuiCol_Button, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

        ImGui::Button(id, ImVec2(8, 8));

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    void ObjectManager::RenderObjectNode(BaseObject* obj, Scene& scene, CommandManager& commandManager) {
        if (!obj) return;

        ImGui::PushID(static_cast<int>(obj->GetId()));

        // Traffic Light Dots (Editor & Render)
        ImGui::BeginGroup();
        // Upper dot (Editor)
        if (ImGui::InvisibleButton("##DotEditor", ImVec2(8, 7))) {
            obj->CycleEditorVisibility();
        }
        if (ImGui::IsItemVisible()) {
            ImVec4 col = (obj->GetEditorVisibility() == VisibilityMode::Visible) ? C4DTheme::ColorTrafficVisible :
                         (obj->GetEditorVisibility() == VisibilityMode::Hidden)  ? C4DTheme::ColorTrafficHidden :
                                                                                    C4DTheme::ColorTrafficDefault;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 p0 = ImGui::GetItemRectMin();
            ImVec2 p1 = ImGui::GetItemRectMax();
            dl->AddCircleFilled(ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f), 3.0f, ImGui::GetColorU32(col));
        }

        // Lower dot (Render)
        if (ImGui::InvisibleButton("##DotRender", ImVec2(8, 7))) {
            obj->CycleRenderVisibility();
        }
        if (ImGui::IsItemVisible()) {
            ImVec4 col = (obj->GetRenderVisibility() == VisibilityMode::Visible) ? C4DTheme::ColorTrafficVisible :
                         (obj->GetRenderVisibility() == VisibilityMode::Hidden)  ? C4DTheme::ColorTrafficHidden :
                                                                                    C4DTheme::ColorTrafficDefault;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 p0 = ImGui::GetItemRectMin();
            ImVec2 p1 = ImGui::GetItemRectMax();
            dl->AddCircleFilled(ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f), 3.0f, ImGui::GetColorU32(col));
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        // Node selection and tree
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (scene.GetSelectedObject() == obj) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (obj->GetChildren().empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        // Object label with type prefix
        std::string label = obj->GetName();
        bool nodeOpen = ImGui::TreeNodeEx("##Node", flags, "%s", label.c_str());

        if (ImGui::IsItemClicked()) {
            scene.SelectObject(obj);
        }

        if (ImGui::BeginPopupContextItem()) {
            scene.SelectObject(obj);
            if (ImGui::MenuItem("Duplicate")) {
                commandManager.ExecuteCommand(std::make_unique<DuplicateObjectCommand>(scene, obj->GetId()));
            }
            if (ImGui::MenuItem("Delete", "Del")) {
                commandManager.ExecuteCommand(std::make_unique<DeleteObjectCommand>(scene, obj->GetId()));
            }
            ImGui::EndPopup();
        }

        // Tags strip to the right
        ImGui::SameLine(ImGui::GetWindowWidth() - 140.0f);
        for (const auto& tag : obj->GetTags()) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.28f, 0.35f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 1));
            ImGui::SmallButton(tag.name.c_str());
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::SameLine();
        }
        ImGui::NewLine();

        // Recurse children if open
        if (nodeOpen && !obj->GetChildren().empty()) {
            for (const auto& child : obj->GetChildren()) {
                RenderObjectNode(child.get(), scene, commandManager);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

} // namespace CineMod
