# FE JS View UML 설명

## MVC 매핑

현재 FE는 `docs/js` 아래의 JavaScript View 계층입니다.

- Model: `gactorio::*` C++ backend classes
- Controller: `ctrl::Controller`, JavaScript에서는 `Module.Controller`
- View: `Application`, `AppUI`, `UIComponent`, 각 Panel class

View는 `gactorio::*` Model class를 직접 import하지 않습니다. 사용자 입력은 `Module.Controller` method 호출로만 전달되고, 화면 렌더링은 `snapshot()`에서 받은 plain data를 기반으로 수행됩니다.

## 주요 클래스

- `UIComponent`: 모든 panel의 abstract base 역할입니다. `bind()`와 `render(snapshot)` interface를 제공합니다.
- `AppUI`: `UIComponent` panel 목록을 보관하고 `renderAll(snapshot)`으로 전체 panel을 렌더링합니다.
- `Application`: animation frame loop를 담당합니다. 매 frame `ctrl.tick(dt)`를 호출하고, 10Hz로 `ctrl.snapshot()`을 plain snapshot으로 변환해 View에 전달합니다.
- `SimControlPanel`: pause/reset/speed/checkpoint/undo/statistics UI를 담당합니다.
- `FactoryPanel`: production line과 machine card/conveyor view를 담당합니다.
- `ProductsPanel`: product catalog를 controller에서 읽어 제품 enqueue button을 만듭니다.
- `InventoryPanel`: raw item restock button과 finished product table을 렌더링합니다.
- `EventLogPanel`: Observer event stream을 렌더링합니다.
- `util.js`: embind vector handle을 plain JS array로 변환하고 HTML escape를 제공합니다.

## UML 화살표 규칙

- 실선 + 빈 삼각형: inheritance/generalization입니다. 각 Panel은 `UIComponent`를 상속합니다.
- 채워진 다이아몬드: composition/ownership입니다. `Application`은 `AppUI`를 소유하고, `AppUI`는 등록된 `UIComponent` panel들을 구성 요소로 관리합니다.
- 실선 + 열린 화살표: association/call 관계입니다. `Application`은 `Module.Controller`에 `tick()`과 `snapshot()`을 호출합니다.
- 점선 + 열린 화살표: dependency입니다. Panel은 command 전송을 위해 `Module.Controller`에 의존하고, rendering을 위해 `FactoryView` snapshot에 의존합니다.
- DOM으로 향하는 점선: View rendering/event dependency입니다. Panel은 `innerHTML`과 DOM event listener를 사용하지만, 이 의존성은 View 내부에만 머뭅니다.

## 수업 자료와의 대응

수업 자료의 composable UI 구조는 `UIComponent -> AppUI -> Application` 형태로 반영되어 있습니다. ImGui 예제의 `MachineUI.render()`는 이 프로젝트에서 각 JS Panel의 `render(snapshot)`에 해당하고, ImGui button callback은 DOM `addEventListener()`와 `bind()`로 분리되었습니다.

따라서 View 구조 자체는 MVC 원칙에 맞습니다. 단, 과제 PDF가 Dear ImGui를 명시하고 있으므로, JavaScript View가 ImGui 대체 구현으로 인정되는지는 교수님 Q&A 시간에 확인하는 것이 안전합니다.
