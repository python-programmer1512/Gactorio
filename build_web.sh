#!/usr/bin/env bash
# Build the C++ backend + embind layer to docs/gactorio.{js,wasm} via Emscripten.
# Output lands in docs/ because GitHub Pages serves that folder.
# Prereq: Emscripten SDK installed and `emcc --version` works.

set -e

# 1) Regenerate include/common/Config.h from data/factory_config.json so any
#    hyperparameter edits in the JSON propagate to the C++ build.
python tools/gen_config.py

SOURCES=(
    src/common/Types.cpp
    src/common/SimClock.cpp
    src/model/Item.cpp
    src/model/Product.cpp
    src/model/ProductCatalog.cpp
    src/model/ProductionTask.cpp
    src/model/Recipe.cpp
    src/model/Inventory.cpp
    src/model/MachineState.cpp
    src/model/MachineStates.cpp
    src/model/Machine.cpp
    src/model/ProductionLine.cpp
    src/model/Factory.cpp
    src/model/CarbonationFactory.cpp
    src/model/events/EventBus.cpp
    src/model/events/EventLogObserver.cpp
    src/model/events/StatisticsObserver.cpp
    src/dto/InventorySnapshot.cpp
    src/dto/MachineSnapshot.cpp
    src/dto/ProductionLineSnapshot.cpp
    src/dto/EventSnapshot.cpp
    src/dto/StatisticsSnapshot.cpp
    src/dto/FactorySnapshot.cpp
    src/controller/SimulationHistory.cpp
    src/controller/FactoryController.cpp
    src/controller/Controller.cpp
    src/web/bindings.cpp
)

emcc "${SOURCES[@]}" \
    -I include \
    -std=c++17 \
    -O2 \
    --bind \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s EXPORT_NAME=Module \
    -o docs/gactorio.js

echo "OK -> docs/gactorio.js + docs/gactorio.wasm"
