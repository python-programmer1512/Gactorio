#pragma once

// =============================================================================
// SimulationHistory — the Caretaker in the Memento pattern.
//
// Owns a stack of FactoryMemento snapshots. The Controller pushes a new
// memento on saveCheckpoint(); the most recent one is removed via undo()
// and handed back to the Factory (the Originator) for restoration.
//
// Caretaker rules:
//   - Never inspects the contents of the memento (treats it as opaque).
//   - Has no knowledge of how snapshots are produced or applied.
// =============================================================================

#include "model/memento/FactoryMemento.hpp"

#include <optional>
#include <vector>

namespace gactorio {

class SimulationHistory {
public:
    void   push(FactoryMemento m);
    bool   canUndo()   const;
    std::optional<FactoryMemento> pop();
    void   clear();
    std::size_t size() const;

private:
    std::vector<FactoryMemento> snapshots_;
};

} // namespace gactorio
