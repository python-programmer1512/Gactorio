아래는 **BE 상세 대본 + FE/JS MVC 보완 대본**을, 새로 올린 **Demo Flow 형식**에 맞춰 다시 통합한 발표 대본 초안이야. 참고한 흐름은 BE 대본, FE 보완 대본, 데모 진행 순서판이야.   

# Gactorio 발표 대본 — Demo Flow 기반 통합본

조원: 고원규, 조용빈
발표 주제: Gactorio C++ Backend / WebAssembly View / UML 구조 설명

---

## 0. 한 줄 요약 — 오프닝

안녕하세요. 지금부터 Gactorio 프로젝트를 발표하겠습니다.

Gactorio는 에너지 드링크 생산 공장을 시뮬레이션하는 C++ OOP 프로젝트입니다.
핵심 Backend는 C++로 작성했고, 이를 WebAssembly로 빌드한 뒤 HTML, CSS, JavaScript View와 연결했습니다.

오늘 발표는 다음 순서로 진행하겠습니다.

먼저 실제 애플리케이션 UI를 시연하고, 그다음 구현한 생산 시나리오를 설명하겠습니다. 이후 UML 다이어그램을 보면서 MVC 구조, 클래스 관계, 디자인 패턴, 확장성, SOLID 원칙을 설명하겠습니다.

---

# 1단계 — 애플리케이션 실행 & UI 시연

먼저 실제로 동작하는 Gactorio 화면을 보여드리겠습니다.

이 화면은 크게 시뮬레이션 제어 영역, 공장 생산라인 영역, 제품 생산 요청 영역, 재고 영역, 이벤트 로그 영역으로 구성되어 있습니다.

왼쪽 위의 Simulation Control 영역에서는 시뮬레이션을 일시정지하거나 다시 실행할 수 있습니다. Reset 버튼으로 전체 상태를 초기화할 수 있고, 배속 슬라이더를 통해 시뮬레이션 속도를 조절할 수 있습니다. 또한 Save Checkpoint와 Undo 버튼도 있습니다. 이 부분은 Memento Pattern을 사용해 현재 공장 상태를 저장하고 이전 상태로 되돌리는 기능입니다.

중앙의 Factory 영역은 실제 생산라인을 보여줍니다. 각 라인에는 여러 기계가 카드 형태로 표시됩니다. 카드에는 기계 이름, 현재 상태, 진행률, HP가 표시됩니다. 기계는 Idle, Working, Broken, Maintenance 같은 상태를 가질 수 있고, 상태에 따라 색상과 진행률이 바뀝니다.

오른쪽의 Products 영역에서는 Voltz Classic, Hyper Bolt, Aurora Zero 같은 제품 생산을 요청할 수 있습니다. 제품 버튼을 누르면 Controller를 통해 Backend에 생산 명령이 전달되고, 생산 작업이 queue에 들어갑니다.

Inventory 영역에서는 원자재와 완제품 재고를 확인할 수 있습니다. 생산이 완료되면 완제품 수량이 증가하고, 필요한 원자재는 생산 과정에서 소비됩니다.

Event Log 영역에서는 생산 중 발생한 이벤트가 표시됩니다. 예를 들어 작업 시작, 공정 완료, 제품 완성, 기계 고장, 수리 완료 같은 이벤트가 시간순으로 기록됩니다. 이 로그는 생산 로직이 직접 작성하는 것이 아니라 Observer Pattern을 통해 수집됩니다.

이제 제품 하나를 생산해 보겠습니다.

제품 버튼을 누르면 생산 작업이 queue에 들어가고, 생산라인이 현재 공정에 맞는 idle 기계를 찾아 작업을 배정합니다. 기계 상태가 Idle에서 Working으로 바뀌고, 진행률 막대가 차오르는 것을 볼 수 있습니다. 한 공정이 끝나면 다음 공정으로 넘어가고, 마지막 공정까지 완료되면 완제품이 Inventory에 추가됩니다.

여기서 중요한 점은 JavaScript View가 C++ Model을 직접 수정하지 않는다는 것입니다. View는 오직 Module.Controller라는 단일 경계를 통해 명령을 보내고, snapshot으로 받은 데이터를 화면에 표시합니다.

즉, UI 버튼은 Factory나 Machine을 직접 건드리지 않고, Controller에 command를 보내는 방식으로 동작합니다.

---

# 2단계 — 구현한 시나리오 설명

다음으로 구현한 주요 시나리오를 설명하겠습니다.

## 시나리오 1. Normal Flow — 정상 생산 흐름

첫 번째 시나리오는 Normal Flow입니다.

이 시나리오는 가장 기본적인 생산 흐름입니다. 제품 생산 요청이 들어오면 ProductionTask가 생성되고, ProductionLine의 queue에 들어갑니다.

생산 과정은 여러 공정 step으로 나뉩니다. 예를 들어 Mixing, Quality, Bottling, Packaging 같은 단계가 있고, 각 단계는 해당 역할을 수행할 수 있는 Machine에 배정됩니다.

ProductionLine은 queue에 대기 중인 작업을 확인하고, 현재 step을 처리할 수 있는 idle 기계를 찾습니다. 적절한 기계가 있으면 작업을 배정하고, 기계는 Working 상태로 전환됩니다.

기계의 update가 반복 호출되면서 진행률이 증가하고, step이 완료되면 ProductionTask는 다음 step으로 이동합니다. 마지막 step까지 완료되면 제품 생산이 끝나고, 완제품 수량이 Inventory에 반영됩니다.

이 구조에서 핵심은 다형성입니다. Factory나 ProductionLine의 update 루프는 구체 기계 타입을 직접 검사하지 않습니다. MixingStation인지, BottlingStation인지 if문으로 구분하지 않고, Machine 추상 인터페이스를 통해 update를 호출합니다.

즉, 시뮬레이션 루프는 Machine 포인터를 통해 다형적으로 동작합니다.

## 시나리오 2. Random Breakdowns — 무작위 고장과 정비

두 번째 시나리오는 Random Breakdowns입니다.

이 시나리오에서는 기계가 생산 중 일정 확률로 손상을 입을 수 있습니다. HP가 0이 되면 기계는 Broken 상태가 되고, 생산이 멈춥니다.

Broken 상태에서는 작업이 진행되지 않습니다. 사용자가 Repair 또는 Repair All 명령을 내리면 기계는 Maintenance 상태로 전환됩니다. 일정 시간이 지나 수리가 완료되면 HP가 회복되고 다시 Idle 또는 작업 가능한 상태로 돌아갑니다.

이 부분은 State Pattern을 사용했습니다. Machine은 단순히 MachineStatus enum만 들고 있는 것이 아니라, IdleState, WorkingState, BrokenState, MaintenanceState 같은 상태 객체를 가집니다.

각 상태 객체가 자신의 행동을 담당하기 때문에 Machine 클래스 안에 상태별 if문이 크게 늘어나지 않습니다. 예를 들어 WorkingState는 진행률 증가와 고장 가능성을 처리하고, BrokenState는 생산을 멈추며, MaintenanceState는 수리 진행을 담당합니다.

이 구조 덕분에 새로운 상태를 추가하더라도 기존 Machine update 구조를 크게 바꾸지 않고 확장할 수 있습니다.

## 추가 시나리오. Bottleneck / Overflow

추가로 라인별 scenario 선택 구조도 포함했습니다.

Bottleneck 시나리오는 특정 공정의 처리 속도를 낮춰 병목 현상을 만들 수 있습니다. 이 경우 앞 공정에서 작업은 계속 들어오지만, 특정 단계에서 처리 속도가 느려져 queue가 쌓이는 모습을 확인할 수 있습니다.

Overflow 시나리오는 queue capacity를 제한해, 수용 가능한 작업 수를 넘어서는 생산 요청이 들어왔을 때 dropped task count가 증가하도록 구성했습니다.

즉, 단순히 정상 생산만 구현한 것이 아니라, 생산라인 단위로 scenario를 바꿔가며 병목, 고장, 오버플로 같은 상황을 확인할 수 있도록 만들었습니다.

---

# 3단계 — 클래스 다이어그램 설명

이제 UML 다이어그램을 기준으로 전체 구조를 설명하겠습니다.

설명 순서는 MVC 구조, C++과 JavaScript 연결, is-a 관계, has-a 관계, dependency 관계, 디자인 패턴, 확장성, SOLID 원칙 순서입니다.

---

## 3-1. MVC 전체 구조와 C++ / JavaScript 연결

먼저 전체 구조입니다.

이 프로젝트는 MVC 원칙에 따라 Model, Controller, View를 분리했습니다.

Model은 C++ Backend의 gactorio 네임스페이스에 있는 클래스들입니다. Factory, ProductionLine, Machine, Product, Inventory, EventBus, Memento 계층이 여기에 해당합니다. 이들은 실제 시뮬레이션 상태와 규칙을 담당합니다.

Controller는 외부 요청을 받는 경계입니다. C++ 내부에는 FactoryController가 있고, WebAssembly 연결을 위해 ctrl::Controller라는 web-facing facade가 있습니다. JavaScript에서는 이 객체가 Module.Controller라는 이름으로 노출됩니다.

View는 docs/js 아래의 JavaScript 클래스들입니다. Application, AppUI, UIComponent, 그리고 여러 Panel 클래스들이 화면을 담당합니다.

C++과 JavaScript는 Emscripten embind를 통해 연결했습니다. src/web/bindings.cpp에서 C++의 Controller와 View용 구조체를 JavaScript에 공개합니다. 그래서 JavaScript에서는 new Module.Controller()를 통해 C++ Controller 객체를 만들고, tick, snapshot, enqueue, repair, undo 같은 메서드를 호출할 수 있습니다.

명령 흐름은 View에서 Controller로 내려갑니다.

사용자가 버튼을 누르면 JavaScript Panel이 Module.Controller 메서드를 호출합니다. 이 호출은 ctrl::Controller로 전달되고, 내부적으로 gactorio::FactoryController를 거쳐 Model을 조작합니다.

반대로 조회 흐름은 snapshot을 통해 올라옵니다.

Application은 주기적으로 controller.snapshot()을 호출합니다. Backend는 현재 Factory 상태를 FactoryView, LineView, MachineView 같은 View 전용 데이터로 복사해서 반환합니다. JavaScript는 이 데이터를 plain object로 변환한 뒤 화면을 다시 렌더링합니다.

여기서 중요한 점은 View가 Model 객체를 직접 알지 않는다는 것입니다. JavaScript 파일은 Factory, Machine, ProductionLine, Inventory 같은 C++ Model 객체를 직접 다루지 않습니다. 오직 Module.Controller에 command를 보내고, snapshot 데이터만 읽습니다.

따라서 Model과 View 사이의 결합도가 낮고, Backend 내부 구조를 바꾸더라도 Controller와 snapshot 경계만 유지하면 View 변경을 최소화할 수 있습니다.

---

## 3-2. JavaScript View 클래스 구조

다음은 JavaScript View 구조입니다.

최상위에는 Application 클래스가 있습니다. Application은 브라우저의 requestAnimationFrame을 사용해 main loop를 실행합니다. 매 frame마다 controller.tick(dt)을 호출해 시뮬레이션 시간을 진행시키고, 일정 간격마다 snapshot을 받아 화면을 갱신합니다.

Application 아래에는 AppUI가 있습니다. AppUI는 여러 panel을 관리하는 compositor 역할을 합니다. 내부에 UIComponent 목록을 가지고 있고, renderAll(snapshot)을 호출하면 등록된 모든 panel의 render가 실행됩니다.

UIComponent는 모든 panel의 공통 기반 클래스입니다. 각 panel은 UIComponent를 상속하고, 자신이 담당하는 화면 영역을 렌더링합니다.

주요 panel은 다음과 같습니다.

SimControlPanel은 pause, resume, reset, speed, checkpoint, undo 같은 시뮬레이션 제어를 담당합니다.

FactoryPanel은 생산라인과 기계 카드를 표시합니다. 각 라인의 scenario dropdown도 여기에서 다룹니다.

InspectorPanel은 선택한 기계의 상세 상태를 표시하고, repair나 force break 같은 조작을 수행합니다.

ProductsPanel은 제품 목록을 보여주고 생산 요청 버튼을 제공합니다.

InventoryPanel은 원자재와 완제품 재고를 표시하고, restock 명령을 전달합니다.

EventLogPanel은 Observer로 수집된 이벤트 로그를 화면에 보여줍니다.

RuntimeConfigPanel은 JSON 기반 runtime config를 불러오는 기능을 담당합니다.

이 구조의 장점은 화면 영역을 독립적으로 확장할 수 있다는 점입니다. 새로운 UI가 필요하면 UIComponent를 상속한 panel을 추가하고, AppUI에 등록하면 됩니다. 기존 panel 전체를 수정할 필요가 없습니다.

---

## 3-3. is-a 관계 — 상속 구조

이제 UML의 is-a 관계, 즉 상속 구조를 보겠습니다.

첫 번째는 Machine 계층입니다.

Machine은 추상 기반 클래스이고, 구체 기계들이 이를 상속합니다. 현재 기본 기계로는 MixingStation, QualityStation, BottlingStation, PackagingStation이 있습니다. 또한 config 기반 생성을 위해 ConfiguredStation도 사용할 수 있습니다.

Machine은 typeName, role, processType, update 같은 공통 인터페이스를 제공하고, 구체 기계들은 자신의 역할에 맞게 동작합니다.

두 번째는 MachineState 계층입니다.

MachineState는 기계 상태의 추상 기반 클래스입니다. 이를 IdleState, WorkingState, BrokenState, MaintenanceState가 상속합니다. 각 상태는 enter, update, exit, name 같은 메서드를 통해 상태별 행동을 담당합니다.

세 번째는 Product 계층입니다.

Product는 제품의 추상 기반 클래스이고, VoltzClassic, HyperBolt, AuroraZero 같은 구체 제품이 이를 상속합니다. 제품은 이름, ID, 공정 route, recipe 정보를 가집니다.

네 번째는 Observer 계층입니다.

Observer는 이벤트를 받는 공통 인터페이스입니다. EventLogObserver와 StatisticsObserver가 이를 상속합니다. EventLogObserver는 이벤트 로그를 저장하고, StatisticsObserver는 완료 제품 수, 고장 횟수, 수리 횟수 같은 통계를 계산합니다.

다섯 번째는 View 쪽 UIComponent 계층입니다.

UIComponent를 기반으로 SimControlPanel, FactoryPanel, ProductsPanel, InventoryPanel, EventLogPanel, InspectorPanel, RuntimeConfigPanel이 상속 구조를 이룹니다.

이처럼 Backend와 Frontend 모두에서 추상 기반 클래스와 구체 파생 클래스를 사용해 공통 인터페이스를 유지하고 있습니다.

---

## 3-4. has-a 관계 — 소유와 포함 구조

다음은 has-a 관계입니다.

Factory는 전체 공장의 aggregate root입니다. Factory는 SimClock, Inventory, ProductionLine 목록, EventBus, EventLogObserver, StatisticsObserver를 소유합니다.

ProductionLine은 생산이 실제로 일어나는 단위입니다. 내부에 ProductionTask queue를 가지고 있고, 여러 Machine을 unique_ptr로 소유합니다.

Machine은 현재 상태를 나타내는 MachineState를 unique_ptr로 소유합니다. 이 부분이 State Pattern의 핵심입니다.

ProductionTask는 하나의 제품 생산 작업을 나타냅니다. 어떤 Product를 생산하는지, 현재 몇 번째 step인지, input이 소비되었는지 등을 저장합니다.

Inventory는 원자재와 완제품 수량을 관리합니다. 실제 Product 객체를 저장하는 것이 아니라, item id와 product id 기준으로 수량을 관리합니다.

Memento 구조에서도 has-a 관계가 있습니다. FactoryMemento는 LineMemento 목록을 가지고, LineMemento는 MachineMemento와 ProductionTaskMemento 정보를 가집니다. SimulationHistory는 FactoryMemento를 stack 형태로 보관합니다.

여기서 중요한 점은 소유 관계를 명확하게 표현했다는 것입니다. 강한 소유는 unique_ptr이나 값 멤버로 표현하고, queue와 machine 사이에서 공유되는 작업은 shared_ptr로 표현합니다. 단순 참조나 캐시는 raw pointer를 사용하되, 소유하지 않는다는 의미를 분명히 했습니다.

---

## 3-5. dependency 관계 — 의존 방향

다음은 dependency 관계입니다.

의존 관계는 소유하지는 않지만, 생성하거나 호출하거나 사용하는 관계입니다.

FactoryController는 Factory를 조작하고, DTO Snapshot을 생성합니다. 외부 계층은 FactoryController를 통해서만 Model에 명령을 내립니다.

ProductCatalog는 ProductDefinition을 바탕으로 Product를 생성합니다. Product를 새로 추가할 때는 catalog의 정의를 통해 확장할 수 있습니다.

Factory는 checkpoint와 undo를 위해 FactoryMemento를 생성하고 복원합니다. 하지만 SimulationHistory는 Memento 내용을 해석하지 않고 보관만 합니다.

Machine과 ProductionLine은 EventBus에 의존합니다. 작업 시작, step 완료, 제품 완료, 기계 고장 같은 이벤트가 발생하면 EventBus를 통해 Observer에게 전달합니다.

View 쪽 panel들은 Module.Controller에 의존합니다. Panel은 버튼 입력을 Controller command로 전달하고, snapshot 데이터를 읽어 DOM을 갱신합니다.

전체 의존 방향은 View에서 Controller로, Controller에서 Model로 향합니다. Model은 View를 알지 못하고, JavaScript나 DOM에 의존하지 않습니다. 이 점이 MVC 분리에서 가장 중요합니다.

---

## 3-6. Tick / Update Sequence

다음은 한 번의 tick이 어떻게 진행되는지 설명하겠습니다.

외부에서 controller.tick(deltaTime)이 호출되면, Controller는 내부 Factory의 update(deltaTime)을 호출합니다.

Factory::update가 시작되면 먼저 SimClock이 갱신됩니다. SimClock은 실제 deltaTime을 시뮬레이션 시간으로 변환하고, pause 상태라면 시간이 진행되지 않도록 처리합니다.

그다음 Factory는 각 ProductionLine에 대해 작업 배정을 시도합니다. queue에 대기 중인 작업이 있고, 현재 step을 처리할 수 있는 idle 기계가 있으면 해당 작업을 기계에 배정합니다.

작업을 받은 기계는 Working 상태가 되고, update를 통해 공정을 진행합니다. WorkingState는 진행률을 증가시키고, 공정이 완료되면 ProductionTask를 다음 step으로 넘깁니다.

모든 step이 끝나면 제품 생산이 완료됩니다. Factory는 완제품을 Inventory에 추가하고, 제품 완료 이벤트를 EventBus에 발행합니다.

이후 Factory는 다시 작업 배정을 시도합니다. 방금 작업을 끝낸 기계가 idle 상태가 되었을 수 있기 때문에, 다음 tick까지 기다리지 않고 이어서 다음 작업을 배정할 수 있습니다.

정리하면 tick 흐름은 시간 갱신, 작업 배정, 기계 update, 공정 완료 처리, 재고 반영, 이벤트 발행, 재배정 순서입니다.

---

## 3-7. State Pattern

이제 디자인 패턴을 설명하겠습니다.

첫 번째는 State Pattern입니다.

Machine은 Idle, Working, Broken, Maintenance 상태를 가집니다. 이 상태들을 단순 enum 값으로만 처리하면 Machine 클래스 안에 상태별 if문이 계속 늘어납니다.

그래서 이 프로젝트에서는 MachineState라는 추상 클래스를 만들고, 상태별 행동을 하위 클래스로 분리했습니다.

IdleState는 작업을 받을 수 있는 상태입니다. 작업이 배정되면 WorkingState로 전환됩니다.

WorkingState는 실제 생산을 진행하는 상태입니다. 진행률을 올리고, 공정 완료 조건을 확인합니다. 또한 고장 가능성도 처리할 수 있습니다.

BrokenState는 기계가 고장난 상태입니다. 이 상태에서는 생산이 멈춥니다.

MaintenanceState는 수리 중인 상태입니다. 일정 시간이 지나면 HP를 회복하고 다시 작업 가능한 상태로 돌아갑니다.

이 설계 덕분에 Machine은 현재 상태 객체에게 update 행동을 위임할 수 있습니다. 새로운 상태가 필요할 때도 Machine 전체를 수정하기보다 MachineState 하위 클래스를 추가하는 방식으로 확장할 수 있습니다.

---

## 3-8. Observer / Event Pattern

두 번째는 Observer Pattern입니다.

생산 과정에서는 여러 이벤트가 발생합니다. 작업이 queue에 들어갔을 때, 작업이 시작되었을 때, step이 완료되었을 때, 제품이 완성되었을 때, 기계가 고장났거나 수리되었을 때 이벤트가 발생합니다.

이 이벤트를 생산 로직이 직접 로그나 통계에 반영하도록 만들면, Machine과 ProductionLine이 너무 많은 책임을 가지게 됩니다.

그래서 EventBus와 Observer 구조를 사용했습니다.

Machine이나 ProductionLine은 이벤트가 발생하면 EventBus::publish를 호출합니다. EventBus는 등록된 Observer들에게 이벤트를 전달합니다.

EventLogObserver는 이벤트를 받아 로그로 저장합니다. StatisticsObserver는 이벤트를 받아 완료 제품 수, 시작된 작업 수, 완료된 step 수, 고장 횟수, 수리 횟수 같은 통계를 갱신합니다.

이 구조의 장점은 생산 로직과 부가 기능이 분리된다는 점입니다. 나중에 알림 기능이나 디버깅 모니터링 기능이 필요하다면, 새로운 Observer를 추가하면 됩니다. 기존 Machine이나 ProductionLine 로직을 크게 바꾸지 않아도 됩니다.

---

## 3-9. Memento Pattern

세 번째는 Memento Pattern입니다.

이 프로젝트는 Save Checkpoint와 Undo 기능을 지원합니다. 이를 위해 Factory 상태를 저장하고 복원할 수 있는 Memento 구조를 만들었습니다.

Originator는 Factory입니다. Factory는 현재 시뮬레이션 상태를 FactoryMemento로 저장할 수 있고, 저장된 FactoryMemento를 받아 자신의 상태를 복원할 수 있습니다.

Memento 역할은 FactoryMemento, LineMemento, MachineMemento, ProductionTaskMemento 등이 담당합니다. 이들은 시뮬레이션 시간, 재고, 생산라인 상태, queue 상태, 기계 상태, 진행 중인 작업 정보를 저장합니다.

Caretaker는 SimulationHistory입니다. SimulationHistory는 FactoryMemento를 stack 형태로 보관합니다. 하지만 Memento 내부 내용을 직접 해석하지 않습니다. 실제 복원은 Originator인 Factory가 담당합니다.

외부에서는 FactoryController의 saveCheckpoint, undo, canUndo, historySize 같은 메서드를 통해 이 기능을 사용합니다.

여기서 DTO Snapshot과 Memento는 분리되어 있습니다. DTO는 View에 현재 상태를 보여주기 위한 데이터이고, Memento는 undo를 위해 내부 상태를 복원하는 데이터입니다. 따라서 화면 표시용 snapshot을 그대로 undo에 사용하지 않고, 별도의 FactoryMemento 계층을 사용합니다.

---

## 3-10. Config 기반 확장 구조

다음은 config 기반 확장 구조입니다.

현재 Backend는 hard-coded 기본 공장뿐 아니라 JSON config 기반 공장 생성도 지원합니다.

FactoryConfig는 JSON에서 읽어온 설정 데이터입니다. 여기에는 item, product, station, recipe, production line, initial inventory, startup task, scenario 정의가 들어 있습니다.

FactoryConfigLoader는 JSON 파일이나 문자열을 읽어 FactoryConfig로 변환합니다.

DefinitionRegistry는 각 정의를 id 기준으로 인덱싱하고, station이나 recipe가 서로 올바르게 연결되어 있는지 검증합니다.

FactoryBuilder는 검증된 config를 실제 runtime 객체로 바꿉니다. 생산라인을 만들고, StationFactory를 통해 기계를 생성하고, 초기 재고와 시작 작업을 세팅합니다.

이 구조 덕분에 새 제품이나 새 생산라인을 추가할 때 C++ update 루프 전체를 수정할 필요가 줄어듭니다. 설정 데이터와 builder 경로를 통해 공장 구성을 확장할 수 있습니다.

---

## 3-11. 확장성과 유지보수성

이제 이 설계가 왜 확장에 유리한지 정리하겠습니다.

새 기계 타입을 추가하려면 Machine을 상속한 클래스를 만들고, 필요한 가상 함수를 구현한 뒤 생성 경로에 등록하면 됩니다. Factory update 루프는 Machine 인터페이스만 사용하므로 수정할 필요가 적습니다.

새 제품을 추가할 때는 ProductCatalog나 config 정의에 제품 정보를 추가하면 됩니다. 제품의 route와 recipe가 데이터로 분리되어 있기 때문에, 생산 루프를 제품별 if문으로 수정하지 않아도 됩니다.

새 기계 상태를 추가할 때는 MachineState를 상속한 상태 클래스를 만들고 전이 지점을 연결하면 됩니다. Machine 내부에 거대한 switch문을 추가하지 않아도 됩니다.

새 통계나 로그 기능이 필요하면 Observer를 새로 구현해 EventBus에 등록하면 됩니다. 이벤트 발행자는 기존과 동일하게 EventBus에 publish만 하면 됩니다.

새 UI 영역이 필요하면 UIComponent를 상속한 panel을 만들고 AppUI에 등록하면 됩니다. 기존 panel을 크게 수정할 필요가 없습니다.

정리하면, 이 프로젝트는 새 기능을 기존 핵심 루프에 직접 끼워 넣기보다, 추상 클래스와 등록 구조를 통해 확장하는 방향으로 설계했습니다.

---

## 3-12. SOLID 원칙 정리

마지막으로 SOLID 원칙 관점에서 정리하겠습니다.

첫째, Single Responsibility Principle입니다. Factory는 전체 공장 상태를 관리하고, ProductionLine은 작업 배정을 담당하며, Machine은 공정 진행을 담당합니다. Inventory는 재고만 관리하고, EventBus는 이벤트 전달만 담당합니다. SimulationHistory는 Memento 보관만 담당합니다.

둘째, Open-Closed Principle입니다. 기계, 제품, 상태, Observer, UI panel은 기존 핵심 구조를 크게 수정하지 않고 새 클래스를 추가하는 방식으로 확장할 수 있습니다.

셋째, Liskov Substitution Principle입니다. Factory와 ProductionLine은 구체 기계가 아니라 Machine 추상 타입을 통해 동작합니다. EventBus도 구체 Observer가 아니라 Observer 인터페이스를 통해 이벤트를 전달합니다.

넷째, Interface Segregation Principle입니다. Observer는 onEvent 중심의 작은 인터페이스이고, MachineState는 상태 전이에 필요한 메서드만 가집니다. UIComponent도 bind와 render 중심으로 역할이 분리되어 있습니다.

다섯째, Dependency Inversion Principle입니다. View는 구체 Model에 직접 의존하지 않고 Module.Controller라는 경계에 의존합니다. Backend 내부에서도 ProductionLine은 구체 기계 클래스가 아니라 Machine 추상 클래스에 의존합니다.

결과적으로 이 설계는 캡슐화, 상속, 다형성, 추상화, 그리고 State, Observer, Memento Pattern을 함께 사용해 유지보수와 확장이 쉬운 구조를 목표로 했습니다.

---

# 4단계 — Q&A 대비

마지막으로 예상 질문에 대한 답변을 준비했습니다.

## Q1. 왜 Dear ImGui가 아니라 Web UI를 사용했나요?

과제의 핵심은 UI 기술 자체보다 MVC 분리와 Backend 구조라고 판단했습니다.
그래서 C++ Model과 Controller를 WebAssembly로 빌드하고, View는 HTML/CSS/JavaScript로 구현했습니다.

중요한 점은 View와 Model이 직접 연결되지 않는다는 것입니다. JavaScript View는 Module.Controller라는 단일 경계를 통해서만 명령과 조회를 수행합니다. 따라서 UI 기술은 DOM으로 바뀌었지만, MVC 원칙은 유지했습니다.

## Q2. 시뮬레이션 루프에 구체 타입 분기가 있나요?

핵심 update 루프에서는 구체 기계 타입을 직접 검사하지 않습니다.
Factory와 ProductionLine은 Machine 추상 인터페이스를 통해 작업을 배정하고 update를 호출합니다.

즉, MixingStation인지 BottlingStation인지 직접 if문으로 구분하지 않고, role과 processType 같은 공통 인터페이스를 사용합니다.

## Q3. DTO Snapshot과 Memento는 무엇이 다른가요?

DTO Snapshot은 View에 현재 상태를 보여주기 위한 데이터입니다.
예를 들어 FactorySnapshot, MachineSnapshot, InventorySnapshot은 화면 표시를 위해 필요한 값을 복사해서 전달합니다.

반면 Memento는 undo를 위한 내부 상태 저장 데이터입니다. 진행 중인 작업, 기계 상태, 재고, 시간처럼 복원에 필요한 정보를 저장합니다.

따라서 snapshot은 표시용이고, memento는 복원용입니다.

## Q4. Observer Pattern을 쓴 이유는 무엇인가요?

생산 로직이 직접 로그와 통계를 수정하게 만들면 Machine과 ProductionLine의 책임이 커집니다.

그래서 EventBus를 통해 이벤트를 발행하고, EventLogObserver와 StatisticsObserver가 각각 로그 저장과 통계 갱신을 담당하도록 분리했습니다.

이 구조 덕분에 나중에 새로운 통계나 알림 기능을 추가해도 생산 로직을 크게 바꾸지 않아도 됩니다.

## Q5. C++과 JavaScript 사이에서 메모리 관리는 어떻게 하나요?

Embind가 반환하는 vector나 객체 핸들은 JavaScript GC가 자동으로 정리하지 않습니다.

그래서 util.js에서 C++ vector를 JavaScript 배열로 복사한 뒤 delete를 호출해 핸들을 해제합니다.
이 처리를 panel마다 흩어 놓지 않고 util.js에 모아서, 각 panel은 화면 렌더링에 집중하도록 했습니다.

## Q6. 이 프로젝트에서 가장 중요한 OOP 설계 포인트는 무엇인가요?

가장 중요한 포인트는 Model과 View의 분리, 그리고 다형적 시뮬레이션 루프입니다.

View는 Model을 직접 만지지 않고 Controller를 통해서만 통신합니다.
또한 Factory와 ProductionLine은 구체 기계 타입을 직접 검사하지 않고 Machine 인터페이스를 통해 update를 호출합니다.

이 두 가지가 MVC 구조와 OOP 다형성을 가장 잘 보여주는 부분이라고 생각합니다.

---

# 마무리

정리하겠습니다.

Gactorio Backend는 Factory를 중심으로 생산라인, 기계, 제품, 재고, 시간, 이벤트, 통계, undo 기능을 관리합니다.

Controller와 DTO Snapshot을 통해 View와 Model을 분리했고, JavaScript View는 Module.Controller를 통해서만 C++ Backend와 통신합니다.

Machine은 State Pattern으로 상태별 행동을 분리했고, EventBus와 Observer를 통해 이벤트 로그와 통계를 생산 로직에서 분리했습니다. 또한 Memento Pattern을 사용해 checkpoint와 undo 기능을 구현했습니다.

마지막으로 config 기반 구조를 통해 제품, 기계, 생산라인, 시나리오를 데이터 중심으로 확장할 수 있게 했습니다.

이상으로 Gactorio 프로젝트 발표를 마치겠습니다. 감사합니다.
