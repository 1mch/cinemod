#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class MaterialManager {
    public:
        MaterialManager() = default;
        void Render(Scene& scene);

    private:
        void RenderEditor(Scene& scene);
        bool m_showEditor = false;
    };

} // namespace CineMod
