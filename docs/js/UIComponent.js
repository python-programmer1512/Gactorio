// =============================================================================
// UIComponent — 모든 패널의 추상 기반 클래스 (JS View 계층)
// -----------------------------------------------------------------------------
// 수업 자료의 ImGui 시절 C++ View 설계(src/views/UIComponent.h)를 그대로 옮긴 것:
//
//     class UIComponent {
//         virtual ~UIComponent() = default;
//         virtual void render() = 0;   // 매 프레임 그리기
//     };
//
// JS 패널은 여기에 더해 일회성 bind() 훅을 갖는다. ImGui 는 "선언과 그리기"가
// 한 프레임에 같이 일어나지만, DOM 은 "이벤트 핸들러 등록(bind)"과 "그리기(render)"가
// 분리되기 때문이다.
//
// MVC 관점: 이 계층 전체가 View 다. View 는 C++ Model 을 절대 직접 모른다 —
// 오직 Controller 프록시(Module.Controller)만 호출한다.
// =============================================================================

export class UIComponent {
    // AppUI 에 등록될 때 정확히 한 번 호출된다. DOM 이벤트 핸들러를 여기서 연결한다.
    // 기본 구현: 연결할 것이 없으면 그대로 둠.
    bind() {}

    // 매 렌더 틱마다 plain-data 스냅샷(util.toPlainSnapshot 결과)을 받아 화면을 그린다.
    // 반드시 하위 클래스에서 override 해야 한다(추상 메서드 흉내).
    render(_snapshot) {
        throw new Error(`${this.constructor.name} must implement render()`);
    }
}
