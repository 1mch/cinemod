#include "CoordinatesManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>

namespace CineMod {

    void CoordinatesManager::Render(Scene& scene) {
        ImGui::Begin("Coordinates");

        BaseObject* obj = scene.GetSelectedObject();
        if (!obj) {
            ImGui::TextDisabled("No active object.");
            ImGui::End();
            return;
        }

        // Space selection
        if (ImGui::RadioButton("World", m_system == CoordSystem::World)) {
            m_system = CoordSystem::World;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Object", m_system == CoordSystem::Object)) {
            m_system = CoordSystem::Object;
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 70.0f);
        ImGui::Button("Apply");

        ImGui::Separator();

        glm::vec3 pos = obj->GetPosition();
        glm::vec3 scale = obj->GetScale();
        glm::vec3 hpb = obj->GetRotationHPB();

        // 3-column layout: P, S, R
        ImGui::Columns(3, "CoordCols", false);

        // Position
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Position");
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisX);
        if (ImGui::DragFloat("X##coordPx", &pos.x, 1.0f, 0.0f, 0.0f, "%.1f")) obj->SetPosition(pos);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisY);
        if (ImGui::DragFloat("Y##coordPy", &pos.y, 1.0f, 0.0f, 0.0f, "%.1f")) obj->SetPosition(pos);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisZ);
        if (ImGui::DragFloat("Z##coordPz", &pos.z, 1.0f, 0.0f, 0.0f, "%.1f")) obj->SetPosition(pos);
        ImGui::PopStyleColor();

        ImGui::NextColumn();

        // Size / Scale
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Size / Scale");
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisX);
        if (ImGui::DragFloat("X##coordSx", &scale.x, 0.01f, 0.001f, 100.0f, "%.2f")) obj->SetScale(scale);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisY);
        if (ImGui::DragFloat("Y##coordSy", &scale.y, 0.01f, 0.001f, 100.0f, "%.2f")) obj->SetScale(scale);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisZ);
        if (ImGui::DragFloat("Z##coordSz", &scale.z, 0.01f, 0.001f, 100.0f, "%.2f")) obj->SetScale(scale);
        ImGui::PopStyleColor();

        ImGui::NextColumn();

        // Rotation HPB
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Rotation (HPB)");
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisY); // Heading rotates around Y
        if (ImGui::DragFloat("H##coordRh", &hpb.x, 0.5f, -360.0f, 360.0f, "%.1f°")) obj->SetRotationHPB(hpb);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisX); // Pitch rotates around X
        if (ImGui::DragFloat("P##coordRp", &hpb.y, 0.5f, -360.0f, 360.0f, "%.1f°")) obj->SetRotationHPB(hpb);
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, C4DTheme::ColorAxisZ); // Bank rotates around Z
        if (ImGui::DragFloat("B##coordRb", &hpb.z, 0.5f, -360.0f, 360.0f, "%.1f°")) obj->SetRotationHPB(hpb);
        ImGui::PopStyleColor();

        ImGui::Columns(1);

        ImGui::End();
    }

} // namespace CineMod
