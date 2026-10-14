#pragma once

#include <string>
#include <vector>
#include <functional>
#include "core/Types.hpp"

namespace CineMod {

    class Scene;
    class ViewportRenderer;
    class CommandManager;

    struct CommanderItem {
        std::string name;
        std::string shortcut;
        std::string category; // "Create", "Tool", "Edit", "View", "Animation"
        std::function<void()> action;
    };

    class Commander {
    public:
        Commander();
        ~Commander() = default;

        void Open();
        void Close();
        void Toggle();
        bool IsOpen() const { return m_isOpen; }

        void Render(Scene& scene, ViewportRenderer& renderer, ToolMode& currentTool, CommandManager& commandManager);

    private:
        bool m_isOpen = false;
        char m_searchQuery[128] = "";
        int m_selectedIndex = 0;
        bool m_focusSearch = false;
        std::vector<CommanderItem> m_items;

        void PopulateItems(Scene& scene, ViewportRenderer& renderer, ToolMode& currentTool, CommandManager& commandManager);
    };

} // namespace CineMod
