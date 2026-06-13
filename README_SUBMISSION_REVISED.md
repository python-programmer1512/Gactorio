# Gactorio

Gactorio is an energy drink factory simulator for the OOP with C++ course at GIST EECS.

The simulation backend is written in C++17 and compiled to WebAssembly for the browser. The frontend View is plain HTML/CSS/JavaScript under `docs/`, so the project can be hosted directly with GitHub Pages.

## Architecture

- Model: `include/model`, `src/model`
- Controller: `include/controller`, `src/controller`
- DTO/Snapshot: `include/dto`, `src/dto`
- Web View: `docs/index.html`, `docs/style.css`, `docs/js`
- Emscripten bridge: `src/web/bindings.cpp`

The View never accesses `gactorio::*` Model classes directly. It talks only to `Module.Controller`, which is the Emscripten-exposed facade for `ctrl::Controller`.

```text
JS View panels
  -> Module.Controller
    -> ctrl::Controller
      -> gactorio::FactoryController
        -> gactorio::* Model

Model state
  -> DTO / FactoryView snapshot
    -> JS View rendering
```

## Build And Run

### Native backend tests

```powershell
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

### WebAssembly build

Prerequisite: Emscripten SDK must be installed and activated.

Windows:

```powershell
.\build_web.ps1
```

macOS/Linux:

```bash
./build_web.sh
```

The scripts generate:

- `docs/gactorio.js`
- `docs/gactorio.wasm`

### Local browser run

```powershell
python -m http.server 8080 --directory docs
```

Open:

```text
http://127.0.0.1:8080/
```

## GitHub Pages

Use the repository Pages setting:

- Source: `main`
- Folder: `/docs`

Expected URL:

```text
https://python-programmer1512.github.io/Gactorio/
```

## Design Patterns

- MVC: `gactorio::*` Model, `ctrl::Controller` Controller, `docs/js` View
- State: `MachineState`, `IdleState`, `WorkingState`, `BrokenState`, `MaintenanceState`
- Observer: `EventBus`, `EventLogObserver`, `StatisticsObserver`
- Memento: `FactoryMemento`, `SimulationHistory`, checkpoint/undo
- Polymorphism: production update loop calls `Machine::update()` through base-class pointers

## Presentation Assets

- Backend UML: `presentation/UML_raw`
- Revised UML: `presentation/UML_raw/*_REVISED.drawio`
- FE JS View UML: `presentation/UML_raw/FE_JS_View_Class_Diagram.drawio`
- Scripts: `presentation/Script`
