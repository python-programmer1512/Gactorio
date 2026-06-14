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
//
// UML: BE_Overall_Class_Diagram (ctrl::Controller --▷ delegates to
// gactorio::FactoryController via PImpl) and FE_JS_View_Class_Diagram (the JS
// View only ever talks to Module.Controller, the embind projection of this).
// =============================================================================

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ctrl {

// uint32 keeps embind happy without -sWASM_BIGINT and is plenty for our IDs.
using LineId    = std::uint32_t;
using MachineId = std::uint32_t;
using ProductId = std::uint32_t;
using ItemId    = std::uint32_t;

enum class ProductKind : std::uint32_t {
    Unknown      = 0,
    VoltzClassic = 101,
    HyperBolt    = 102,
    AuroraZero   = 103
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
    std::string scenarioId;
    std::string scenarioName;
    std::size_t queueCapacity = 0;
    std::size_t droppedTaskCount = 0;
    bool        isRemovable;           // true ⇒ line can be safely deleted
    std::vector<MachineView> machines;
};

struct EventView {
    double      time;       // simulation seconds
    std::string typeName;   // "TaskStarted" / "ProductCompleted" / ...
    std::string message;
};

struct InventoryEntry {
    std::string id;          // stable item/product id, e.g. "ingredient"
    ItemId      legacyId = 0; // numeric compatibility id, or 0 if unknown
    std::string name;        // compatibility display name
    std::string displayName;
    std::string kind;
    int         quantity = 0;
    bool        isProduct = false;
    bool        restockable = false;
    int         restockAmount = 0;
};

struct ProductOption {
    std::string id;         // stable product id, e.g. "voltz_classic"
    ProductId   legacyId = 0;
    std::string key;        // compatibility UI key
    std::string name;
    std::string displayName;
    std::string defaultRecipeId;
    std::string tier;
    std::string color;
    double      durationSeconds = 0.0;
    std::string requirements; // "Ingredient x2, Water x1, ..."
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
    // Numeric-id enqueue (kept because the smoke tests pin the ProductId↔string
    // mapping); the View itself only ever calls the *ById string overloads.
    bool enqueueProduct(LineId line, ProductId product);
    bool enqueueProductById(LineId line, const std::string& productId);
    // Enqueue to whichever line currently has the smallest queue.
    LineId enqueueAutoProductById(const std::string& productId);
    // Spawn a new beverage line. Returns its LineId (0 on failure).
    LineId addLine();
    // Remove a line. Returns false if the line is busy or unknown.
    bool   removeLine (LineId id);
    bool breakMachine (MachineId id);
    // Always-available quick repair: +config::kIncrementalRepairHp HP.
    bool repair       (MachineId id);
    bool instantRepair(MachineId id);
    // Add 5 units of one raw inventory item. Product IDs are rejected.
    bool restockItem  (ItemId id);
    bool restockItemById(const std::string& itemId);
    // Only meaningful when the machine is Broken (HP=0). Triggers a long
    // maintenance routine (config::kRepairAllDelaySeconds) that fully
    // restores HP and resumes the paused task from the start of its step.
    bool repairAll    (MachineId id);
    bool setLineScenario(LineId line, const std::string& scenarioId);
    bool loadFactoryConfigFromString(const std::string& jsonText);
    void clearEventLog();

    // ---- Memento (snapshot history) ----------------------------------------
    // Capture current Factory state and push onto an internal stack.
    void   saveCheckpoint();
    // Pop the most recent snapshot and restore Factory state from it.
    bool   undo();
    bool   canUndo()     const;
    std::size_t historySize() const;

    // ---- Query (read Model, returns cached View) ----------------------------
    const FactoryView& snapshot() const;
    std::string getLineScenario(LineId line) const;
    const std::vector<ProductOption>& products() const;

private:
    struct Impl;                              // PImpl hides all gactorio:: types
    std::unique_ptr<Impl> m_impl;
};

} // namespace ctrl
