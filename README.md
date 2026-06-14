# Gactorio

**Gactorio = GIST + Factorio.** Energy Drink Factory Simulator.

A C++17 OOP project for the *OOP with C++* course (GIST EECS, Spring 2026).
The backend (Model + Controller) is C++ compiled to WebAssembly; the View is
plain HTML/CSS/JS (ES modules). The whole thing runs in the browser — no server
required — and is hosted on GitHub Pages.

```
   ┌───────────────────────────┐         ┌───────────────────────────┐
   │  C++ Model  (gactorio::)  │         │  View  (docs/*.html, js/) │
   │  - Factory, Machine, …    │         │  - DOM render             │
   │  - State / Observer / …   │         │  - button handlers        │
   └─────────────┬─────────────┘         └─────────────┬─────────────┘
                 ▲                                     │
                 │ (only the Controller layer touches  │
                 │  both Model and View)               │
                 ▼                                     ▼
                 ┌─────────────────────────────────────┐
                 │ ctrl::Controller  +  embind binding │
                 │ → JS sees Module.Controller         │
                 └─────────────────────────────────────┘
```

## Layout

```
Gactorio/
├── include/
│   ├── common/        Types, SimClock, ScenarioType, Config.h   (gactorio::)
│   ├── model/         Factory, Machine, MachineState(s), Product, …
│   │   ├── config/    JSON-driven config subsystem  (gactorio::config_model::)
│   │   ├── events/    EventBus + Observer interfaces (Observer pattern)
│   │   └── memento/   FactoryMemento / LineMemento / MachineMemento
│   ├── dto/           FactorySnapshot & friends (Model → Controller transport)
│   └── controller/
│       ├── FactoryController.hpp   Internal BE service   (gactorio::)
│       └── Controller.h            ★ View-facing API     (ctrl::)
├── src/                ← mirrors include/ (implementation)
│   ├── web/bindings.cpp            ← Emscripten/embind glue (Module.Controller)
│   └── backend_example.cpp         ← native CLI sanity check
├── docs/               ★ the View AND the deployed GitHub Pages site
│   ├── index.html                  ← page structure
│   ├── style.css                   ← styling
│   ├── js/
│   │   ├── main.js                 ← bootstrap; builds Controller + panels
│   │   ├── Application.js          ← main loop (requestAnimationFrame)
│   │   ├── AppUI.js                ← panel compositor
│   │   ├── UIComponent.js          ← abstract panel base class
│   │   ├── util.js                 ← embind vector helpers, esc()
│   │   └── panels/                 ← SimControl, Factory, Inspector,
│   │                                  Products, Inventory, EventLog, RuntimeConfig
│   ├── config/                     ← runtime JSON copied here by build_web
│   ├── gactorio.js   (generated)   ← Emscripten glue
│   └── gactorio.wasm (generated)   ← compiled C++ binary
├── tests/              ← 33 backend smoke tests (model + controller)
├── data/
│   ├── factory_config.json             source config (Config.h is generated from it)
│   ├── factory_config.runtime.json     runtime JSON loaded by the View
│   └── factory_config.custom_runtime.json   alternate config for the demo
├── presentation/       ← UML diagrams (drawio) + presentation scripts
├── CMakeLists.txt      ← native build (lib + smoke tests) + Emscripten branch
├── build_web.ps1 / build_web.sh   ← one-shot wasm build → docs/
└── architecture.md
```

## Build & Run

### Web (the real deal)

Prereqs:
1. Install [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).
2. Activate for the current shell:
   ```powershell
   C:\emsdk\emsdk_env.ps1     # or:  source ~/emsdk/emsdk_env.sh
   emcc --version
   ```

Build:
```powershell
./build_web.ps1     # Windows
./build_web.sh      # macOS / Linux
```

Output lands **directly in `docs/`** (`gactorio.js` + `gactorio.wasm`, plus the
runtime JSON under `docs/config/`).

Serve `docs/` over any static HTTP server (browsers refuse `.wasm` via `file://`):
```powershell
cd docs
python -m http.server 8080
# open http://localhost:8080
```

### Native (BE library + smoke tests, no GUI)

```powershell
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure   # 33/33 pass
.\build\gactorio_backend_example.exe
```

Tools: MSYS2 UCRT64 with `g++` (16.x, C++17), `cmake`, `ninja`.

## How Model / View / Controller stay decoupled

- **View** (`docs/*`) only knows `Module.Controller`. It never sees a single
  `gactorio::` symbol — those types are completely hidden behind PImpl.
- **Model** (`gactorio::*`) only knows itself. It does not depend on the View
  in any way. The compiler enforces this — the `gactorio_backend` library does
  not link against the browser, ImGui, GLFW, or anything UI-related.
- **Controller** is the only place that translates between the two worlds:
  - Commands come in as plain JS calls (`controller.tick(0.016)`,
    `controller.enqueueAutoProductById("voltz_classic")`) and get forwarded to
    `gactorio::FactoryController` inside `Controller.cpp`.
  - Queries come out via `controller.snapshot()` which returns a
    `ctrl::FactoryView` — a plain-data DTO of strings and numbers, no
    `gactorio::` types leaking.

## Design Patterns

| Pattern        | Where |
|----------------|-------|
| State          | `MachineState` + `IdleState` / `WorkingState` / `BrokenState` / `MaintenanceState` drive each `Machine` |
| Observer       | `EventBus` publishes events to `EventLogObserver` and `StatisticsObserver` |
| Memento        | `Factory` (Originator) ↔ `FactoryMemento` / `LineMemento` / `MachineMemento`; `SimulationHistory` is the Caretaker behind Save Checkpoint / Undo |
| Factory Method | `StationFactory::create()` builds the right `Machine` subclass from a `StationDefinition` |
| Builder        | `FactoryBuilder` assembles a configured `Factory` from runtime JSON |
| Polymorphism   | `Factory::update()` iterates `Machine*` base pointers; each station overrides `update`/`role`/`typeName` |
| Inheritance    | `CarbonationFactory : public Factory`; four concrete stations : `Machine` |
| MVC + PImpl    | `gactorio::*` Model · `ctrl::Controller` Controller (hides Model) · `docs/*` View |

UML diagrams (drawio + PNG) for every pattern live in `presentation/UML/` and
`presentation/UML_raw/`.

## Deploying to GitHub Pages

`build_web.ps1` writes straight into `docs/`, so the repo is deploy-ready:

1. Commit & push `docs/`.
2. Repo → Settings → Pages → Source: `main` branch, folder `/docs`.
3. Visit `https://python-programmer1512.github.io/Gactorio/`.

## Team

| Member  | Student ID | Responsibility |
|---------|-----------|----------------|
| 고원규  | 20265010  | FE, BE, PM     |
| 조용빈  | 20265260  | BE             |
