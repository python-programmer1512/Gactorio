# Gactorio

**Gactorio = GIST + Factorio.** Energy Drink Factory Simulator.

A C++17 OOP project for the OOP with C++ course (GIST EECS, Spring 2026).
The backend (Model + Controller) is C++ compiled to WebAssembly; the View is
plain HTML/CSS/JS under `docs/`. The whole thing runs in the browser — no server
required — and is hosted directly from the `docs/` folder on GitHub Pages.

```
   ┌───────────────────────────┐         ┌───────────────────────────┐
   │  C++ Model  (gactorio::)  │         │  View  (docs/*.html, js/) │
   │  - Factory, Machine, …    │         │  - DOM render             │
   │  - State / Observer /     │         │  - button handlers        │
   │    Memento                │         │                           │
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
│   ├── common/, model/, dto/    ← internal Model headers (gactorio:: namespace)
│   └── controller/
│       ├── FactoryController.hpp    Internal BE service (gactorio::)
│       └── Controller.h             ★ View-facing API   (ctrl::)
├── src/
│   ├── common/, model/, dto/    ← Model impl
│   ├── controller/
│   │   ├── FactoryController.cpp
│   │   ├── SimulationHistory.cpp    ← Memento caretaker
│   │   └── Controller.cpp           ← translates between gactorio:: and ctrl::
│   ├── web/bindings.cpp             ← Emscripten/embind glue
│   └── backend_example.cpp          ← native CLI sanity check
├── docs/                            ← the GitHub Pages site (View + build output)
│   ├── index.html                   ← View: page structure
│   ├── style.css                    ← View: styling
│   ├── js/
│   │   ├── main.js                  ← bootstrap: creates Module.Controller, panels
│   │   ├── Application.js            ← main loop (tick + 10 Hz render)
│   │   ├── AppUI.js                 ← panel compositor
│   │   ├── UIComponent.js           ← abstract panel base
│   │   ├── util.js                  ← esc / embind-vector helpers
│   │   └── panels/                  ← SimControl, Factory, Products, Inventory, EventLog
│   ├── gactorio.js   (generated)    ← Emscripten glue
│   └── gactorio.wasm (generated)    ← compiled C++ binary
├── tests/                           ← 12 backend smoke tests
├── data/factory_config.json         ← tuning values → include/common/Config.h
├── tools/gen_config.py              ← regenerates Config.h from the JSON
├── CMakeLists.txt                   ← native build + tests
├── build_web.ps1 / build_web.sh     ← one-shot wasm build script (outputs to docs/)
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

Generates `docs/gactorio.js` + `docs/gactorio.wasm` (in place — no copy step needed).

Serve `docs/` over any static HTTP server (browsers refuse `.wasm` via `file://`):
```powershell
python -m http.server 8080 --directory docs
# open http://localhost:8080
```

### Native (BE library + smoke tests, no GUI)

```powershell
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
.\build\gactorio_backend_example.exe
```

Tools: MSYS2 UCRT64 with `gcc`, `cmake`, `ninja`.

## How Model / View / Controller stay decoupled

- **View** (`docs/js/*`) only knows `Module.Controller`. It never sees a single
  `gactorio::` symbol — those types are completely hidden behind PImpl.
- **Model** (`gactorio::*`) only knows itself. It does not depend on the View
  in any way. The compiler enforces this — the backend does not link against the
  browser, ImGui, GLFW, or anything UI-related.
- **Controller** is the only place that translates between the two worlds:
  - Commands come in as plain JS calls (`controller.tick(0.016)`,
    `controller.enqueueAutoProduct(101)`) and get forwarded to
    `gactorio::FactoryController` inside `Controller.cpp`.
  - Queries come out via `controller.snapshot()` which returns a
    `ctrl::FactoryView` — a plain-data DTO of strings and numbers, no
    `gactorio::` types leaking.
  - `src/web/bindings.cpp` exposes `ctrl::Controller` to JS as `Module.Controller`
    via Emscripten embind.

## Design Patterns

| Pattern   | Where |
|-----------|-------|
| MVC       | `gactorio::*` Model · `ctrl::Controller` Controller · `docs/js` View |
| State     | `MachineState` + `IdleState` / `WorkingState` / `BrokenState` / `MaintenanceState` |
| Observer  | `EventBus` publishing to `EventLogObserver` and `StatisticsObserver` |
| Memento   | `FactoryMemento` (Originator: `Factory`) + `SimulationHistory` (Caretaker) — checkpoint / undo |
| Polymorphism | `Factory::update()` iterates `Machine*` base pointers — no type branching |
| Inheritance | `CarbonationFactory : public Factory` for project-specific setup |

## Deploying to GitHub Pages

The build writes directly into `docs/`, which already is a complete static site.

1. Build with `build_web.ps1` (or `build_web.sh`).
2. Commit & push `docs/`.
3. Repo → Settings → Pages → Source: `main` branch, folder `/docs`.
4. Visit `https://python-programmer1512.github.io/Gactorio/`.

## Presentation Assets

- Class / pattern UML (drawio, importable): `presentation/UML_raw/*.drawio`
- FE JS View UML: `presentation/UML_raw/FE_JS_View_Class_Diagram.drawio`
- UML ↔ code consistency analysis: `presentation/UML/UML_Code_Consistency_Analysis_KR.md`
- Presentation scripts: `presentation/Script/` (demo-flow + diagram walkthroughs)

## Team

| Member  | Student ID | Responsibility |
|---------|-----------|----------------|
| 고원규  | 20265010  | FE, BE, PM     |
| 조용빈  | 20265260  | BE             |
