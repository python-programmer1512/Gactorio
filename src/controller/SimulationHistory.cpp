#include "controller/SimulationHistory.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

SimulationHistory::SimulationHistory(std::size_t maxHistorySize)
    : maxHistorySize_(std::max<std::size_t>(1, maxHistorySize)) {}

void SimulationHistory::clear() {
    undoStack_.clear();
    redoStack_.clear();
}

void SimulationHistory::record(FactoryMemento memento) {
    undoStack_.push_back(std::move(memento));
    if (undoStack_.size() > maxHistorySize_) {
        undoStack_.erase(undoStack_.begin());
    }
    redoStack_.clear();
}

bool SimulationHistory::canUndo() const {
    return !undoStack_.empty();
}

bool SimulationHistory::canRedo() const {
    return !redoStack_.empty();
}

std::optional<FactoryMemento> SimulationHistory::undo(FactoryMemento current) {
    if (!canUndo()) {
        return std::nullopt;
    }

    auto previous = std::move(undoStack_.back());
    undoStack_.pop_back();
    redoStack_.push_back(std::move(current));
    if (redoStack_.size() > maxHistorySize_) {
        redoStack_.erase(redoStack_.begin());
    }
    return previous;
}

std::optional<FactoryMemento> SimulationHistory::redo(FactoryMemento current) {
    if (!canRedo()) {
        return std::nullopt;
    }

    auto next = std::move(redoStack_.back());
    redoStack_.pop_back();
    undoStack_.push_back(std::move(current));
    if (undoStack_.size() > maxHistorySize_) {
        undoStack_.erase(undoStack_.begin());
    }
    return next;
}

} // namespace gactorio
