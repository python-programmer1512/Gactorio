# Beverage Factory Backend Summary

## Overview

This project is a C++17 backend simulation for a beverage production line. The active backend code lives under `include/` and the matching implementation folders `src/common`, `src/model`, `src/model/events`, `src/controller`, and `src/dto`.

The current default factory is `CarbonationFactory`, which builds a carbonation beverage line:

```text
Raw Material -> Carbonator -> Filler -> Sealer -> Labeler -> Finished Goods
```

Legacy UI prototype folders have been removed from `src`; the repository now keeps the backend-focused Model, Controller, DTO, and test code in the active CMake target.

## Build And Test

The top-level `CMakeLists.txt` defines:

- `gactorio_backend`: backend library
- `gactorio_backend_example`: sample executable using `src/main.cpp`
- Smoke test executables for model and controller behavior

Typical CMake commands:

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

In the current verification environment, `cmake` was not installed, so the backend and all smoke tests were verified with direct `g++ -std=c++17` compilation using the same active source files listed in `CMakeLists.txt`.

Verified smoke tests:

- `product_smoke_test`
- `inventory_smoke_test`
- `production_task_smoke_test`
- `sim_clock_smoke_test`
- `machine_smoke_test`
- `concrete_machine_smoke_test`
- `machine_state_smoke_test`
- `observer_smoke_test`
- `production_line_smoke_test`
- `factory_flow_smoke_test`
- `factory_inventory_enqueue_smoke_test`
- `factory_controller_smoke_test`

All verified smoke tests pass.

## Implemented Features

- Beverage product domain:
  - `SodaCan`
  - `SparklingWater`
  - `EnergyDrink`
- Beverage input materials:
  - `Water`
  - `Syrup`
  - `CarbonDioxide`
  - `Can`
  - `Caffeine`
  - `Label`
- Product recipes:
  - Each product exposes required input materials.
  - Each product exposes an ordered process route.
- Inventory:
  - Stores raw materials and finished goods separately.
  - Checks whether inputs are available.
  - Consumes inputs atomically.
  - Adds completed products to finished goods.
- Production task flow:
  - Tracks current process step.
  - Advances to the next step when a machine completes work.
  - Reports route progress.
  - Marks tasks complete after all route steps finish.
- Production line:
  - Maintains a task queue.
  - Assigns the front task to a machine matching the current step role.
  - Leaves work queued while matching machines are busy, paused, broken, or under maintenance.
  - Uses `Machine` polymorphism rather than concrete machine type checks.
- Machines:
  - `Carbonator`
  - `Filler`
  - `Sealer`
  - `Labeler`
  - `Conveyor` as an optional extensible machine type
- Machine operations:
  - `assignTask`
  - `update`
  - progress tracking
  - forced breakdown
  - repair and maintenance
  - pause and resume
- Factory aggregate:
  - Owns inventory, production lines, machine registry, event bus, event log, statistics, and simulation clock.
  - Consumes inputs before enqueueing products.
  - Updates all machines through base-class pointers.
  - Adds completed products back to finished goods inventory.
- Controller API:
  - Creates and resets a default carbonation factory.
  - Starts, pauses, stops, resets, and speeds up simulation time.
  - Enqueues beverage products by `ProductType`.
  - Breaks, repairs, pauses, and resumes machines by id.
  - Exposes read-only snapshot DTOs for UI use.
- Snapshot DTOs:
  - `FactorySnapshot`
  - `ProductionLineSnapshot`
  - `MachineSnapshot`
  - `InventorySnapshot`
  - `EventSnapshot`
  - `StatisticsSnapshot`
- Event system:
  - Publishes task, process, material, product, machine, and state events.
  - Stores UI-readable event logs.
  - Aggregates statistics from events.

## Major Class Responsibilities

### `Product`

Abstract base class for manufacturable beverage products. Concrete products define product id, display name, material requirements, and process route.

Concrete implementations:

- `SodaCan`
- `SparklingWater`
- `EnergyDrink`

### `Item`

Abstract base class for raw materials. Concrete item classes and `ItemTypeName` provide beverage material identity and names.

### `Inventory`

Encapsulates raw material and finished goods storage. It provides controlled methods for adding materials, consuming requirements atomically, checking quantities, and adding completed products.

### `ProductionTask`

Represents one product moving through its route. It tracks the current step index, current step, completion state, and total route progress.

### `Machine`

Abstract base class for production machines. It owns machine identity, status, current task, processing progress, health, state object, and event publishing hook.

Concrete machines extend `Machine` by implementing:

- `typeName()`
- `processType()`
- `role()`
- `canAcceptRecipe()`

### `MachineState`

Abstract base class for machine state objects. The current state controls how a machine behaves during `update`.

Concrete states:

- `IdleState`
- `WorkingState`
- `PausedState`
- `BrokenState`
- `MaintenanceState`

### `ProductionLine`

Owns machines as `std::unique_ptr<Machine>` and stores queued tasks. It assigns the current task to a machine whose `MachineRole` matches the current process step. It does not need concrete machine-type branches.

### `Factory`

Aggregates the simulation model. It owns production lines, inventory, simulation clock, event bus, event log observer, and statistics observer. The update loop runs machines through `Machine*` pointers.

### `CarbonationFactory`

Default factory configuration for the beverage domain. It initializes beverage inventory, beverage recipes, and the default carbonation line.

### `FactoryController`

MVC-style controller boundary for UI or application code. It converts commands into model operations and returns snapshot DTOs. UI code should use this API instead of directly mutating model objects.

### Snapshot DTOs

DTO classes expose read-only views of model state through getters. They are suitable for UI rendering and avoid exposing mutable model internals.

## Design Patterns

### State Pattern

`Machine` owns a `std::unique_ptr<MachineState>`. Each machine state implements `update(Machine&, double)`:

- `IdleState` transitions to working when work is available.
- `WorkingState` advances production.
- `PausedState` does nothing until resumed.
- `BrokenState` does nothing until repair is requested.
- `MaintenanceState` advances repair time and returns the machine to idle.

This keeps state-specific behavior out of large conditional blocks.

### Observer Pattern

`EventBus` stores `Observer*` subscribers. Model components publish `Event` objects, and observers react to them:

- `EventLogObserver` stores recent events for UI logs.
- `StatisticsObserver` updates aggregate counters for started tasks, completed steps, completed products, breakdowns, repairs, and state changes.

The model does not directly update log or statistics fields; it publishes events.

### MVC-Style Controller

The project does not currently include an active view layer, but it has a controller boundary:

- Model: `Factory`, `ProductionLine`, `Machine`, `Inventory`, `Product`, events
- Controller: `FactoryController`
- View-facing data: snapshot DTOs

This lets a future UI query state and send commands without directly owning or mutating model internals.

## OOP Requirement Check

- Abstraction: `Product`, `Item`, `Machine`, `MachineState`, and `Observer` define clear abstract contracts.
- Encapsulation: simulation classes use private fields and public methods/getters instead of public data members.
- Inheritance: beverage products, items, machines, states, and observers extend base classes.
- Polymorphism: production and factory loops use `Machine` base pointers/references.
- State Pattern: machine states are represented as separate objects.
- Observer Pattern: event logs and statistics are updated from published events.
- MVC separation: UI-facing access is through `FactoryController` and snapshot DTOs.
- Extensibility: adding a machine usually means deriving from `Machine` and assigning a role; the core production loop does not require concrete type changes.

## Current Limitations And Notes

- The simulation is deterministic aside from explicit breakdown calls; breakdown probability is stored but not yet used for random failures.
- `Conveyor` exists but the default beverage route currently uses `Carbonator -> Filler -> Sealer -> Labeler`.
- Recipes are present, but current task routing is product-route driven rather than recipe-driven.
- There is no persistence layer for saving/loading simulation state.
- There is no active UI target in the backend build; old prototype view/controller folders remain outside the active target.
- Finished goods are counted by product id in inventory; richer batch metadata is not implemented.
- Multi-line balancing is simple: each request is enqueued to the requested line id.
- The event log has an in-memory size cap and is not persisted.
