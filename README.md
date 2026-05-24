# Gactorio

**Gactorio = GIST + Factorio.** Theme: Energy Drink factory simulator.

A C++17 OOP project for the OOP with C++ course (GIST EECS, Spring 2026).
Combines a pure simulation backend (Model + Controller) with a Dear ImGui
front-end (View) wired together through read-only DTO snapshots.

```
GUI (ImGui)  →  FactoryController  →  Snapshot  →  GUI renders
                       ↓ commands
                   Factory (Model)
```

## Requirements

- **MSYS2** (UCRT64 environment) on Windows
- Packages: `gcc`, `cmake`, `ninja`, `glfw`, `pkgconf`

Install once:

```powershell
# In MSYS2 UCRT64 shell
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
                   mingw-w64-ucrt-x86_64-cmake \
                   mingw-w64-ucrt-x86_64-ninja \
                   mingw-w64-ucrt-x86_64-glfw \
                   mingw-w64-ucrt-x86_64-pkgconf
```

Then add `C:\msys64\ucrt64\bin` to your user `Path`.

## Build & Run

```powershell
git clone https://github.com/python-programmer1512/Gactorio.git
cd Gactorio
cmake -S . -B build -G Ninja
cmake --build build
.\build\gactorio.exe
```

The GUI window shows three panels:

- **Simulation Control** — pause/resume, reset, speed slider, live statistics
- **Factory** — per-line production with machine status table and product enqueue buttons
- **Event Log** — Observer-pattern events (TaskStarted, ProductCompleted, MachineBroken, …)
- **Inventory** — current item/product counts

### Other build targets

| Target                       | What it builds                                 |
|------------------------------|------------------------------------------------|
| `gactorio`                   | GUI executable (default)                       |
| `gactorio_backend`           | Backend static library (no UI deps)            |
| `gactorio_backend_example`   | CLI sanity check that ticks the simulator      |
| `*_smoke_test`               | Eleven backend unit tests (run with `ctest`)   |

Disable optional builds:

```powershell
cmake -S . -B build -DGACTORIO_BUILD_GUI=OFF -DGACTORIO_BUILD_TESTS=OFF
```

Run backend tests:

```powershell
ctest --test-dir build --output-on-failure
```

## Project Layout

```
Gactorio/
├── include/              # Backend public headers
│   ├── common/           # SimClock, ids, enums
│   ├── model/            # Machine, MachineState, Factory, Item, Product, …
│   ├── model/events/     # Observer pattern: EventBus, EventLogObserver, …
│   ├── dto/              # Read-only Snapshot value objects
│   └── controller/       # FactoryController public API
├── src/
│   ├── common/, model/, dto/, controller/   # Backend impl
│   ├── views/                               # ImGui front-end (View layer)
│   │   ├── Application.{h,cpp}              # GLFW + OpenGL + ImGui bootstrap
│   │   ├── AppUI.{h,cpp}                    # Panel compositor
│   │   ├── UIComponent.h                    # Abstract base for panels
│   │   └── FactoryPanel.{h,cpp}             # Renders FactorySnapshot
│   ├── main.cpp                             # GUI entry point
│   └── backend_example.cpp                  # CLI sanity check (no GUI)
├── tests/                                   # Backend smoke tests
├── data/factory_config.json                 # Recipe/machine catalog (planned)
├── architecture.md                          # Backend architecture spec
└── CMakeLists.txt
```

## Design Patterns

| Pattern   | Where it lives                                                        |
|-----------|-----------------------------------------------------------------------|
| State     | `MachineState` + concrete `IdleState`/`WorkingState`/`BrokenState`/`MaintenanceState`. `Machine::update()` delegates to the current state — no `if`/`switch` on enum |
| Observer  | `Observer` + `EventBus` publishing `Event` to `EventLogObserver` and `StatisticsObserver` subscribers |
| MVC       | Backend = Model + Controller; ImGui front-end = View. View reads `FactorySnapshot` only, sends commands through `FactoryController` |
| Polymorphism (Factory updates) | `Factory::update()` iterates `Machine*` base pointers. Adding a new `Machine` subclass requires no changes to the update loop |
| Inheritance (Factory specialization) | `CarbonationFactory : public Factory` configures domain-specific machines and recipes |

## Controller API Cheat Sheet

```cpp
gactorio::FactoryController controller;     // builds default factory

// Commands (mutations)
controller.tick(deltaTime);
controller.pauseSimulation();
controller.resumeSimulation();
controller.resetSimulation();
controller.setSimulationSpeed(2.0);
controller.enqueueProduct(lineId, gactorio::ProductType::ToyCar);
controller.forceBreak(machineId);
controller.repairMachine(machineId);

// Queries (read-only DTO)
gactorio::FactorySnapshot snap = controller.snapshot();
for (const auto& line : snap.productionLines()) { ... }
for (const auto& event : snap.events())          { ... }
```

The View **never** holds raw `Machine*`, `Factory*`, or `Inventory*`. All
reads go through the snapshot, all writes go through controller commands.

## Adding a New Machine

1. Derive a class from `Machine`.
2. Override `typeName()`, `processType()`, `role()`, `canAcceptRecipe()`.
3. Construct it inside `CarbonationFactory` (or another `Factory` subclass).

No changes required in `Factory::update()`, `ProductionLine::update()`, or
the View — they all operate through base pointers and snapshots.

## Team

| Member  | Student ID | Responsibility |
|---------|-----------|----------------|
| 고원규  | 20265010  | FE, BE, PM     |
| 조용빈  | 20265260  | BE             |
