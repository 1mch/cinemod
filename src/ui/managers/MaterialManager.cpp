#include "MaterialManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>

namespace CineMod {

    void MaterialManager::Render(Scene& scene) {
        ImGui::Begin("Material Manager");

        // Menu / buttons
        if (ImGui::Button("+ New Material") || (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())) {
            scene.AddMaterial("Mat");
        }
        ImGui::SameLine();
        ImGui::TextDisabled("| Double click empty area to create");

        ImGui::Separator();

        // Material swatches grid
        float swatchSize = 64.0f;
        int cols = static_cast<int>(ImGui::GetWindowWidth() / (swatchSize + 16.0f));
        if (cols < 1) cols = 1;

        int idx = 0;
        for (const auto& mat : scene.GetMaterials()) {
            ImGui::PushID(static_cast<int>(mat.id));

            bool isSelected = (scene.GetSelectedMaterial() && scene.GetSelectedMaterial()->id == mat.id);
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Border, C4DTheme::ColorOrangeAccent);
            }

            ImVec4 col(mat.color.r, mat.color.g, mat.color.b, 1.0f);
            if (ImGui::ColorButton(mat.name.c_str(), col, ImGuiColorEditFlags_NoTooltip, ImVec2(swatchSize, swatchSize))) {
                scene.SelectMaterial(const_cast<Material*>(&mat));
            }

            if (isSelected) {
                ImGui::PopStyleColor();
            }

            ImGui::TextWrapped("%s", mat.name.c_str());

            if ((++idx % cols) != 0) {
                ImGui::SameLine();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }

} // namespace CineMod
