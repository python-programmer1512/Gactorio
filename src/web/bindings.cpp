// =============================================================================
// Emscripten bindings: expose ctrl::Controller to JavaScript.
//
// Build with `emcc` (see build_web.ps1 or build_web.sh). The result is a pair
// of files in web/:
//   - gactorio.js    (JS glue: loads .wasm, defines Module.Controller, ...)
//   - gactorio.wasm  (the compiled C++ binary)
//
// JS usage (see web/app.js):
//   const ctrl = new Module.Controller();
//   ctrl.tick(0.016);
//   const snap = ctrl.snapshot();
//   ctrl.enqueue(1, Module.ProductKind.VoltzClassic);
// =============================================================================

#include <emscripten/bind.h>

#include "controller/Controller.h"

using namespace emscripten;
using namespace ctrl;

EMSCRIPTEN_BINDINGS(gactorio_module) {
    // ---- Enum --------------------------------------------------------------
    enum_<ProductKind>("ProductKind")
        .value("Unknown",      ProductKind::Unknown)
        .value("VoltzClassic", ProductKind::VoltzClassic)
        .value("HyperBolt",    ProductKind::HyperBolt)
        .value("AuroraZero",   ProductKind::AuroraZero);

    // ---- Leaf structs ------------------------------------------------------
    value_object<MachineView>("MachineView")
        .field("id",       &MachineView::id)
        .field("name",     &MachineView::name)
        .field("type",     &MachineView::type)
        .field("state",    &MachineView::state)
        .field("progress", &MachineView::progress)
        .field("health",   &MachineView::health);
    register_vector<MachineView>("VectorMachineView");

    value_object<LineView>("LineView")
        .field("id",                  &LineView::id)
        .field("name",                &LineView::name)
        .field("queueLength",         &LineView::queueLength)
        .field("currentTaskName",     &LineView::currentTaskName)
        .field("currentTaskProgress", &LineView::currentTaskProgress)
        .field("machines",            &LineView::machines);
    register_vector<LineView>("VectorLineView");

    value_object<EventView>("EventView")
        .field("time",     &EventView::time)
        .field("typeName", &EventView::typeName)
        .field("message",  &EventView::message);
    register_vector<EventView>("VectorEventView");

    value_object<InventoryEntry>("InventoryEntry")
        .field("id",       &InventoryEntry::id)
        .field("quantity", &InventoryEntry::quantity);
    register_vector<InventoryEntry>("VectorInventoryEntry");

    value_object<Statistics>("Statistics")
        .field("tasksStarted",     &Statistics::tasksStarted)
        .field("stepsCompleted",   &Statistics::stepsCompleted)
        .field("productsDone",     &Statistics::productsDone)
        .field("machinesBroken",   &Statistics::machinesBroken)
        .field("machinesRepaired", &Statistics::machinesRepaired)
        .field("stateChanges",     &Statistics::stateChanges);

    value_object<FactoryView>("FactoryView")
        .field("simulationTime", &FactoryView::simulationTime)
        .field("stats",          &FactoryView::stats)
        .field("lines",          &FactoryView::lines)
        .field("events",         &FactoryView::events)
        .field("inventory",      &FactoryView::inventory);

    // ---- Controller --------------------------------------------------------
    // snapshot() returns a *copy* of the cached FactoryView to JS (embind
    // does the deep-copy via value_object). Cheap because the cache is reused
    // on the C++ side between ticks.
    class_<Controller>("Controller")
        .constructor<>()
        .function("tick",         &Controller::tick)
        .function("pause",        &Controller::pause)
        .function("resume",       &Controller::resume)
        .function("reset",        &Controller::reset)
        .function("setSpeed",     &Controller::setSpeed)
        .function("enqueue",      &Controller::enqueue)
        .function("breakMachine", &Controller::breakMachine)
        .function("repair",       &Controller::repair)
        .function("snapshot",     &Controller::snapshot);
}
