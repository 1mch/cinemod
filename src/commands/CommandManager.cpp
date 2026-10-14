#include "CommandManager.hpp"

namespace CineMod {

    void CommandManager::ExecuteCommand(std::unique_ptr<ICommand> cmd) {
        if (!cmd) return;
        cmd->Execute();
        m_undoStack.push_back(std::move(cmd));
        m_redoStack.clear();

        if (m_undoStack.size() > MAX_UNDO_STEPS) {
            m_undoStack.erase(m_undoStack.begin());
        }
    }

    void CommandManager::Undo() {
        if (m_undoStack.empty()) return;

        auto cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();

        cmd->Undo();
        m_redoStack.push_back(std::move(cmd));
    }

    void CommandManager::Redo() {
        if (m_redoStack.empty()) return;

        auto cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();

        cmd->Execute();
        m_undoStack.push_back(std::move(cmd));
    }

    void CommandManager::Clear() {
        m_undoStack.clear();
        m_redoStack.clear();
    }

    std::string CommandManager::GetUndoName() const {
        if (m_undoStack.empty()) return "";
        return m_undoStack.back()->GetName();
    }

    std::string CommandManager::GetRedoName() const {
        if (m_redoStack.empty()) return "";
        return m_redoStack.back()->GetName();
    }

} // namespace CineMod
