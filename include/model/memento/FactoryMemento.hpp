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
//   - per-line: queued task state + per-machine HP
//
// Out of scope (intentionally not preserved):
//   - machine progress inside a single step (machines reset to Idle on restore;
//     queued task snapshots re-feed them naturally)
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

    MachineMemento(
        MachineId id,
        double health,
        MachineStatus status,
        double progress,
        std::string stationDefinitionId,
        std::string stationKind,
        std::string displayName,
        std::string typeName,
        double processingSpeed,
        double breakdownProbability,
        std::optional<std::size_t> assignedTaskIndex = std::nullopt)
        : id_(id),
          health_(health),
          status_(status),
          progress_(progress),
          stationDefinitionId_(std::move(stationDefinitionId)),
          stationKind_(std::move(stationKind)),
          displayName_(std::move(displayName)),
          typeName_(std::move(typeName)),
          processingSpeed_(processingSpeed),
          breakdownProbability_(breakdownProbability),
          assignedTaskIndex_(assignedTaskIndex) {}

    MachineId id() const { return id_; }
    double health() const { return health_; }
    MachineStatus status() const { return status_; }
    double progress() const { return progress_; }
    const std::string& stationDefinitionId() const { return stationDefinitionId_; }
    const std::string& stationKind() const { return stationKind_; }
    const std::string& displayName() const { return displayName_; }
    const std::string& typeName() const { return typeName_; }
    double processingSpeed() const { return processingSpeed_; }
    double breakdownProbability() const { return breakdownProbability_; }
    const std::optional<std::size_t>& assignedTaskIndex() const { return assignedTaskIndex_; }

private:
    MachineId     id_;
    double        health_;
    MachineStatus status_;
    double        progress_ = 0.0;
    std::string   stationDefinitionId_;
    std::string   stationKind_;
    std::string   displayName_;
    std::string   typeName_;
    double        processingSpeed_ = 1.0;
    double        breakdownProbability_ = 0.0;
    std::optional<std::size_t> assignedTaskIndex_;
};

class StepOutputMemento {
public:
    StepOutputMemento() = default;

    StepOutputMemento(
        std::optional<std::string> itemId,
        std::optional<ProductId> productId,
        int quantity)
        : itemId_(std::move(itemId)),
          productId_(std::move(productId)),
          quantity_(quantity) {}

    const std::optional<std::string>& itemId() const { return itemId_; }
    const std::optional<ProductId>& productId() const { return productId_; }
    int quantity() const { return quantity_; }

private:
    std::optional<std::string> itemId_;
    std::optional<ProductId> productId_;
    int quantity_ = 0;
};

class ProductionTaskMemento {
public:
    ProductionTaskMemento() = default;

    ProductionTaskMemento(
        ProductId productId,
        RecipeId recipeId,
        std::size_t currentStepIndex,
        bool currentStepInputsConsumed,
        std::vector<StepOutputMemento> pendingStepOutputs = {})
        : productId_(std::move(productId)),
          recipeId_(std::move(recipeId)),
          currentStepIndex_(currentStepIndex),
          currentStepInputsConsumed_(currentStepInputsConsumed),
          pendingStepOutputs_(std::move(pendingStepOutputs)) {}

    const ProductId& productId() const { return productId_; }
    const RecipeId& recipeId() const { return recipeId_; }
    std::size_t currentStepIndex() const { return currentStepIndex_; }
    bool currentStepInputsConsumed() const { return currentStepInputsConsumed_; }
    const std::vector<StepOutputMemento>& pendingStepOutputs() const {
        return pendingStepOutputs_;
    }

private:
    ProductId productId_;
    RecipeId recipeId_;
    std::size_t currentStepIndex_ = 0;
    bool currentStepInputsConsumed_ = false;
    std::vector<StepOutputMemento> pendingStepOutputs_;
};

class LineMemento {
public:
    LineMemento(
        LineId id,
        std::vector<ProductId> queueProductIds,
        std::vector<MachineMemento> machines,
        ScenarioType scenario = ScenarioType::NormalFlow,
        std::optional<std::size_t> queueCapacity = std::nullopt,
        std::size_t droppedTaskCount = 0,
        std::vector<ProductionTaskMemento> tasks = {},
        std::string definitionId = {},
        std::string displayName = {})
        : id_(id),
          queueProductIds_(std::move(queueProductIds)),
          machines_(std::move(machines)),
          scenario_(scenario),
          queueCapacity_(queueCapacity),
          droppedTaskCount_(droppedTaskCount),
          tasks_(std::move(tasks)),
          definitionId_(std::move(definitionId)),
          displayName_(std::move(displayName)) {
        if (tasks_.empty()) {
            tasks_.reserve(queueProductIds_.size());
            for (const auto& productId : queueProductIds_) {
                tasks_.emplace_back(productId, RecipeId{}, 0, false);
            }
        }
    }

    LineId id() const { return id_; }
    const std::vector<ProductId>& queueProductIds() const { return queueProductIds_; }
    const std::vector<MachineMemento>& machines() const { return machines_; }
    ScenarioType scenario() const { return scenario_; }
    const std::optional<std::size_t>& queueCapacity() const { return queueCapacity_; }
    std::size_t droppedTaskCount() const { return droppedTaskCount_; }
    const std::vector<ProductionTaskMemento>& tasks() const { return tasks_; }
    const std::string& definitionId() const { return definitionId_; }
    const std::string& displayName() const { return displayName_; }

private:
    LineId                       id_;
    std::vector<ProductId>       queueProductIds_;
    std::vector<MachineMemento>  machines_;
    ScenarioType                 scenario_;
    std::optional<std::size_t>   queueCapacity_;
    std::size_t                  droppedTaskCount_;
    std::vector<ProductionTaskMemento> tasks_;
    std::string                  definitionId_;
    std::string                  displayName_;
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
