// =============================================================================
// AppUI — panel compositor.
//
// Mirrors src/views/AppUI.h from the ImGui era: holds the panel list and
// fans render() out to every registered panel. Adding a new panel never
// requires modifying AppUI (Open-Closed Principle) — just addPanel().
//
// UML: FE_JS_View_Class_Diagram — AppUI ◆ owns a list of UIComponent and
// drives them polymorphically through render(snapshot).
// =============================================================================

import { UIComponent } from './UIComponent.js';

export class AppUI {
    #panels = [];

    addPanel(panel) {
        if (!(panel instanceof UIComponent)) {
            throw new TypeError('addPanel expects a UIComponent');
        }
        this.#panels.push(panel);
        panel.bind();
    }

    renderAll(snapshot) {
        for (const panel of this.#panels) panel.render(snapshot);
    }
}
