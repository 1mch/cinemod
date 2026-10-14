#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace CineMod {

    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void Execute() = 0;
        virtual void Undo() = 0;
        virtual std::string GetName() const = 0;
    };

    class CommandManager {
    public:
        CommandManager() = default;

        void ExecuteCommand(std::unique_ptr<ICommand> cmd);
        bool CanUndo() const { return !m_undoStack.empty(); }
        bool CanRedo() const { return !m_redoStack.empty(); }

        void Undo();
        void Redo();

        void Clear();

        std::string GetUndoName() const;
        std::string GetRedoName() const;

    private:
        static constexpr size_t MAX_UNDO_STEPS = 100;
        std::vector<std::unique_ptr<ICommand>> m_undoStack;
        std::vector<std::unique_ptr<ICommand>> m_redoStack;
    };

} // namespace CineMod
