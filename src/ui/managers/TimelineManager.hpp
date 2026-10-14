#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class TimelineManager {
    public:
        TimelineManager() = default;
        void Render(Scene& scene);
        void Update(Scene& scene, float deltaTime);

    private:
        float m_playbackAccumulator = 0.0f;
    };

} // namespace CineMod
