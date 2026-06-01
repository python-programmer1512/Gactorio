#pragma once

// =============================================================================
// MVC Controller layer.
//
// This is the ONLY header the View layer is allowed to include from the
// controller / model side. It exposes view-friendly data types (plain structs
// of strings and numbers) and a Controller class that hides the Model entirely
// (PImpl). The View never sees a single gactorio::* symbol.
//
// Model code likewise never includes this header — communication is one-way
// from View through Controller into Model, and back as immutable FactoryView.
// =============================================================================

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ctrl {

// uint32 keeps embind happy without -sWASM_BIGINT and is plenty for our IDs.
using LineId    = std::uint32_t;
using MachineId = std::uint32_t;

enum class ProductKind {
    Unknown,
    VoltzClassic,
    HyperBolt,
    AuroraZero
};

// -----------------------------------------------------------------------------
// View-side DTOs
// -----------------------------------------------------------------------------
struct MachineView {
    MachineId   id;
    std::string name;
    std::string type;       // "Mixing Station" etc.
    std::string state;      // "Idle" / "Working" / "Broken" / "Maintenance"
    double      progress;   // 0.0 .. 1.0
    double      health;     // 0.0 .. 100.0
};

struct LineView {
    LineId      id;
    std::string name;
    std::size_t queueLength;
    std::string currentTaskName;       // empty if idle
    double      currentTaskProgress;   // 0.0 .. 1.0
    std::vector<MachineView> machines;
};

struct EventView {
    double      time;       // simulation seconds
    std::string typeName;   // "TaskStarted" / "ProductCompleted" / ...
    std::string message;
};

struct InventoryEntry {
    std::string id;         // already human-readable: "Ingredient", "Voltz Classic"
    int         quantity;
};

struct Statistics {
    int tasksStarted     = 0;
    int stepsCompleted   = 0;
    int productsDone     = 0;
    int machinesBroken   = 0;
    int machinesRepaired = 0;
    int stateChanges     = 0;
};

struct FactoryView {
    double                       simulationTime = 0.0;
    Statistics                   stats;
    std::vector<LineView>        lines;
    std::vector<EventView>       events;
    std::vector<InventoryEntry>  inventory;
};

// -----------------------------------------------------------------------------
// Controller — single point of contact between View and Model.
// -----------------------------------------------------------------------------
class Controller {
public:
    Controller();
    ~Controller();
    Controller(const Controller&)            = delete;
    Controller& operator=(const Controller&) = delete;

    // ---- Commands (mutate Model) --------------------------------------------
    void tick(double deltaTime);
    void pause();
    void resume();
    void reset();
    void setSpeed(double multiplier);
    bool enqueue      (LineId line,    ProductKind product);
    bool breakMachine (MachineId id);
    bool repair       (MachineId id);

    // ---- Query (read Model, returns cached View) ----------------------------
    const FactoryView& snapshot() const;

private:
    struct Impl;                              // PImpl hides all gactorio:: types
    std::unique_ptr<Impl> m_impl;
};

} // namespace ctrl
