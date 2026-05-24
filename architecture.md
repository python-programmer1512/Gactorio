# Gactorio Backend Architecture

## Scope

This project implements only the C++ backend for a factory simulation.

The backend owns the Model and Controller layers of MVC. It does not implement
Dear ImGui, rendering, widgets, view classes, or any other UI code. The UI team
will communicate with the backend through controller APIs and read-only
snapshot/DTO structures.

## Folder Structure

```text
Gactorio/
  include/
    model/
      Item.hpp
      Product.hpp
      Recipe.hpp
      Inventory.hpp
      Machine.hpp
      MachineState.hpp
      MachineStates.hpp
      ProductionLine.hpp
      Factory.hpp
      CarbonationFactory.hpp
      events/
        IFactoryObserver.hpp
        FactoryEvent.hpp
        EventLog.hpp
        Statistics.hpp
    controller/
      FactoryController.hpp
      FactoryCommand.hpp
    dto/
      FactorySnapshot.hpp
      MachineSnapshot.hpp
      ProductionLineSnapshot.hpp
      InventorySnapshot.hpp
      EventSnapshot.hpp
      StatisticsSnapshot.hpp
  src/
    model/
      Item.cpp
      Product.cpp
      Recipe.cpp
      Inventory.cpp
      Machine.cpp
      MachineState.cpp
      MachineStates.cpp
      ProductionLine.cpp
      Factory.cpp
      CarbonationFactory.cpp
      events/
        EventLog.cpp
        Statistics.cpp
    controller/
      FactoryController.cpp
  tests/
    model/
    controller/
  docs/
    ...
  architecture.md
```

The exact file list may be adjusted during implementation, but the dependency
direction should stay the same.

## Layer Boundaries

### Model

The Model contains simulation state and behavior:

- factory topology
- production lines
- machines
- recipes
- inventories
- machine state transitions
- domain events
- statistics

The Model must not depend on Controller, DTO, or UI code.

### Controller

The Controller exposes use-case-oriented methods for the UI:

- create or reset a factory
- add, remove, or configure machines
- assign recipes
- start, stop, pause, or resume simulation entities
- advance simulation time
- retrieve read-only snapshots

The Controller depends on Model and DTO types. It should not expose mutable
Model objects directly.

### DTO / Snapshot

Snapshots are read-only data structures intended for UI consumption.

They contain plain values such as ids, names, counts, state names, progress,
event messages, and aggregate statistics. They do not contain pointers or
mutable references to Model objects.

The UI may cache snapshots for rendering, but it must request a new snapshot
after commands or simulation ticks.

### View / UI

Not implemented in this repository scope.

The View may depend on Controller and DTO types, but the backend must not depend
on View types.

## Dependency Direction

```text
UI/View
  -> controller
    -> dto
    -> model

model
  -> model/events

dto
  -> standard library value types only
```

Rules:

- Model classes do not include controller headers.
- Model classes do not include DTO headers unless a later implementation has a
  strong reason and keeps conversion one-way.
- Controller translates Model state into DTO snapshots.
- UI never receives non-const access to Model entities.
- Simulation update loops operate through base-class pointers or references.

## Major Classes

### Item

Represents a raw or intermediate material that can exist in inventory.

Responsibilities:

- store item id and display name
- provide value semantics
- act as a recipe input or inventory entry

`Item` is intentionally small. It does not simulate behavior.

### Product

Represents an output produced by a recipe or machine.

Responsibilities:

- store product id and display name
- optionally store category or metadata needed by the simulation
- act as a recipe output

`Product` can be separate from `Item` to satisfy the project structure and to
make the domain distinction explicit. If implementation later benefits from a
shared abstraction, both may derive from a common `Material` interface or value
base, but the public domain names should remain clear.

### Recipe

Defines how inputs are converted into outputs.

Responsibilities:

- store input requirements
- store output products
- store production duration
- expose read-only recipe metadata

`Recipe` is data-oriented and should not know which concrete machine executes
it.

### Inventory

Stores quantities of items and products.

Responsibilities:

- check whether required inputs are available
- consume inputs atomically
- add outputs
- expose read-only inventory contents

Inventory mutation is encapsulated behind methods such as `canConsume`,
`consume`, and `add`.

### Machine

Abstract base class for all machine types.

Responsibilities:

- expose a polymorphic simulation API such as `update(double deltaTime)`
- own or reference a `MachineState`
- hold machine id, name, assigned recipe, progress, and local inventory rules
- publish domain events through an observer interface
- provide virtual hooks for machine-specific production behavior

Expected interface shape:

```cpp
class Machine {
public:
    virtual ~Machine() = default;
    virtual void update(double deltaTime) = 0;
    virtual std::string typeName() const = 0;
    virtual bool canAcceptRecipe(const Recipe& recipe) const = 0;
};
```

Factory and ProductionLine must store machines as base-class pointers:

```cpp
std::vector<std::unique_ptr<Machine>> machines;
```

The simulation loop must call virtual functions on `Machine`. It must not branch
on concrete machine types with `if/else`, `switch`, or `dynamic_cast`.

### Concrete Machine Types

Examples:

- `Assembler`
- `Furnace`
- `Carbonator`
- `Packager`

Responsibilities:

- implement machine-specific recipe compatibility
- implement machine-specific production rules through overridden methods
- provide machine-specific display/type metadata

Adding a new machine type should require adding a new class and registering it
with construction code, not modifying the factory update loop.

### MachineState

Abstract base class for machine states.

Responsibilities:

- define state behavior for update, enter, and exit
- encapsulate transitions between states
- avoid state checks spread across `Machine`

Expected states:

- `IdleState`
- `RunningState`
- `PausedState`
- `BlockedState`
- `ErrorState`
- `MaintenanceState` if needed

Expected interface shape:

```cpp
class MachineState {
public:
    virtual ~MachineState() = default;
    virtual void enter(Machine& machine) {}
    virtual void update(Machine& machine, double deltaTime) = 0;
    virtual void exit(Machine& machine) {}
    virtual std::string name() const = 0;
};
```

The machine delegates state-dependent behavior to the current state object.

### ProductionLine

Owns and updates a group of machines.

Responsibilities:

- preserve machine ordering
- add or remove machines
- update all machines through `Machine*` or `std::unique_ptr<Machine>`
- coordinate shared line inventory if the design uses one
- publish or forward events from machines to the factory-level observers

The update loop must be polymorphic:

```cpp
for (auto& machine : machines) {
    machine->update(deltaTime);
}
```

No concrete machine type branching belongs here.

### Factory

Top-level simulation aggregate.

Responsibilities:

- own production lines
- own global inventory if needed
- own observer registration
- advance the simulation clock
- update production lines
- publish factory-level events

`Factory` should operate on abstractions. It does not need to know whether a
line contains an `Assembler`, `Carbonator`, or future machine type.

### CarbonationFactory

Concrete factory specialization.

Responsibilities:

- configure the default carbonation-oriented recipes, products, and machines
- provide project-specific setup while reusing the base `Factory` behavior

This demonstrates inheritance at the factory level without making the base
simulation loop depend on the concrete subclass.

### IFactoryObserver

Observer interface for simulation events.

Responsibilities:

- receive immutable event data
- allow multiple observers to react to the same event

Expected interface shape:

```cpp
class IFactoryObserver {
public:
    virtual ~IFactoryObserver() = default;
    virtual void onFactoryEvent(const FactoryEvent& event) = 0;
};
```

### FactoryEvent

Value object describing something that happened in the simulation.

Examples:

- machine started
- machine paused
- inputs consumed
- product completed
- output blocked
- error occurred

Fields may include:

- simulation time
- event type
- source id
- source name
- message
- related item or product quantities

### EventLog

Observer that stores recent events.

Responsibilities:

- implement `IFactoryObserver`
- append incoming events
- optionally limit retained history
- expose events to snapshots

### Statistics

Observer that aggregates metrics from events.

Responsibilities:

- implement `IFactoryObserver`
- count produced products
- count consumed inputs
- track machine state changes
- track uptime, blocked time, or throughput if needed
- expose aggregate values to snapshots

Statistics should be updated from events, not by the UI polling machines and
recomputing values.

### FactoryController

Main backend API for the UI team.

Responsibilities:

- own or reference the active `Factory`
- expose commands as stable public methods
- validate user requests
- translate backend state into snapshots
- hide mutable Model internals

Example API surface:

```cpp
class FactoryController {
public:
    void createDefaultCarbonationFactory();
    void reset();

    void tick(double deltaTime);
    void pauseMachine(MachineId id);
    void resumeMachine(MachineId id);
    void assignRecipe(MachineId machineId, RecipeId recipeId);
    void addMachine(ProductionLineId lineId, MachineTypeId typeId);
    void removeMachine(MachineId id);

    FactorySnapshot snapshot() const;
};
```

The exact id types may be aliases or small value classes.

## Snapshot / DTO Boundary

Snapshots are the only data shape the UI should use for rendering.

### FactorySnapshot

Contains:

- simulation time
- list of production line snapshots
- global inventory snapshot
- event log snapshot
- statistics snapshot
- available recipes and machine types if the UI needs menus

### ProductionLineSnapshot

Contains:

- line id
- line name
- ordered machine snapshots
- optional line-level throughput or blocked status

### MachineSnapshot

Contains:

- machine id
- machine name
- machine type name
- current state name
- assigned recipe name or id
- progress from `0.0` to `1.0`
- input/output inventory summaries
- last error message if any

### InventorySnapshot

Contains:

- item/product id
- display name
- quantity

### EventSnapshot

Contains:

- simulation timestamp
- event type
- source name
- message

### StatisticsSnapshot

Contains:

- total products produced by product id
- total items consumed by item id
- machine state counts
- throughput metrics if implemented

## Design Patterns

### Abstraction

The simulation loop depends on abstract interfaces and base classes:

- `Machine`
- `MachineState`
- `IFactoryObserver`
- optionally `Factory`

### Encapsulation

Mutable simulation details are hidden:

- Inventory changes go through inventory methods.
- Machine state changes go through machine/state methods.
- Factory internals are not exposed to UI.
- Controller returns snapshots instead of mutable references.

### Inheritance

Used where the domain has substitutable behavior:

- concrete machines inherit from `Machine`
- concrete states inherit from `MachineState`
- `CarbonationFactory` inherits from `Factory`
- observers inherit from `IFactoryObserver`

### Polymorphism

Factory and production lines update machines through base-class pointers.

State-specific behavior is executed through the `MachineState` interface.

Observer updates are dispatched through the `IFactoryObserver` interface.

### State Pattern

Each machine owns a current `MachineState`.

State objects decide how a machine behaves during `update` and when transitions
occur. This prevents `Machine` from becoming a large conditional block based on
state enum values.

### Observer Pattern

`Factory`, `ProductionLine`, or `Machine` publishes `FactoryEvent` values to
registered `IFactoryObserver` instances.

`EventLog` and `Statistics` subscribe to these events:

```text
Machine / ProductionLine / Factory
  -> notify FactoryEvent
    -> EventLog::onFactoryEvent
    -> Statistics::onFactoryEvent
```

The event publisher does not know how logs or statistics are stored.

### MVC

Backend scope:

- Model: simulation domain classes
- Controller: command API and snapshot API

Out of scope:

- View
- Dear ImGui
- rendering
- input widgets

## Simulation Update Flow

```text
UI calls FactoryController::tick(deltaTime)
  -> FactoryController validates deltaTime
  -> Factory::update(deltaTime)
    -> ProductionLine::update(deltaTime)
      -> Machine::update(deltaTime)
        -> current MachineState::update(machine, deltaTime)
          -> machine-specific virtual production hooks
          -> events are emitted when meaningful changes occur
    -> EventLog observer receives events
    -> Statistics observer receives events
  -> UI calls FactoryController::snapshot()
```

Important rule:

The update flow must not contain concrete machine branches such as:

```cpp
if (machine->typeName() == "Carbonator") { ... }
dynamic_cast<Carbonator*>(machine.get());
```

Machine-specific behavior belongs inside virtual methods of concrete machine
classes.

## Adding a New Machine Type

To add a new machine:

1. Create a class deriving from `Machine`.
2. Implement recipe compatibility and production behavior.
3. Add construction support in a factory method or machine registry used by the
   controller.
4. Add optional DTO display metadata.

Do not modify:

- `Factory::update`
- `ProductionLine::update`
- state dispatch logic
- observer dispatch logic

## Controller Contract for UI Team

The UI team should treat the backend as a command/query API.

Commands mutate simulation state:

- `tick`
- `reset`
- `addMachine`
- `removeMachine`
- `assignRecipe`
- `pauseMachine`
- `resumeMachine`

Queries return read-only DTOs:

- `snapshot`
- optional `availableRecipes`
- optional `availableMachineTypes`

The UI should not:

- hold raw `Machine*`, `Factory*`, or `Inventory*`
- mutate Model objects directly
- infer statistics by scanning internal Model state
- depend on concrete machine classes

The backend guarantees:

- stable ids in snapshots
- no mutable references exposed through DTOs
- update loop remains polymorphic
- adding a new machine type does not change factory update logic

## Initial Implementation Order

Recommended order:

1. Define ids, `Item`, `Product`, `Recipe`, and `Inventory`.
2. Define `FactoryEvent` and `IFactoryObserver`.
3. Implement `EventLog` and `Statistics`.
4. Define `MachineState` and basic states.
5. Define abstract `Machine` and one or two concrete machines.
6. Implement `ProductionLine` and `Factory` update loops.
7. Implement `CarbonationFactory` setup.
8. Implement DTO snapshots.
9. Implement `FactoryController`.
10. Add focused tests for polymorphic update, state transitions, observers, and
    snapshot immutability.
