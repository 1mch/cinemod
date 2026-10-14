#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class AttributeManager {
    public:
        AttributeManager() = default;
        void Render(Scene& scene);

    private:
        void RenderBasicTab(BaseObject* obj);
        void RenderCoordTab(BaseObject* obj);
        void RenderObjectTab(BaseObject* obj);
    };

} // namespace CineMod
