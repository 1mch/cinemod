#include "C4DTheme.hpp"

namespace CineMod {

    void C4DTheme::ApplyTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Base C4D Dark Theme
        colors[ImGuiCol_Text]                  = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.169f, 0.169f, 0.169f, 1.00f); // #2B2B2B
        colors[ImGuiCol_ChildBg]               = ImVec4(0.145f, 0.145f, 0.145f, 1.00f); // #252525
        colors[ImGuiCol_PopupBg]               = ImVec4(0.180f, 0.180f, 0.180f, 0.98f);
        colors[ImGuiCol_Border]                = ImVec4(0.120f, 0.120f, 0.120f, 1.00f); // #1F1F1F
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        
        // Frame / Inputs
        colors[ImGuiCol_FrameBg]               = ImVec4(0.118f, 0.118f, 0.118f, 1.00f); // #1E1E1E
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);

        // Titles & Docking Tabs
        colors[ImGuiCol_TitleBg]               = ImVec4(0.140f, 0.140f, 0.140f, 1.00f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.120f, 0.120f, 0.120f, 1.00f);

        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.150f, 0.150f, 0.150f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.120f, 0.120f, 0.120f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.300f, 0.300f, 0.300f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.400f, 0.400f, 0.400f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.500f, 0.500f, 0.500f, 1.00f);

        // Checkmark & Sliders
        colors[ImGuiCol_CheckMark]             = ColorOrangeAccent;
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.380f, 0.380f, 0.380f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]      = ColorOrangeAccent;

        // Buttons
        colors[ImGuiCol_Button]                = ImVec4(0.220f, 0.220f, 0.220f, 1.00f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.280f, 0.280f, 0.280f, 1.00f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.350f, 0.350f, 0.350f, 1.00f);

        // Headers (Collapsing headers, tree nodes)
        colors[ImGuiCol_Header]                = ImVec4(0.220f, 0.220f, 0.220f, 1.00f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.280f, 0.280f, 0.280f, 1.00f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.350f, 0.350f, 0.350f, 1.00f);

        // Separators
        colors[ImGuiCol_Separator]             = ImVec4(0.120f, 0.120f, 0.120f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]      = ColorOrangeAccent;
        colors[ImGuiCol_SeparatorActive]       = ColorOrangeAccent;

        // Resize Grips
        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.260f, 0.260f, 0.260f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered]     = ColorOrangeAccent;
        colors[ImGuiCol_ResizeGripActive]      = ColorOrangeAccent;

        // Docking Tabs
        colors[ImGuiCol_Tab]                   = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.260f, 0.260f, 0.260f, 1.00f);
        colors[ImGuiCol_TabActive]             = ImVec4(0.220f, 0.220f, 0.220f, 1.00f);
        colors[ImGuiCol_TabUnfocused]          = ImVec4(0.140f, 0.140f, 0.140f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.180f, 0.180f, 0.180f, 1.00f);
        colors[ImGuiCol_DockingPreview]        = ImVec4(0.92f, 0.58f, 0.05f, 0.35f); // Orange docking preview!
        colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.120f, 0.120f, 0.120f, 1.00f);

        // Styling geometry
        style.WindowRounding    = 2.0f;
        style.ChildRounding     = 2.0f;
        style.FrameRounding     = 2.0f;
        style.PopupRounding     = 2.0f;
        style.ScrollbarRounding = 2.0f;
        style.GrabRounding      = 2.0f;
        style.TabRounding       = 2.0f;

        style.WindowBorderSize  = 1.0f;
        style.ChildBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;
        style.FrameBorderSize   = 1.0f;
        style.TabBorderSize     = 1.0f;

        style.WindowPadding     = ImVec2(6.0f, 6.0f);
        style.FramePadding      = ImVec2(5.0f, 3.0f);
        style.ItemSpacing       = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);
        style.IndentSpacing     = 16.0f;
        style.ScrollbarSize     = 12.0f;
    }

} // namespace CineMod
