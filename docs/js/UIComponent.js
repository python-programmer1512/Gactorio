// =============================================================================
// UIComponent — abstract base for every panel.
//
// Mirrors the C++ View design from the ImGui era (src/views/UIComponent.h):
//
//     class UIComponent {
//         virtual ~UIComponent() = default;
//         virtual void render() = 0;
//     };
//
// JS panels additionally get a one-time bind() hook for DOM event wiring,
// since (unlike ImGui) the DOM separates "declare handlers" from "draw".
//
// UML: FE_JS_View_Class_Diagram — UIComponent is the abstract base; every
// panel is-a UIComponent (▷) and AppUI holds them (composition ◆).
// =============================================================================

export class UIComponent {
    // Called exactly once when the panel is registered with AppUI.
    // Wire DOM event handlers here. Default: nothing to bind.
    bind() {}

    // Called on every render tick with a plain-data snapshot
    // (see util.toPlainSnapshot). Must be overridden.
    render(_snapshot) {
        throw new Error(`${this.constructor.name} must implement render()`);
    }
}
