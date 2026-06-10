#pragma once

// =============================================================================
// Memento Pattern — checkpoint structures for the factory simulation.
//
// FactoryMemento captures a snapshot of Factory state at a point in time.
// It is the Originator's (Factory) output and the Caretaker's
// (SimulationHistory) opaque payload. Restoring a memento brings the
// simulation back to the captured state.
//
// Scope:
//   - simulation time
//   - inventory (raw items + finished products)
//   - per-line: queued product IDs + per-machine HP
//
// Out of scope (intentionally not preserved):
//   - in-flight ProductionTasks  (machines reset to Idle on restore; queues
//     re-feed them naturally)
//   - statistics counters and event log (carry forward — they describe
//     real history, not state)
// =============================================================================

#include "common/Types.hpp"

#include <cstddef>
#include <map>
#include <vector>

namespace gactorio {

struct MachineMemento {
    MachineId     id;
    double        health;
    MachineStatus status;
};

struct LineMemento {
    LineId                       id;
    std::vector<ProductId>       queueProductIds;
    std::vector<MachineMemento>  machines;
};

struct FactoryMemento {
    SimulationTime                simulationTime;
    std::map<ItemType, int>       items;
    std::map<ProductId, int>      products;
    std::vector<LineMemento>      lines;
};

} // namespace gactorio
