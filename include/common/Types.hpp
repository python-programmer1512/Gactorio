#pragma once

#include <cstdint>

namespace gactorio {

using SimulationTime = double;

using ItemId = std::uint64_t;
using ProductId = std::uint64_t;
using RecipeId = std::uint64_t;
using MachineId = std::uint64_t;
using LineId = std::uint64_t;
using ProductionLineId = LineId;

enum class ItemType : std::uint64_t {
    Unknown = 0,
    Water = 1,
    Syrup = 2,
    CarbonDioxide = 3,
    Can = 4,
    Caffeine = 5,
    Label = 6
};

enum class ProductType : std::uint64_t {
    Unknown = 0,
    SodaCan = 101,
    SparklingWater = 102,
    EnergyDrink = 103
};

enum class ProcessType {
    Unknown,
    Carbonation,
    Conveying,
    Filling,
    Sealing,
    Labeling
};

enum class MachineRole {
    Unknown,
    Carbonator,
    Conveyor,
    Filler,
    Sealer,
    Labeler
};

enum class MachineStatus {
    Idle,
    Running,
    Working = Running,
    Paused,
    Blocked,
    Error,
    Broken = Error,
    Maintenance
};

enum class EventType {
    Info,
    TaskEnqueued,
    TaskStarted,
    StepCompleted,
    ProductCompleted,
    MachineBroken,
    MachineRepaired,
    StateChanged,
    InputsConsumed,
    MachinePaused
};

} // namespace gactorio
