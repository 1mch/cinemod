#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class CoordinatesManager {
    public:
        CoordinatesManager() = default;
        void Render(Scene& scene);

    private:
        CoordSystem m_system = CoordSystem::World;
    };

} // namespace CineMod
