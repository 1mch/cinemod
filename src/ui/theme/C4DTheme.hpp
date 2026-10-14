#pragma once

#include <imgui.h>

namespace CineMod {

    class C4DTheme {
    public:
        static void ApplyTheme();

        // Helper Cinema 4D Colors
        static constexpr ImVec4 ColorOrangeAccent = ImVec4(0.92f, 0.58f, 0.05f, 1.0f); // #EA940D
        static constexpr ImVec4 ColorAxisX        = ImVec4(0.91f, 0.30f, 0.24f, 1.0f); // #E74C3C
        static constexpr ImVec4 ColorAxisY        = ImVec4(0.18f, 0.80f, 0.44f, 1.0f); // #2ECC71
        static constexpr ImVec4 ColorAxisZ        = ImVec4(0.20f, 0.60f, 0.86f, 1.0f); // #3498DB

        static constexpr ImVec4 ColorTrafficDefault = ImVec4(0.50f, 0.50f, 0.50f, 1.0f); // Gray
        static constexpr ImVec4 ColorTrafficVisible = ImVec4(0.20f, 0.85f, 0.30f, 1.0f); // Green
        static constexpr ImVec4 ColorTrafficHidden  = ImVec4(0.90f, 0.20f, 0.20f, 1.0f); // Red
    };

} // namespace CineMod
