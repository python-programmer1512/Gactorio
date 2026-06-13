# FE JS MVC 보완 발표 대본

이제 Backend UML에 이어, 실제 화면을 담당하는 JavaScript View 구조를 간단히 설명하겠습니다.

이 프로젝트는 C++ backend를 WebAssembly로 빌드하고, HTML/CSS/JavaScript를 View로 사용합니다. 과제 자료에서 설명한 MVC 원칙은 그대로 유지했습니다. Model은 `gactorio::*` C++ class들이고, Controller는 `ctrl::Controller`입니다. JavaScript에서는 Emscripten embind를 통해 이 Controller가 `Module.Controller`라는 객체로 노출됩니다.

중요한 점은 View가 Model을 직접 알지 않는다는 것입니다. JavaScript 파일 어디에서도 `Factory`, `Machine`, `ProductionLine` 같은 Model class를 직접 다루지 않습니다. View는 오직 `Module.Controller`에 command를 보내고, `snapshot()`으로 받은 plain data만 읽어서 화면을 그립니다.

View 구조는 수업 자료의 composable UI 구조와 유사합니다. 가장 위에는 `Application`이 있습니다. `Application`은 main loop를 담당하고, 매 frame마다 controller의 `tick()`을 호출합니다. 그리고 일정 간격으로 `snapshot()`을 가져와 `AppUI`에 전달합니다.

`AppUI`는 여러 panel을 관리하는 compositor입니다. 내부에 `UIComponent` 목록을 가지고 있고, `renderAll(snapshot)`을 호출하면 등록된 모든 panel의 `render()`가 실행됩니다.

각 화면 영역은 별도 panel class로 나뉘어 있습니다. `SimControlPanel`은 pause, reset, speed, checkpoint, undo를 담당합니다. `FactoryPanel`은 conveyor 형태의 production line과 machine card를 렌더링합니다. `ProductsPanel`은 제품 목록과 enqueue button을 담당합니다. `InventoryPanel`은 원재료와 완성품 재고를 보여주고, 원재료 restock command를 controller로 전달합니다. `EventLogPanel`은 Observer pattern으로 수집된 event log를 화면에 보여줍니다.

이 구조의 장점은 View를 확장하기 쉽다는 점입니다. 새로운 panel이 필요하면 `UIComponent`를 상속한 class를 만들고, `main.js`에서 `app.addPanel()`로 추가하면 됩니다. 기존 `AppUI`나 다른 panel의 코드는 변경할 필요가 없습니다. 이것은 Open-Closed Principle에 잘 맞습니다.

또한 View와 Backend 사이의 데이터 흐름도 명확합니다. 사용자 입력은 Panel에서 `Module.Controller` command로 전달됩니다. 반대로 Backend 상태는 `FactoryView` snapshot으로 복사되어 View에 전달됩니다. 따라서 View는 Model 객체를 직접 수정할 수 없고, Model의 내부 상태도 외부로 노출되지 않습니다.

결론적으로 JavaScript View는 ImGui 대신 DOM을 사용하지만, 수업 자료에서 요구한 MVC 구조, 즉 Model과 View의 분리, Controller를 통한 통신, composable UI component 구조를 유지하고 있습니다.

추가로 Memento UML은 공개 field가 아니라 private field와 accessor로 표현되어 있습니다. 이는 실제 코드(`FactoryMemento.hpp`)와 일치하며, public data member를 피하고 encapsulation을 강화하기 위한 것입니다.
