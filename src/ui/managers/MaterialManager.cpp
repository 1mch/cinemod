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
        if (ImGui::Button("Edit Material")) {
            if (scene.GetSelectedMaterial()) {
                m_showEditor = true;
            }
        }
        ImGui::SameLine();
        ImGui::TextDisabled("| Double click swatch to edit");

        ImGui::Separator();

        // Material swatches grid
        float swatchSize = 56.0f;
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

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                scene.SelectMaterial(const_cast<Material*>(&mat));
                m_showEditor = true;
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

        if (m_showEditor) {
            RenderEditor(scene);
        }
    }

    void MaterialManager::RenderEditor(Scene& scene) {
        Material* mat = scene.GetSelectedMaterial();
        if (!mat) {
            m_showEditor = false;
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(340, 380), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Material Editor", &m_showEditor)) {
            ImGui::TextColored(C4DTheme::ColorOrangeAccent, "Material: %s [ID: %llu]", mat->name.c_str(), mat->id);
            ImGui::Separator();

            char nameBuf[64];
            strncpy(nameBuf, mat->name.c_str(), sizeof(nameBuf));
            nameBuf[sizeof(nameBuf) - 1] = '\0';
            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
                mat->name = nameBuf;
            }

            ImGui::Separator();
            ImGui::TextDisabled("Channels");

            // Color Channel
            if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::ColorEdit3("Diffuse Color", &mat->color[0]);

                // Quick presets
                ImGui::TextDisabled("Quick Presets:");
                auto PresetBtn = [&](const char* label, const glm::vec3& c) {
                    if (ImGui::SmallButton(label)) mat->color = c;
                    ImGui::SameLine();
                };
                PresetBtn("C4D Blue", glm::vec3(0.2f, 0.45f, 0.9f));
                PresetBtn("Gold", glm::vec3(0.95f, 0.75f, 0.2f));
                PresetBtn("Ruby", glm::vec3(0.85f, 0.15f, 0.2f));
                PresetBtn("White", glm::vec3(0.9f, 0.9f, 0.9f));
                ImGui::NewLine();
            }

            // Reflectance Channel
            if (ImGui::CollapsingHeader("Reflectance", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Roughness", &mat->roughness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Reflectance", &mat->reflectance, 0.0f, 1.0f, "%.2f");
            }

            ImGui::Separator();

            // Assignment to active object
            BaseObject* activeObj = scene.GetSelectedObject();
            if (activeObj) {
                if (ImGui::Button("Assign to Active Object")) {
                    activeObj->SetMaterialId(mat->id);
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(-> %s)", activeObj->GetName().c_str());
            } else {
                ImGui::TextDisabled("No active object to assign material to.");
            }
        }
        ImGui::End();
    }

} // namespace CineMod
