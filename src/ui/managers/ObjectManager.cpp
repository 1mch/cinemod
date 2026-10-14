#include "ObjectManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>

namespace CineMod {

    void ObjectManager::Render(Scene& scene) {
        ImGui::Begin("Object Manager");

        // Top Toolbar inside Object Manager (C4D File, Edit, View, Objects menu)
        if (ImGui::Button("+ Cube")) {
            BaseObject* obj = scene.AddObject("Cube", ObjectType::Cube);
            obj->AddTag({ "Phong", "Phong" });
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Sphere")) {
            BaseObject* obj = scene.AddObject("Sphere", ObjectType::Sphere);
            obj->AddTag({ "Phong", "Phong" });
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Plane")) {
            BaseObject* obj = scene.AddObject("Plane", ObjectType::Plane);
            obj->AddTag({ "Phong", "Phong" });
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Light")) {
            scene.AddObject("Light", ObjectType::Light);
        }
        ImGui::SameLine();
        if (ImGui::Button("Duplicate")) {
            if (scene.GetSelectedObject()) {
                scene.DuplicateObject(scene.GetSelectedObject());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            if (scene.GetSelectedObject()) {
                scene.RemoveObject(scene.GetSelectedObject()->GetId());
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
            RenderObjectNode(obj.get(), scene);
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

    void ObjectManager::RenderObjectNode(BaseObject* obj, Scene& scene) {
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
                RenderObjectNode(child.get(), scene);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

} // namespace CineMod
