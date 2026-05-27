#pragma once

#include "model/memento/FactoryMemento.hpp"

#include <cstddef>
#include <optional>
#include <vector>

namespace gactorio {

class SimulationHistory {
public:
    explicit SimulationHistory(std::size_t maxHistorySize = 100);

    void clear();
    void record(FactoryMemento memento);
    bool canUndo() const;
    bool canRedo() const;
    std::optional<FactoryMemento> undo(FactoryMemento current);
    std::optional<FactoryMemento> redo(FactoryMemento current);

private:
    std::size_t maxHistorySize_;
    std::vector<FactoryMemento> undoStack_;
    std::vector<FactoryMemento> redoStack_;
};

} // namespace gactorio
