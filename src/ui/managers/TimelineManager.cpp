#include "TimelineManager.hpp"
#include "ui/theme/C4DTheme.hpp"
#include <imgui.h>

namespace CineMod {

    void TimelineManager::Update(Scene& scene, float deltaTime) {
        if (!scene.IsPlaying()) return;

        m_playbackAccumulator += deltaTime;
        float frameDuration = 1.0f / static_cast<float>(scene.GetFps());

        while (m_playbackAccumulator >= frameDuration) {
            m_playbackAccumulator -= frameDuration;
            int nextFrame = scene.GetCurrentFrame() + 1;
            if (nextFrame > scene.GetMaxFrame()) {
                nextFrame = scene.GetMinFrame();
            }
            scene.SetCurrentFrame(nextFrame);
        }
    }

    void TimelineManager::Render(Scene& scene) {
        ImGui::Begin("Timeline");

        // Transport Controls
        // Jump to Start (Shift+F)
        if (ImGui::Button("|<##Rewind")) {
            scene.SetCurrentFrame(scene.GetMinFrame());
        }
        ImGui::SameLine();

        // Step Back (F)
        if (ImGui::Button("<##StepBack")) {
            int f = scene.GetCurrentFrame() - 1;
            if (f < scene.GetMinFrame()) f = scene.GetMinFrame();
            scene.SetCurrentFrame(f);
        }
        ImGui::SameLine();

        // Play / Stop Forward (F8)
        const char* playText = scene.IsPlaying() ? "Stop (F8)" : "Play (F8)";
        if (scene.IsPlaying()) {
            ImGui::PushStyleColor(ImGuiCol_Button, C4DTheme::ColorOrangeAccent);
        }
        if (ImGui::Button(playText)) {
            scene.SetPlaying(!scene.IsPlaying());
        }
        if (scene.IsPlaying()) {
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();

        // Step Forward (G)
        if (ImGui::Button(">##StepForward")) {
            int f = scene.GetCurrentFrame() + 1;
            if (f > scene.GetMaxFrame()) f = scene.GetMaxFrame();
            scene.SetCurrentFrame(f);
        }
        ImGui::SameLine();

        // Jump to End (Shift+G)
        if (ImGui::Button(">|##ForwardEnd")) {
            scene.SetCurrentFrame(scene.GetMaxFrame());
        }
        ImGui::SameLine();

        // Record Keyframe (F9)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("● Key (F9)")) {
            // Keyframe recorded placeholder
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        // Current Frame Readout
        ImGui::TextColored(C4DTheme::ColorOrangeAccent, "[ %d F ]", scene.GetCurrentFrame());
        ImGui::SameLine();

        // Scrubber Slider
        int curFrame = scene.GetCurrentFrame();
        ImGui::SetNextItemWidth(-120.0f);
        if (ImGui::SliderInt("##Scrubber", &curFrame, scene.GetMinFrame(), scene.GetMaxFrame())) {
            scene.SetCurrentFrame(curFrame);
        }
        ImGui::SameLine();

        ImGui::TextDisabled("0 - %d F", scene.GetMaxFrame());

        ImGui::End();
    }

} // namespace CineMod
