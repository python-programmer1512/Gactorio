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
    RawMaterial = 1,
    MetalPlate = 2,
    Screw = 3,
    Paint = 4
};

enum class ProductType : std::uint64_t {
    Unknown = 0,
    ToyCar = 101,
    MetalBox = 102,
    DroneFrame = 103
};

enum class ProcessType {
    Unknown,
    Assembly,
    Mixing,
    Carbonation,
    Packaging,
    Storage
};

enum class MachineRole {
    Unknown,
    Producer,
    Processor,
    Buffer,
    Output
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
