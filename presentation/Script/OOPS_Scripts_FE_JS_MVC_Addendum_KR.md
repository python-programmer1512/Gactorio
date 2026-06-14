# FE / JS MVC 보완 발표 대본

이제 Backend UML에 이어, 실제 화면을 담당하는 JavaScript View 구조를 설명하겠습니다.

이 프로젝트는 C++ Backend를 WebAssembly로 빌드하고, HTML/CSS/JavaScript를 View 계층으로 사용합니다. 원래 과제에서 설명한 MVC 원칙은 그대로 유지했습니다. Model은 `gactorio::*` C++ 클래스들이고, Controller 경계는 `ctrl::Controller`입니다. 이 `ctrl::Controller`가 Emscripten embind를 통해 JavaScript에서는 `Module.Controller`라는 객체로 노출됩니다.

가장 중요한 점은 View가 Model을 직접 알지 않는다는 것입니다. JavaScript 파일 어디에서도 `Factory`, `Machine`, `ProductionLine`, `Inventory` 같은 Model 객체를 직접 다루지 않습니다. View는 오직 `Module.Controller`에 command를 보내고, `snapshot()`으로 받은 plain data만 읽어서 화면을 그립니다.

---

## 1. C++ Backend와 JavaScript View의 연결

C++ 쪽 연결 지점은 `src/web/bindings.cpp`입니다. 여기서 `EMSCRIPTEN_BINDINGS`를 통해 `ctrl::Controller`와 View용 구조체들이 JavaScript에 공개됩니다.

JavaScript에서는 다음 흐름으로 사용합니다.

```text
new Module.Controller()
  -> ctrl::Controller
    -> gactorio::FactoryController
      -> gactorio::Factory / Model
```

명령 방향은 View에서 Controller로 내려갑니다. 예를 들어 pause, resume, reset, enqueue, repair, checkpoint, undo 같은 버튼 입력은 모두 `Module.Controller`의 메서드 호출로 전달됩니다.

조회 방향은 반대입니다. JavaScript가 `controller.snapshot()`을 호출하면 C++ Controller가 현재 상태를 View 전용 데이터로 복사해서 돌려줍니다. 이 데이터는 Model 객체 자체가 아니라 `FactoryView`, `LineView`, `MachineView` 같은 값 기반 View 구조입니다.

즉, View는 C++ 객체의 내부 포인터나 참조를 잡지 않고, 매번 복사된 snapshot을 기준으로 화면을 다시 그립니다.

---

## 2. JavaScript View 클래스 구조

JavaScript View의 최상위에는 `Application`이 있습니다.

`Application`은 main loop를 담당합니다. 브라우저의 `requestAnimationFrame`으로 매 frame마다 `controller.tick(dt)`을 호출하고, 일정 간격마다 `controller.snapshot()`을 가져와 화면을 갱신합니다.

그 아래에는 `AppUI`가 있습니다. `AppUI`는 panel compositor 역할을 합니다. 내부에 `UIComponent` 목록을 가지고 있고, `renderAll(snapshot)`을 호출하면 등록된 모든 panel의 `render(snapshot)`이 실행됩니다.

`UIComponent`는 모든 panel의 추상 기반 클래스 역할을 합니다. JavaScript 문법상 C++ abstract class와 완전히 같지는 않지만, 기본 `render()`에서 error를 던지도록 만들어 하위 panel이 반드시 `render()`를 구현하게 했습니다. 또한 DOM 이벤트 연결을 위해 `bind()` hook도 제공합니다.

현재 등록되는 panel은 다음과 같습니다.

- `SimControlPanel`: pause, resume, reset, speed, checkpoint, undo
- `RuntimeConfigPanel`: runtime JSON config 로딩
- `FactoryPanel`: 생산라인과 기계 카드 렌더링
- `InspectorPanel`: 선택한 기계의 상세 상태 표시와 조작
- `ProductsPanel`: 제품 목록과 생산 요청 버튼
- `EventLogPanel`: Observer로 수집된 이벤트 로그 표시
- `InventoryPanel`: 원재료와 완제품 재고 표시, restock 명령

이 구조는 수업 자료의 composable UI 구조와 유사합니다. 새로운 화면 영역이 필요하면 `UIComponent`를 상속한 class를 만들고, `main.js`에서 `app.addPanel()`로 등록하면 됩니다. 기존 `AppUI`나 다른 panel을 크게 수정하지 않아도 되므로 Open-Closed Principle에 잘 맞습니다.

---

## 3. 데이터 흐름

View와 Backend 사이의 데이터 흐름은 command와 snapshot으로 나누어집니다.

사용자 입력은 panel에서 발생합니다. 예를 들어 제품 버튼을 누르면 `ProductsPanel`이 `controller.enqueueAutoProductById()` 같은 command를 호출합니다. 수리 버튼은 `InspectorPanel`이나 관련 panel에서 repair command로 전달되고, checkpoint와 undo는 `SimControlPanel`에서 Controller 메서드로 전달됩니다.

반대로 Backend 상태는 snapshot으로 올라옵니다. `Application`은 `controller.snapshot()`을 호출하고, 그 결과를 `util.toPlainSnapshot()`으로 plain JavaScript object로 변환합니다. 이후 `AppUI.renderAll(snapshot)`이 모든 panel에 같은 snapshot을 전달합니다.

이 방식의 장점은 화면 구성 요소들이 Model을 직접 조회하지 않는다는 점입니다. 각 panel은 자신에게 전달된 snapshot과 Controller command만 사용합니다.

---

## 4. Embind 메모리 처리

WebAssembly와 JavaScript 사이에는 한 가지 주의할 점이 있습니다. embind가 반환하는 vector나 객체 핸들은 JavaScript GC가 자동으로 정리하지 않습니다.

그래서 `docs/js/util.js`의 `vecToArray()`가 embind vector를 일반 JS 배열로 복사한 뒤, 반드시 `delete()`를 호출해 C++ 쪽 할당을 해제합니다.

`toPlainSnapshot()`도 같은 역할을 합니다. C++에서 받은 snapshot을 화면에서 쓰기 쉬운 plain data로 평탄화하고, 중첩 vector도 배열로 변환합니다.

이 처리를 panel마다 흩어 놓지 않고 `util.js`에 모아두었기 때문에, 각 panel 코드는 메모리 관리보다 화면 렌더링 책임에 집중할 수 있습니다.

---

## 5. Layout 변경과 UML 관계

현재 화면에서는 `InspectorPanel`을 독립된 기계 상세 영역으로 두고, `ProductsPanel`과 `InventoryPanel`은 catalog 영역에 배치했습니다. 또한 `RuntimeConfigPanel`을 통해 JSON config를 runtime에 불러올 수 있습니다.

이런 배치는 DOM/CSS 레이아웃 변경입니다. 클래스 관계 자체는 그대로 유지됩니다. 모든 panel은 여전히 `UIComponent`를 상속하고, `AppUI`가 panel 목록을 소유하며, `Application`이 Controller와 AppUI를 연결합니다.

따라서 FE class UML에서 핵심 화살표는 다음과 같이 유지됩니다.

```text
Application
  -> owns AppUI
  -> uses Module.Controller

AppUI
  -> owns UIComponent[]

SimControlPanel / RuntimeConfigPanel / FactoryPanel / InspectorPanel /
ProductsPanel / EventLogPanel / InventoryPanel
  -> inherit UIComponent
  -> use Module.Controller commands or snapshot data
```

---

## 6. MVC 관점 정리

MVC 관점에서 정리하면 다음과 같습니다.

Model은 C++ Backend의 `gactorio::*` 클래스입니다. `Factory`, `ProductionLine`, `Machine`, `Inventory`, `Product`, `EventBus`, Memento 계층이 여기에 해당합니다.

Controller는 JavaScript에 노출된 `Module.Controller`입니다. 실제 C++에서는 `ctrl::Controller`가 web-facing facade 역할을 하고, 내부적으로 `gactorio::FactoryController`에 위임합니다.

View는 `docs/js` 아래의 `Application`, `AppUI`, `UIComponent`, 그리고 여러 panel 클래스입니다.

이 구조에서 View는 Model에 직접 접근하지 않고, Controller를 통해서만 명령과 조회를 수행합니다. Model 상태는 snapshot으로 복사되어 View에 전달되고, View에서 발생한 입력은 command로 Controller에 전달됩니다.

결론적으로 JavaScript View는 Dear ImGui 대신 DOM을 사용하지만, MVC 구조의 핵심인 Model과 View의 분리, Controller를 통한 단일 통신 경계, composable UI component 구조를 유지하고 있습니다.

추가로 Memento UML은 공개 field가 아니라 private field와 accessor로 표현되어 있습니다. 이는 실제 `FactoryMemento.hpp` 구현과 일치하며, public data member를 피하고 encapsulation을 강화하기 위한 설계입니다.
