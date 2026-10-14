#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class ObjectManager {
    public:
        ObjectManager() = default;
        void Render(Scene& scene);

    private:
        void RenderObjectNode(BaseObject* obj, Scene& scene);
        void RenderTrafficLightDot(const char* id, VisibilityMode mode, bool isUpper);
    };

} // namespace CineMod
