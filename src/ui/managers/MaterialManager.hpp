#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class MaterialManager {
    public:
        MaterialManager() = default;
        void Render(Scene& scene);
    };

} // namespace CineMod
