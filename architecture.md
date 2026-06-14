# Gactorio Backend Architecture

## Scope

Gactorio is a C++ backend for an energy-drink factory simulation.

The backend owns the simulation Model, the command/snapshot Controller, and the
DTO types used by the UI boundary. UI implementation details such as Dear ImGui,
JavaScript views, rendering, widgets, and Emscripten bindings are outside the
core backend architecture. The UI should talk to the backend through
`FactoryController` and read-only snapshot DTOs.

## Current Folder Structure

```text
Gactorio/
  include/
    common/
      Config.h
      ScenarioType.hpp
      SimClock.hpp
      Types.hpp
    controller/
      Controller.h                  # web-facing compatibility facade
      ControllerConfigIdAdapters.hpp
      FactoryCommand.hpp
      FactoryController.hpp         # core backend controller
      SimulationHistory.hpp
    dto/
      EventSnapshot.hpp
      FactorySnapshot.hpp
      InventorySnapshot.hpp
      MachineSnapshot.hpp
      ProductionLineSnapshot.hpp
      StatisticsSnapshot.hpp
    model/
      CarbonationFactory.hpp
      ConfigurableFactory.hpp
      ConfiguredStation.hpp
      Factory.hpp
      FactoryBuilder.hpp
      Inventory.hpp
      Item.hpp
      Machine.hpp
      MachineState.hpp
      MachineStates.hpp
      Product.hpp
      ProductCatalog.hpp
      ProductionLine.hpp
      ProductionTask.hpp
      Recipe.hpp
      StationFactory.hpp
      config/
        ConfigIdAdapters.hpp
        DefinitionRegistry.hpp
        FactoryConfig.hpp
        FactoryConfigLoader.hpp
        FactoryRuntimeContext.hpp
      events/
        Event.hpp
        EventBus.hpp
        EventLogObserver.hpp
        Observer.hpp
        StatisticsObserver.hpp
      memento/
        FactoryMemento.hpp
  src/
    common/
    controller/
    dto/
    model/
      config/
      events/
    web/
      bindings.cpp                  # web binding layer, not BE model core
  tests/
    controller/
    model/
  data/
    factory_config.json
  architecture.md
```

The important dependency direction is:

```text
UI / bindings
  -> controller
    -> dto
    -> model
      -> common

model/config
  -> model/common value definitions

model/events
  -> event value types and observer interfaces
```

## Layer Boundaries

### Common

`include/common` contains shared primitive types and simulation-wide constants:

- ids and enums in `Types.hpp`
- scenario identifiers in `ScenarioType.hpp`
- simulation clock behavior in `SimClock.hpp`
- tuning constants in `Config.h`

Common types must stay lightweight. They should not depend on Controller, DTO,
or UI code.

### Model

The Model contains simulation state and behavior:

- factory aggregate state
- production lines and task queues
- machines, machine states, and machine health/progress
- product recipes and process steps
- inventory quantities
- event publication and observers
- config-driven factory definitions
- memento capture and restore

The Model must not depend on Controller, DTO, web bindings, or UI code.

### Controller

`FactoryController` is the main backend API. It validates commands, owns the
active `Factory`, maintains undo history through `SimulationHistory`, and
translates Model state into DTO snapshots.

The Controller may depend on Model and DTO types. It should not expose mutable
Model objects to UI callers.

`controller/Controller.h` and `src/web/bindings.cpp` are compatibility/web
boundary pieces. They may adapt the backend for JavaScript, but they are not the
core backend simulation model.

### DTO / Snapshot

DTOs are read-only value objects for UI consumption:

- `FactorySnapshot`
- `ProductionLineSnapshot`
- `MachineSnapshot`
- `InventorySnapshot`
- `EventSnapshot`
- `StatisticsSnapshot`

Snapshots copy values from the Model. They do not contain raw pointers,
ownership handles, or mutable references to Model internals.

### View / UI

The View may depend on Controller and DTO types. The backend must not depend on
View types.

## Major Runtime Flow

```text
UI calls FactoryController::tick(deltaTime)
  -> FactoryController validates the command
  -> Factory::update(realDeltaTime)
    -> SimClock advances simulation time
    -> ProductionLine::update(deltaTime)
      -> queued ProductionTask is assigned to available Machine
      -> Machine::update(deltaTime)
        -> current MachineState::update(machine, deltaTime)
        -> Machine advances step progress or transitions state
        -> EventBus publishes domain events
    -> ProductionLine collects completed step outputs and products
    -> Inventory receives produced items/products
    -> EventLogObserver and StatisticsObserver receive events
  -> UI requests FactoryController::snapshot()
  -> Controller returns DTO copies
```

The update loop is intentionally polymorphic. Factory and ProductionLine should
not branch on concrete machine classes to process work.

## Core Domain Classes

### Factory

`Factory` is the simulation aggregate root.

Responsibilities:

- own `Inventory`
- own `ProductionLine` instances
- own `EventBus`, `EventLogObserver`, and `StatisticsObserver`
- own `SimClock`
- keep a flattened non-owning `std::vector<Machine*>` cache for lookup
- enqueue products onto lines
- update the simulation
- publish or route events
- create and restore `FactoryMemento`

`Factory` exposes virtual hooks such as `createProductById` and
`createLineForMemento` so specialized factories can rebuild products and line
topologies from ids.

### CarbonationFactory

`CarbonationFactory` is the built-in energy-drink factory specialization.

Responsibilities:

- seed the default beverage production line
- create Voltz Classic, Hyper Bolt, and Aurora Zero products through the catalog
- add/remove beverage lines
- preserve next line/machine ids across memento restore
- rebuild the default line topology for restore

### ConfigurableFactory

`ConfigurableFactory` supports JSON/config-driven factories.

It works with `FactoryRuntimeContext`, `FactoryConfig`, `DefinitionRegistry`,
`FactoryBuilder`, `StationFactory`, and config id adapters to build runtime
factories from data rather than hard-coded product and station definitions.

### FactoryBuilder

`FactoryBuilder` converts a validated `FactoryConfig` into a runtime factory.

Responsibilities:

- create initial inventory
- create configured production lines
- create stations through `StationFactory`
- seed startup tasks
- apply default scenarios/settings

### DefinitionRegistry

`DefinitionRegistry` indexes config definitions by id and validates references.

It is the lookup boundary for config-defined items, products, stations, recipes,
lines, and scenarios.

### Product / Recipe / ProcessStep

`Product` is the abstract finished-good type. Built-in concrete products are:

- `VoltzClassic`
- `HyperBolt`
- `AuroraZero`

Each product has a default recipe id, display name, requirements, and route.
Routes are made of `ProcessStep` values. A process step has:

- a step id
- a string `stepKind`
- an optional legacy `MachineRole`
- a base duration
- step-level inputs
- step-level outputs

`Recipe` is the config-facing recipe value object. Config recipes are converted
into product/process-step runtime objects through the catalog and builder path.

### ProductionTask

`ProductionTask` is one unit of work moving through a product route.

Responsibilities:

- track the current process step
- expose current inputs and outputs
- mark step inputs as consumed
- collect pending step outputs
- report route progress
- create and restore `ProductionTaskMemento`

### Inventory

`Inventory` stores raw items and completed products.

Responsibilities:

- check and consume required inputs
- add item/product outputs
- restock configured or legacy items
- expose read-only quantity maps
- restore item/product maps for memento loading

### ProductionLine

`ProductionLine` owns an ordered set of machines and a queue of production
tasks.

Responsibilities:

- enqueue product tasks
- enforce optional queue capacity
- assign available tasks to machines
- update machines
- collect completed step outputs and completed products
- apply line scenarios such as bottlenecks or breakdown overrides
- create queue/task/machine memento data

Machines are owned as:

```cpp
std::vector<std::unique_ptr<Machine>> machines_;
```

### Machine

`Machine` is the abstract base class for runtime stations.

Responsibilities:

- store id, name, health, progress, status, assigned task, and optional recipe
- own the current `MachineState`
- update by delegating state behavior
- emit events through a non-owning `EventBus*`
- expose station compatibility through `role`, `stationKind`, and accepted steps
- support memento restore

Current concrete machine classes:

- `MixingStation`
- `QualityStation`
- `BottlingStation`
- `PackagingStation`
- `ConfiguredStation`

Adding a station should require adding a station class or config definition and
registering construction logic, not changing `Factory::update`.

### MachineState

`MachineState` is the abstract State-pattern role.

Concrete states:

- `IdleState`
- `WorkingState`
- `BrokenState`
- `MaintenanceState`

The state object decides what happens during `Machine::update`. State-specific
logic is kept out of large status-condition blocks in the factory update loop.

## Event / Observer Architecture

Events use a small Observer pattern:

```text
Machine / ProductionLine / Factory
  -> EventBus::publish(Event)
    -> EventLogObserver::onEvent
    -> StatisticsObserver::onEvent
```

### Event

`Event` is the immutable domain event value.

Events include types such as:

- `TaskEnqueued`
- `TaskStarted`
- `StepCompleted`
- `ProductCompleted`
- `MachineBroken`
- `MachineRepaired`
- `StateChanged`
- `InputsConsumed`
- `MachinePaused`

### EventBus

`EventBus` is the subject. It stores non-owning `Observer*` subscribers and
publishes each event to all subscribers.

### EventLogObserver

`EventLogObserver` records recent events for snapshots and UI display.

### StatisticsObserver

`StatisticsObserver` aggregates event-derived counters such as completed
products, started tasks, completed steps, broken machines, repaired machines,
and state changes.

Statistics should be derived from events rather than recomputed by the UI from
mutable model state.

## Memento / Undo Architecture

Memento support is split by GoF role:

```text
Factory
  -> createMemento()
  -> restoreFromMemento()

FactoryMemento
  -> owns LineMemento
    -> owns MachineMemento
    -> owns ProductionTaskMemento
      -> owns StepOutputMemento

FactoryController
  -> SimulationHistory
    -> stack of FactoryMemento
```

### Originator

`Factory` is the Originator. It captures and restores simulation state.

`CarbonationFactory` extends the memento behavior by preserving next line and
machine ids.

### Memento

`FactoryMemento` stores:

- simulation time
- inventory item quantities
- inventory product quantities
- line snapshots
- machine health/status/progress and station metadata
- queued and assigned production task state
- scenario and queue capacity data
- next line/machine ids when provided by the concrete factory

### Caretaker

`SimulationHistory` is the Caretaker. It owns a stack of `FactoryMemento`
objects but does not inspect their contents.

`FactoryController::saveCheckpoint`, `undo`, `canUndo`, and `historySize` are
the public API for this feature.

## Config Architecture

Config support lives under `model/config`.

Important types:

- `FactoryConfig`: parsed configuration data
- `FactoryConfigLoader`: JSON loading and parsing
- `DefinitionRegistry`: indexed, validated lookup layer
- `FactoryRuntimeContext`: owns config and registry for runtime access
- `ConfigIdAdapters`: maps config string ids to legacy enum/value ids

The config schema describes:

- factory settings
- item definitions
- product definitions
- station definitions
- recipe definitions
- production line topology
- initial inventory
- startup tasks
- scenario definitions

The controller can create factories from config via:

```cpp
FactoryController::createFromConfigFile(path);
FactoryController::createFromConfigString(jsonText);
```

## Snapshot Boundary

Snapshots are the only data shape the UI should use for rendering.

### FactorySnapshot

Contains:

- simulation time
- inventory snapshot
- statistics snapshot
- production line snapshots
- event snapshots

### ProductionLineSnapshot

Contains line-level display and queue information plus ordered machine
snapshots.

### MachineSnapshot

Contains machine id, display name, type/station kind, state/status, progress,
health, and other UI-facing machine values.

### InventorySnapshot

Contains item and product quantities by id/name.

### EventSnapshot

Contains event time, type, source, and message data.

### StatisticsSnapshot

Contains copied aggregate counters from `StatisticsObserver`.

## Controller Contract

The UI should treat `FactoryController` as a command/query API.

Commands mutate simulation state:

- `tick`
- `startSimulation`
- `pauseSimulation`
- `resumeSimulation`
- `stopSimulation`
- `resetSimulation`
- `setSpeed`
- `enqueueProductById`
- `enqueueAutoById`
- `addLine`
- `removeLine`
- `forceBreak`
- `repairMachine`
- `instantRepairMachine`
- `incrementalRepairMachine`
- `restockItemById`
- `pauseMachine`
- `resumeMachine`
- `setLineScenarioById`
- `saveCheckpoint`
- `undo`

Queries return read-only data:

- `snapshot`
- `getFactorySnapshot`
- `getEventLogs`
- `getStatistics`
- `availableProductDefinitions`
- `getLineScenario`
- `canUndo`
- `historySize`
- `runtimeContext`
- `config`
- `registry`

The UI should not:

- hold raw `Factory*`, `Machine*`, `Inventory*`, or `ProductionLine*`
- mutate Model objects directly
- infer statistics by scanning mutable Model internals
- depend on concrete machine subclasses
- depend on `src/web/bindings.cpp` as a model-layer contract

The backend guarantees:

- stable snapshot value objects
- no mutable references exposed through DTOs
- event-driven log/statistics updates
- memento-based undo through the controller
- config-driven factory construction for data-defined scenarios
- polymorphic machine and state dispatch in the simulation loop

## Design Patterns Used

### MVC Boundary

- Model: `model/*`, `model/config/*`, `model/events/*`, `model/memento/*`
- Controller: `FactoryController`, command result types, simulation history
- View: external UI/web layer using DTO snapshots

### Abstraction and Polymorphism

The update path depends on abstract/base roles:

- `Factory`
- `Machine`
- `MachineState`
- `Observer`

Concrete stations and states are substituted through virtual calls.

### Encapsulation

Mutable state is hidden behind domain methods:

- inventory mutation goes through `Inventory`
- machine state changes go through `Machine` and `MachineState`
- event delivery goes through `EventBus`
- UI reads DTO copies rather than Model objects

### State Pattern

`Machine` owns a `MachineState`. The current state handles update behavior and
transitions.

### Observer Pattern

`EventBus` publishes `Event` values to observers. `EventLogObserver` and
`StatisticsObserver` subscribe without changing the publisher.

### Memento Pattern

`Factory` captures and restores `FactoryMemento`. `SimulationHistory` stores
mementos as opaque checkpoints for controller-level undo.

### Factory / Builder

`FactoryBuilder` and `StationFactory` construct runtime objects from validated
config definitions. `CarbonationFactory` provides the built-in specialized
factory.

## Adding a New Config-Defined Product

For data-driven products, update `data/factory_config.json`:

1. Add item definitions if new inputs are needed.
2. Add a product definition.
3. Add a recipe with ordered step definitions.
4. Ensure station definitions accept the recipe step kinds.
5. Add the recipe to one or more production lines.
6. Add tests around config loading and production flow.

No `Factory::update` or `ProductionLine::update` change should be needed.

## Adding a New Built-In Station Class

For code-defined stations:

1. Derive a new class from `Machine`.
2. Implement `role()` and station metadata/compatibility behavior.
3. Register construction in `StationFactory` or the concrete factory setup.
4. Add snapshot/display mapping only if the existing metadata is insufficient.
5. Add focused tests for assignment, update behavior, and snapshots.

Do not modify the factory update loop for concrete station behavior.

## Test Coverage Map

The current tests are organized around:

- model smoke tests for products, recipes, inventory, machines, states, lines,
  factories, config, memento, observers, and scenarios
- controller smoke tests for command APIs, config loading, string ids, JSON-only
  extension behavior, and memento undo

New architecture-affecting work should add tests near the affected layer.
