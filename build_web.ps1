# =============================================================================
# build_web.ps1 — compile the C++ backend + embind layer to docs/gactorio.{js,wasm}.
#
# Output lands directly in docs/ because GitHub Pages serves that folder.
#
# Prerequisites:
#   1. Install Emscripten SDK (https://emscripten.org/docs/getting_started/downloads.html).
#   2. Activate it for this shell:  C:\emsdk\emsdk_env.ps1
#   3. Confirm: `emcc --version`  prints an Emscripten version.
#
# Usage:
#   .\build_web.ps1
#
# Output:
#   docs/gactorio.js    (Emscripten glue)
#   docs/gactorio.wasm  (compiled C++ binary)
# =============================================================================
$ErrorActionPreference = 'Stop'

# 1) Regenerate include/common/Config.h from data/factory_config.json so any
#    hyperparameter edits in the JSON propagate to the C++ build.
python tools/gen_config.py

New-Item -ItemType Directory -Force docs/config | Out-Null
Copy-Item data/factory_config.runtime.json docs/config/factory_config.runtime.json -Force
Copy-Item data/factory_config.custom_runtime.json docs/config/factory_config.custom_runtime.json -Force

$sources = @(
    'src/common/Types.cpp',
    'src/common/SimClock.cpp',
    'src/common/ScenarioType.cpp',
    'src/model/config/ConfigIdAdapters.cpp',
    'src/model/Item.cpp',
    'src/model/Product.cpp',
    'src/model/ProductCatalog.cpp',
    'src/model/config/DefinitionRegistry.cpp',
    'src/model/config/FactoryConfigLoader.cpp',
    'src/model/config/FactoryRuntimeContext.cpp',
    'src/model/ProductionTask.cpp',
    'src/model/Recipe.cpp',
    'src/model/Inventory.cpp',
    'src/model/ConfiguredStation.cpp',
    'src/model/ConfigurableFactory.cpp',
    'src/model/FactoryBuilder.cpp',
    'src/model/StationFactory.cpp',
    'src/model/MachineState.cpp',
    'src/model/MachineStates.cpp',
    'src/model/Machine.cpp',
    'src/model/ProductionLine.cpp',
    'src/model/Factory.cpp',
    'src/model/CarbonationFactory.cpp',
    'src/model/events/EventBus.cpp',
    'src/model/events/EventLogObserver.cpp',
    'src/model/events/StatisticsObserver.cpp',
    'src/dto/InventorySnapshot.cpp',
    'src/dto/MachineSnapshot.cpp',
    'src/dto/ProductionLineSnapshot.cpp',
    'src/dto/EventSnapshot.cpp',
    'src/dto/StatisticsSnapshot.cpp',
    'src/dto/FactorySnapshot.cpp',
    'src/controller/SimulationHistory.cpp',
    'src/controller/ControllerConfigIdAdapters.cpp',
    'src/controller/FactoryController.cpp',
    'src/controller/Controller.cpp',
    'src/web/bindings.cpp'
)

emcc @sources `
    -I include `
    -std=c++17 `
    -O2 `
    --bind `
    -s ALLOW_MEMORY_GROWTH=1 `
    -s EXPORT_NAME=Module `
    -o docs/gactorio.js

Write-Host "OK  ->  docs/gactorio.js + docs/gactorio.wasm"
