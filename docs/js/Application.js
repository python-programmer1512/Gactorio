// =============================================================================
// Application — owns the main loop.
//
// Mirrors src/views/Application.{h,cpp} from the ImGui era: the GLFW window
// + vsync loop becomes requestAnimationFrame, and "ui.renderAll() once per
// frame" becomes a 10 Hz throttled DOM rebuild (rebuilding innerHTML at full
// 60 Hz destroys buttons mid-click — see FactoryPanel for the pointerdown
// counterpart of this fix).
//
// The View never touches the Model: every command and query goes through the
// controller proxy (Module.Controller ⇒ ctrl::Controller ⇒ gactorio::*).
// =============================================================================

import { AppUI } from './AppUI.js';
import { toPlainSnapshot } from './util.js';

export class Application {
    static RENDER_INTERVAL_MS = 100;   // DOM refresh rate: 10 Hz

    #ctrl;
    #ui = new AppUI();
    #lastTime = 0;
    #lastRenderMs = 0;
    #tickCount = 0;

    constructor(controller) {
        this.#ctrl = controller;
    }

    addPanel(panel) {
        this.#ui.addPanel(panel);
    }

    renderNow() {
        const snapshot = toPlainSnapshot(this.#ctrl.snapshot());
        snapshot.tickCount = this.#tickCount;
        this.#ui.renderAll(snapshot);
        this.#lastRenderMs = performance.now();
    }

    run() {
        this.#lastTime = performance.now();
        this.renderNow();
        requestAnimationFrame(this.#frame);
    }

    #frame = (now) => {
        const dt = (now - this.#lastTime) / 1000;
        this.#lastTime = now;

        // Simulation always advances at full vsync rate; the C++ SimClock
        // internally honours pause, so ticking while paused is a no-op.
        this.#ctrl.tick(dt);
        this.#tickCount += 1;

        if (now - this.#lastRenderMs >= Application.RENDER_INTERVAL_MS) {
            const snapshot = toPlainSnapshot(this.#ctrl.snapshot());
            snapshot.tickCount = this.#tickCount;
            this.#ui.renderAll(snapshot);
            this.#lastRenderMs = now;
        }

        requestAnimationFrame(this.#frame);
    };
}
