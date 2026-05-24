#pragma once

// 모든 UI 패널의 추상 base. 매 프레임 render()가 호출된다.
class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void render() = 0;
};
