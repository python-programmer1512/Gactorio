// =============================================================================
// main.js — View bootstrap.
//
// Mirrors the ImGui-era src/main.cpp line for line:
//
//     Application app("Gactorio", 1280, 720);
//     gactorio::FactoryController controller;
//     app.addPanel(std::make_unique<FactoryPanel>(controller));
//     app.run();
//
// `Module.Controller` is the embind proxy for ctrl::Controller — the single
// bridge between this View and the C++ Model. No panel ever bypasses it.
// =============================================================================

import { Application }     from './Application.js';
import { SimControlPanel } from './panels/SimControlPanel.js';
import { FactoryPanel }    from './panels/FactoryPanel.js';
import { ProductsPanel }   from './panels/ProductsPanel.js';
import { EventLogPanel }   from './panels/EventLogPanel.js';
import { InventoryPanel }  from './panels/InventoryPanel.js';

console.log('[gactorio] main.js loaded — build', '2026-06-01-k');

function boot() {
    console.log('[gactorio] wasm runtime ready, creating Controller');
    const controller = new Module.Controller();

    const app = new Application(controller);
    app.addPanel(new SimControlPanel(controller));
    app.addPanel(new FactoryPanel(controller));
    app.addPanel(new ProductsPanel(controller));
    app.addPanel(new EventLogPanel());
    app.addPanel(new InventoryPanel());
    app.run();
}

// Module scripts are deferred, so the wasm runtime may or may not be live by
// the time we execute — handle both orders (race-safe boot).
if (typeof Module !== 'undefined') {
    if (Module.calledRun) boot();
    else Module.onRuntimeInitialized = boot;
} else {
    console.error('[gactorio] Module is undefined — gactorio.js failed to load');
}
