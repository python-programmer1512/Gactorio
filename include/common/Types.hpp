#pragma once

#include <cstdint>
#include <string>

namespace gactorio {

using SimulationTime = double;

using ItemId           = std::uint64_t;
using ProductId        = std::string;
using RecipeId         = std::string;
using MachineId        = std::uint64_t;
using LineId           = std::uint64_t;
using ProductionLineId = LineId;

// -----------------------------------------------------------------------------
// Energy-drink domain types
// -----------------------------------------------------------------------------
enum class ItemType : std::uint64_t {
    Unknown     = 0,
    Ingredient  = 1,   // caffeine/taurine/sugar mix
    Water       = 2,   // distilled water
    EmptyBottle = 3,   // bottle to be filled
    Label       = 4,   // sticker label
    Package     = 5    // outer cardboard / shrink-wrap
};

enum class ProductType : std::uint64_t {
    Unknown      = 0,
    VoltzClassic = 101,   // standard energy drink (Voltz Classic)
    HyperBolt    = 102,   // premium energy drink   (Hyper Bolt)
    AuroraZero   = 103    // specialty zero-sugar   (Aurora Zero)
};

enum class ProcessType {
    Unknown,
    Mixing,      // ingredient + water + blend
    Quality,     // concentration check + carbonation
    Bottling,    // wash + fill + seal
    Packaging    // label + pack
};

enum class MachineRole {
    Unknown,
    Mixing,
    Quality,
    Bottling,
    Packaging
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
