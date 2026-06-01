# Gactorio

**Gactorio = GIST + Factorio.** Energy Drink Factory Simulator.

A C++17 OOP project for the OOP with C++ course (GIST EECS, Spring 2026).
The backend (Model + Controller) is C++ compiled to WebAssembly; the View is
plain HTML/CSS/JS. The whole thing runs in the browser — no server required —
and is hosted on GitHub Pages.

```
   ┌───────────────────────────┐         ┌───────────────────────────┐
   │  C++ Model  (gactorio::)  │         │  View  (web/*.html,*.js)  │
   │  - Factory, Machine, …    │         │  - DOM render             │
   │  - State / Observer       │         │  - button handlers        │
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
│   │   └── Controller.cpp           ← translates between gactorio:: and ctrl::
│   ├── web/bindings.cpp             ← Emscripten/embind glue (~50 lines)
│   └── backend_example.cpp          ← native CLI sanity check
├── web/
│   ├── index.html                   ← View: page structure
│   ├── style.css                    ← View: styling
│   ├── app.js                       ← View: UI logic; talks to Module.Controller only
│   ├── gactorio.js   (generated)    ← Emscripten glue
│   └── gactorio.wasm (generated)    ← compiled C++ binary
├── tests/                           ← 11 backend smoke tests
├── data/factory_config.json
├── CMakeLists.txt                   ← native build + Emscripten branch
├── build_web.ps1 / build_web.sh     ← one-shot wasm build script
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

Generates `web/gactorio.js` + `web/gactorio.wasm`.

Serve `web/` over any static HTTP server (browsers refuse `.wasm` via `file://`):
```powershell
cd web
python -m http.server 8080
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

- **View** (`web/*`) only knows `Module.Controller`. It never sees a single
  `gactorio::` symbol — those types are completely hidden behind PImpl.
- **Model** (`gactorio::*`) only knows itself. It does not depend on the View
  in any way. The compiler enforces this — `gactorio_backend` library does
  not link against the browser, ImGui, GLFW, or anything UI-related.
- **Controller** is the only place that translates between the two worlds:
  - Commands come in as plain JS calls (`controller.tick(0.016)`,
    `controller.enqueue(1, Module.ProductKind.VoltzClassic)`) and get
    forwarded to `gactorio::FactoryController` inside `Controller.cpp`.
  - Queries come out via `controller.snapshot()` which returns a
    `ctrl::FactoryView` — a plain-data DTO of strings and numbers, no
    `gactorio::` types leaking.

## Design Patterns

| Pattern   | Where |
|-----------|-------|
| State     | `MachineState` + `IdleState` / `WorkingState` / `BrokenState` / `MaintenanceState` |
| Observer  | `EventBus` publishing to `EventLogObserver` and `StatisticsObserver` |
| MVC       | `gactorio::*` Model · `ctrl::Controller` Controller · `web/*` View |
| Polymorphism | `Factory::update()` iterates `Machine*` base pointers |
| Inheritance | `CarbonationFactory : public Factory` for project-specific setup |

## Deploying to GitHub Pages

After `build_web.ps1`, the `web/` folder is a complete static site.

Easiest path — **`docs/` folder**:
1. Copy `web/` → `docs/`.
2. Commit & push.
3. Repo → Settings → Pages → Source: `main` branch, folder `/docs`.
4. Visit `https://python-programmer1512.github.io/Gactorio/`.

For automated builds on every push, see `.github/workflows/deploy.yml`.

## Team

| Member  | Student ID | Responsibility |
|---------|-----------|----------------|
| 고원규  | 20265010  | FE, BE, PM     |
| 조용빈  | 20265260  | BE             |
