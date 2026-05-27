#pragma once

#include "common/Types.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace gactorio {

using TaskMementoId = std::uint64_t;

enum class MachineTypeKind {
    Carbonator,
    Filler,
    Conveyor,
    Sealer,
    Labeler
};

struct EventMemento {
    SimulationTime simulationTime = 0.0;
    EventType type = EventType::Info;
    MachineId sourceId = 0;
    std::string message;
};

struct EventLogMemento {
    std::size_t maxEvents = 0;
    std::vector<EventMemento> events;
};

struct StatisticsMemento {
    int completedProductEvents = 0;
    int startedTaskEvents = 0;
    int completedStepEvents = 0;
    int brokenMachineEvents = 0;
    int repairedMachineEvents = 0;
    int stateChangedEvents = 0;
};

struct InventoryMemento {
    std::map<ItemType, int> items;
    std::map<ProductId, int> products;
};

struct SimClockMemento {
    SimulationTime currentTime = 0.0;
    SimulationTime lastDeltaTime = 0.0;
    double speedMultiplier = 1.0;
    bool paused = false;
    bool stopped = false;
};

struct RecipeMemento {
    RecipeId id = 0;
    std::string name;
    double durationSeconds = 0.0;
    std::map<ItemType, int> inputs;
    std::map<ProductId, int> outputs;
};

struct ProductionTaskMemento {
    TaskMementoId taskId = 0;
    ProductId productId = 0;
    std::size_t currentStepIndex = 0;
};

struct MachineMemento {
    MachineTypeKind type = MachineTypeKind::Carbonator;
    MachineId id = 0;
    std::string name;
    MachineStatus status = MachineStatus::Idle;
    double rawProgress = 0.0;
    double health = 100.0;
    double processingSpeed = 1.0;
    double breakdownProbability = 0.0;
    double maintenanceElapsed = 0.0;
    double maintenanceDuration = 0.0;
    SimulationTime simulationTime = 0.0;
    std::optional<RecipeMemento> recipe;
    std::optional<TaskMementoId> assignedTaskId;
};

struct ProductionLineMemento {
    ProductionLineId id = 0;
    std::string name;
    std::vector<ProductionTaskMemento> taskQueue;
    std::vector<ProductId> completedProducts;
    std::vector<MachineMemento> machines;
};

struct FactoryMemento {
    SimClockMemento clock;
    InventoryMemento inventory;
    std::vector<RecipeMemento> recipes;
    std::vector<ProductionLineMemento> productionLines;
    EventLogMemento eventLog;
    StatisticsMemento statistics;
};

} // namespace gactorio
