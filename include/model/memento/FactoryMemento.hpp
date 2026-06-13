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

#include "common/ScenarioType.hpp"
#include "common/Types.hpp"

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace gactorio {

class MachineMemento {
public:
    MachineMemento(MachineId id, double health, MachineStatus status)
        : id_(id), health_(health), status_(status) {}

    MachineId id() const { return id_; }
    double health() const { return health_; }
    MachineStatus status() const { return status_; }

private:
    MachineId     id_;
    double        health_;
    MachineStatus status_;
};

class LineMemento {
public:
    LineMemento(
        LineId id,
        std::vector<ProductId> queueProductIds,
        std::vector<MachineMemento> machines,
        ScenarioType scenario = ScenarioType::NormalFlow,
        std::optional<std::size_t> queueCapacity = std::nullopt,
        std::size_t droppedTaskCount = 0)
        : id_(id),
          queueProductIds_(std::move(queueProductIds)),
          machines_(std::move(machines)),
          scenario_(scenario),
          queueCapacity_(queueCapacity),
          droppedTaskCount_(droppedTaskCount) {}

    LineId id() const { return id_; }
    const std::vector<ProductId>& queueProductIds() const { return queueProductIds_; }
    const std::vector<MachineMemento>& machines() const { return machines_; }
    ScenarioType scenario() const { return scenario_; }
    const std::optional<std::size_t>& queueCapacity() const { return queueCapacity_; }
    std::size_t droppedTaskCount() const { return droppedTaskCount_; }

private:
    LineId                       id_;
    std::vector<ProductId>       queueProductIds_;
    std::vector<MachineMemento>  machines_;
    ScenarioType                 scenario_;
    std::optional<std::size_t>   queueCapacity_;
    std::size_t                  droppedTaskCount_;
};

class FactoryMemento {
public:
    FactoryMemento(
        SimulationTime simulationTime,
        std::map<std::string, int> items,
        std::map<ProductId, int> products)
        : simulationTime_(simulationTime),
          items_(std::move(items)),
          products_(std::move(products)) {}

    SimulationTime simulationTime() const { return simulationTime_; }
    const std::map<std::string, int>& items() const { return items_; }
    const std::map<ProductId, int>& products() const { return products_; }
    const std::vector<LineMemento>& lines() const { return lines_; }
    LineId nextLineId() const { return nextLineId_; }
    MachineId nextMachineId() const { return nextMachineId_; }

    void addLine(LineMemento line) {
        lines_.push_back(std::move(line));
    }

    void setNextIds(LineId nextLineId, MachineId nextMachineId) {
        nextLineId_ = nextLineId;
        nextMachineId_ = nextMachineId;
    }

private:
    SimulationTime                simulationTime_;
    std::map<std::string, int>    items_;
    std::map<ProductId, int>      products_;
    std::vector<LineMemento>      lines_;
    LineId                        nextLineId_ = 0;
    MachineId                     nextMachineId_ = 0;
};

} // namespace gactorio
