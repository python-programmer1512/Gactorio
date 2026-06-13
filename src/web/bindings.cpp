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
        .field("scenarioId",          &LineView::scenarioId)
        .field("scenarioName",        &LineView::scenarioName)
        .field("queueCapacity",       &LineView::queueCapacity)
        .field("droppedTaskCount",    &LineView::droppedTaskCount)
        .field("isRemovable",         &LineView::isRemovable)
        .field("machines",            &LineView::machines);
    register_vector<LineView>("VectorLineView");

    value_object<EventView>("EventView")
        .field("time",     &EventView::time)
        .field("typeName", &EventView::typeName)
        .field("message",  &EventView::message);
    register_vector<EventView>("VectorEventView");

    value_object<InventoryEntry>("InventoryEntry")
        .field("id",        &InventoryEntry::id)
        .field("legacyId",  &InventoryEntry::legacyId)
        .field("name",      &InventoryEntry::name)
        .field("displayName", &InventoryEntry::displayName)
        .field("kind",      &InventoryEntry::kind)
        .field("quantity",  &InventoryEntry::quantity)
        .field("isProduct", &InventoryEntry::isProduct)
        .field("restockable", &InventoryEntry::restockable)
        .field("restockAmount", &InventoryEntry::restockAmount);
    register_vector<InventoryEntry>("VectorInventoryEntry");

    value_object<ProductOption>("ProductOption")
        .field("id",              &ProductOption::id)
        .field("legacyId",        &ProductOption::legacyId)
        .field("key",             &ProductOption::key)
        .field("name",            &ProductOption::name)
        .field("displayName",     &ProductOption::displayName)
        .field("defaultRecipeId", &ProductOption::defaultRecipeId)
        .field("tier",            &ProductOption::tier)
        .field("color",           &ProductOption::color)
        .field("durationSeconds", &ProductOption::durationSeconds)
        .field("requirements",    &ProductOption::requirements);
    register_vector<ProductOption>("VectorProductOption");

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
        .function("enqueueProduct", &Controller::enqueueProduct)
        .function("enqueueProductById", &Controller::enqueueProductById)
        .function("enqueueAuto",  &Controller::enqueueAuto)
        .function("enqueueAutoProduct", &Controller::enqueueAutoProduct)
        .function("enqueueAutoProductById", &Controller::enqueueAutoProductById)
        .function("addLine",      &Controller::addLine)
        .function("removeLine",   &Controller::removeLine)
        .function("breakMachine", &Controller::breakMachine)
        .function("repair",         &Controller::repair)
        .function("instantRepair",  &Controller::instantRepair)
        .function("restockItem",    &Controller::restockItem)
        .function("restockItemById", &Controller::restockItemById)
        .function("repairAll",      &Controller::repairAll)
        .function("setLineScenario", &Controller::setLineScenario)
        .function("loadFactoryConfigFromString", &Controller::loadFactoryConfigFromString)
        .function("clearEventLog",  &Controller::clearEventLog)
        .function("getLineScenario", &Controller::getLineScenario)
        .function("saveCheckpoint", &Controller::saveCheckpoint)
        .function("undo",           &Controller::undo)
        .function("canUndo",        &Controller::canUndo)
        .function("historySize",    &Controller::historySize)
        .function("products",       &Controller::products)
        .function("snapshot",       &Controller::snapshot);
}
