# Gactorio Backend

Gactorio is a C++17 backend-only factory simulation project. It implements the
Model and Controller parts of MVC. UI, Dear ImGui, rendering, and View code are
intentionally not included.

## Build and Run

```powershell
cmake -S . -B build
cmake --build build
.\build\gactorio_backend_example.exe
```

If CMake is not available, the project can also be compiled directly with a
C++17 compiler such as `g++`.

## Backend Structure

- `include/common`, `src/common`: shared ids, enums, and `SimClock`
- `include/model`, `src/model`: factory domain model
- `include/model/events`, `src/model/events`: Observer Pattern event system
- `include/dto`, `src/dto`: read-only snapshots for UI consumption
- `include/controller`, `src/controller`: backend API boundary for the UI team
- `tests/model`: smoke tests for backend behavior

The default factory flow is:

```text
Raw Material -> Cutter -> Conveyor C1 -> Assembler -> Conveyor C2 -> Painter -> Finished Goods
```

## Controller API

The UI team should access the backend through `FactoryController`.

Main commands:

- `startSimulation()`
- `pauseSimulation()`
- `resetSimulation()`
- `setSpeed(double)`
- `enqueueProduct(LineId, ProductType)`
- `forceBreak(MachineId)`
- `repairMachine(MachineId)`

Read-only queries:

- `getFactorySnapshot()`
- `getEventLogs()`
- `getStatistics()`

The UI should not keep mutable pointers to `Factory`, `ProductionLine`,
`Machine`, or `Inventory`.

## Snapshots / DTOs

DTOs are read-only value objects:

- `MachineSnapshot`
- `LineSnapshot` / `ProductionLineSnapshot`
- `InventorySnapshot`
- `FactorySnapshot`
- `StatisticsSnapshot`
- `EventSnapshot`

They expose getters only and do not contain mutable model references.

## OOP Principles

- Abstraction: `Machine`, `MachineState`, `Product`, `Item`, and `Observer`
  define abstract contracts.
- Encapsulation: model data is private/protected and exposed through methods or
  snapshots.
- Inheritance: concrete machines, products, items, states, and observers derive
  from base classes.
- Polymorphism: `Factory` updates machines through `Machine*` base pointers.
  `ProductionLine` assigns tasks using `canProcess(role)` and `canAcceptTask()`
  without concrete type checks.

## State Pattern

Machine state is implemented with:

- `MachineState`
- `IdleState`
- `WorkingState`
- `BrokenState`
- `MaintenanceState`

`Machine::update()` delegates behavior to the current state object. State
transitions are encapsulated inside `Machine`, and transition hooks publish
model events.

## Observer Pattern

Model events flow through:

- `Event`
- `EventBus`
- `Observer`
- `EventLogObserver`
- `StatisticsObserver`

Machines and factory model objects publish events such as `TaskEnqueued`,
`TaskStarted`, `StepCompleted`, `ProductCompleted`, `MachineBroken`,
`MachineRepaired`, and `StateChanged`. Observers subscribe to model events only;
they do not know about UI.

## Adding a Machine

1. Derive a new class from `Machine`.
2. Override `typeName()`, `processType()`, `role()`, and `canAcceptRecipe()`.
3. Configure processing speed, initial health, and breakdown probability through
   the base constructor.
4. Add the machine to a `ProductionLine`.

No changes are required in `Factory::update()` or `ProductionLine` routing.

## Adding a Product

1. Add a `ProductType` entry in `common/Types.hpp`.
2. Derive a new class from `Product`.
3. Provide product id, name, item requirements, and process route.
4. Add product construction in the controller product factory.

The process route is a list of `ProcessStep` values, each requiring a
`MachineRole` and a base duration.

## Tests

Smoke tests cover:

- inventory atomic consume
- product route definitions
- machine task processing
- state transitions
- factory update loop
- observer event log/statistics

Run through CTest after a CMake build:

```powershell
ctest --test-dir build
```
