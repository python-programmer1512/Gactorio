#pragma once

// Abstract base for every ImGui UI panel.
// render()  is called once per frame.
// update()  is called once per frame BEFORE render so panels can drive
//           simulation state, animations, etc. Default is no-op.
class UIComponent {
public:
    virtual ~UIComponent() = default;

    virtual void update(double /*deltaTime*/) {}
    virtual void render() = 0;
};
