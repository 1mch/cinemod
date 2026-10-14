#pragma once

#include "scene/Scene.hpp"

namespace CineMod {

    class CommandManager;

    class ObjectManager {
    public:
        ObjectManager() = default;
        void Render(Scene& scene, CommandManager& commandManager);

    private:
        void RenderObjectNode(BaseObject* obj, Scene& scene, CommandManager& commandManager);
        void RenderTrafficLightDot(const char* id, VisibilityMode mode, bool isUpper);
    };

} // namespace CineMod
