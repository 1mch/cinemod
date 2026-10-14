#include "AttributeManager.hpp"
#include <imgui.h>

namespace CineMod {

    void AttributeManager::Render(Scene& scene) {
        ImGui::Begin("Attribute Manager");

        BaseObject* obj = scene.GetSelectedObject();
        if (!obj) {
            ImGui::TextDisabled("No object selected.");
            ImGui::End();
            return;
        }

        // Header with Object Name and Type
        ImGui::TextColored(ImVec4(0.92f, 0.58f, 0.05f, 1.0f), "[%s]", obj->GetName().c_str());
        ImGui::Separator();

        if (ImGui::BeginTabBar("AttributeTabs")) {
            if (ImGui::BeginTabItem("Basic")) {
                RenderBasicTab(obj);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Coord.")) {
                RenderCoordTab(obj);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Object")) {
                RenderObjectTab(obj);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void AttributeManager::RenderBasicTab(BaseObject* obj) {
        char nameBuf[128];
        strncpy(nameBuf, obj->GetName().c_str(), sizeof(nameBuf));
        nameBuf[sizeof(nameBuf) - 1] = '\0';
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
            obj->SetName(nameBuf);
        }

        bool enabled = obj->IsEnabled();
        if (ImGui::Checkbox("Enabled", &enabled)) {
            obj->SetEnabled(enabled);
        }

        // Visibility in Editor
        const char* visModes[] = { "Default (Inherit)", "On", "Off" };
        int edMode = static_cast<int>(obj->GetEditorVisibility());
        if (ImGui::Combo("Visible in Editor", &edMode, visModes, IM_ARRAYSIZE(visModes))) {
            obj->SetEditorVisibility(static_cast<VisibilityMode>(edMode));
        }

        // Visibility in Render
        int renMode = static_cast<int>(obj->GetRenderVisibility());
        if (ImGui::Combo("Visible in Renderer", &renMode, visModes, IM_ARRAYSIZE(visModes))) {
            obj->SetRenderVisibility(static_cast<VisibilityMode>(renMode));
        }
    }

    void AttributeManager::RenderCoordTab(BaseObject* obj) {
        glm::vec3 pos = obj->GetPosition();
        glm::vec3 scale = obj->GetScale();
        glm::vec3 hpb = obj->GetRotationHPB();

        ImGui::TextDisabled("Position");
        if (ImGui::DragFloat("P.X##px", &pos.x, 1.0f, 0.0f, 0.0f, "%.1f cm")) obj->SetPosition(pos);
        if (ImGui::DragFloat("P.Y##py", &pos.y, 1.0f, 0.0f, 0.0f, "%.1f cm")) obj->SetPosition(pos);
        if (ImGui::DragFloat("P.Z##pz", &pos.z, 1.0f, 0.0f, 0.0f, "%.1f cm")) obj->SetPosition(pos);

        ImGui::Separator();
        ImGui::TextDisabled("Scale");
        if (ImGui::DragFloat("S.X##sx", &scale.x, 0.01f, 0.001f, 100.0f, "%.3f")) obj->SetScale(scale);
        if (ImGui::DragFloat("S.Y##sy", &scale.y, 0.01f, 0.001f, 100.0f, "%.3f")) obj->SetScale(scale);
        if (ImGui::DragFloat("S.Z##sz", &scale.z, 0.01f, 0.001f, 100.0f, "%.3f")) obj->SetScale(scale);

        ImGui::Separator();
        ImGui::TextDisabled("Rotation (HPB Euler)");
        if (ImGui::DragFloat("R.H (Heading)##rh", &hpb.x, 0.5f, -360.0f, 360.0f, "%.1f deg")) obj->SetRotationHPB(hpb);
        if (ImGui::DragFloat("R.P (Pitch)##rp",   &hpb.y, 0.5f, -360.0f, 360.0f, "%.1f deg")) obj->SetRotationHPB(hpb);
        if (ImGui::DragFloat("R.B (Bank)##rb",    &hpb.z, 0.5f, -360.0f, 360.0f, "%.1f deg")) obj->SetRotationHPB(hpb);
    }

    void AttributeManager::RenderObjectTab(BaseObject* obj) {
        if (obj->GetType() == ObjectType::Cube) {
            ImGui::TextDisabled("Cube Parameters");
            ImGui::DragFloat3("Size (X,Y,Z)", &obj->cubeSize[0], 1.0f, 0.1f, 10000.0f, "%.1f cm");
            ImGui::DragInt3("Segments", &obj->cubeSegments[0], 1, 1, 100);
            ImGui::Checkbox("Fillet (Rounded Edges)", &obj->cubeFillet);
            if (obj->cubeFillet) {
                ImGui::DragFloat("Fillet Radius", &obj->cubeFilletRadius, 0.5f, 0.1f, 100.0f, "%.1f cm");
            }
        } else if (obj->GetType() == ObjectType::Sphere) {
            ImGui::TextDisabled("Sphere Parameters");
            ImGui::DragFloat("Radius", &obj->sphereRadius, 1.0f, 0.1f, 10000.0f, "%.1f cm");
            ImGui::DragInt("Segments", &obj->sphereSegments, 1, 3, 128);
        } else {
            ImGui::TextDisabled("No specific parametric properties for this object type.");
        }
    }

} // namespace CineMod
