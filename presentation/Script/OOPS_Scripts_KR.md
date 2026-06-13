# Gactorio OOPS 발표 대본

조원: 고원규, 조용빈  
최종 수정 일자: 26.06.13


---

# 1. Application UI Demonstration - 1 min

먼저 Gactorio 실행 파일을 열어 실제 UI를 보여드리겠습니다.

화면 상단에는 `Gactorio`라는 제목과 함께 에너지 드링크 공장 시뮬레이터라는 설명이 보입니다. 이 프로젝트는 C++ Backend를 WebAssembly로 빌드하고, HTML, CSS, JavaScript View에서 조작하는 구조입니다.

왼쪽의 `Simulation Control` 영역에서는 현재 simulation time을 확인할 수 있고, `Pause`, `Reset`, `Speed` slider로 시뮬레이션 실행 상태와 속도를 제어할 수 있습니다. 여기서 speed를 올리면 시간이 더 빠르게 진행되고, pause를 누르면 생산 진행이 멈춥니다.

그 아래 `Memento` 영역에는 `Save Checkpoint`와 `Undo` 버튼이 있습니다. 현재 공장 상태를 checkpoint로 저장한 뒤, 제품 생산이나 기계 상태가 바뀐 후에도 undo를 누르면 이전 상태로 되돌아갈 수 있습니다. history 숫자는 저장된 checkpoint 개수를 보여줍니다.

가운데 `Factory` 영역에는 생산라인과 기계 카드가 표시됩니다. 각 라인에는 Mixing, Quality, Bottling, Packaging 같은 기계가 있고, 제품 작업이 들어오면 기계 상태와 progress가 바뀌는 것을 볼 수 있습니다. `+ Add Line` 버튼으로 새로운 생산라인도 추가할 수 있습니다.

오른쪽 `Products` 영역에서는 Voltz Classic, Hyper Bolt, Aurora Zero 같은 제품을 선택해 생산 queue에 넣을 수 있습니다. `Inventory` 영역에서는 원자재와 완제품 재고를 확인하고, 원자재 restock도 할 수 있습니다.

마지막으로 `Statistics`와 `Event Log`에는 Observer pattern으로 수집된 생산 시작, 공정 완료, 제품 완료, 기계 고장, 수리 완료 같은 이벤트와 통계가 표시됩니다. 즉, 이 UI는 단순 화면이 아니라 Backend의 Factory, Controller, DTO, Event 구조가 실제로 연결되어 동작하는 결과입니다.

---

# 2. Implemented Scenarios - 1 min

저희가 구현한 대표 시나리오는 세 가지입니다.

첫 번째는 제품 생산 시나리오입니다. 사용자가 Products panel에서 제품을 선택하면 View는 Model을 직접 건드리지 않고 Controller에 enqueue command를 보냅니다. Controller는 Factory에 요청을 전달하고, Factory는 Inventory에서 필요한 원자재를 확인하고 소비한 뒤 ProductionTask를 생성해 ProductionLine queue에 넣습니다. 이후 tick이 진행되면 ProductionLine이 idle 상태의 Machine에 작업을 배정하고, Machine은 공정 진행률을 올립니다. 모든 ProcessStep이 끝나면 완제품 수량이 Inventory에 반영되고 Event Log와 Statistics도 갱신됩니다.

두 번째는 기계 고장과 수리 시나리오입니다. 생산 중 Machine의 health가 줄어들거나 break command가 호출되면 Machine은 Broken 상태가 됩니다. Broken 상태에서는 update가 호출되어도 생산이 진행되지 않습니다. 이후 repair 또는 repairAll command가 들어오면 Maintenance 상태로 전환되고, 수리 시간이 진행된 뒤 다시 사용 가능한 상태로 돌아갑니다. 이 흐름은 State Pattern으로 구현되어 Idle, Working, Broken, Maintenance 상태별 동작이 분리되어 있습니다.

세 번째는 checkpoint와 undo 시나리오입니다. 사용자가 Save Checkpoint를 누르면 FactoryController가 Factory의 현재 상태를 FactoryMemento로 저장하고 SimulationHistory stack에 보관합니다. 그 후 라인을 추가하거나 제품을 생산한 뒤 Undo를 누르면 가장 최근 FactoryMemento를 꺼내 Factory가 자신의 시간, 재고, 생산라인 queue, 기계 상태를 복원합니다. 이 기능은 Memento Pattern으로 구현했습니다.

---

# 3. Class Diagrams and Class Structures - 13 min

이제 클래스 다이어그램을 보면서 전체 class structure를 설명하겠습니다.

## 3.1 Overall Class Diagram

먼저 전체 클래스 다이어그램입니다.

Backend는 크게 `Controller`, `DTO`, `Common`, `Model` 영역으로 나뉩니다. 전체 흐름에서 가장 중요한 점은 외부 요청이 `FactoryController`로 들어오고, `FactoryController`가 Model을 조작한 뒤, 그 결과를 DTO Snapshot 형태로 외부에 반환한다는 것입니다.

`FactoryController`는 Backend의 진입점입니다. 사용자가 시뮬레이션을 진행하거나 제품을 추가하거나 checkpoint와 undo 기능을 사용할 때 직접 접근하는 클래스입니다. 내부적으로는 `CarbonationFactory`를 `unique_ptr`로 소유하고, undo 기능을 위해 `SimulationHistory`도 함께 가집니다.

DTO 영역은 외부에 전달되는 읽기 전용 상태 객체입니다. `FactorySnapshot`은 전체 공장 상태를 대표하고, 그 안에 `InventorySnapshot`, `ProductionLineSnapshot`, `MachineSnapshot`, `EventSnapshot`, `StatisticsSnapshot`이 포함됩니다. 이 구조의 핵심은 Model 객체를 직접 외부에 노출하지 않고, 현재 상태를 값으로 복사해서 전달한다는 점입니다.

Common 영역에는 `SimClock`과 공통 enum, ID type이 있습니다. `SimClock`은 시뮬레이션 시간, 배속, 일시정지 상태를 담당하고, `Types.hpp`에는 `ProductId`, `MachineId`, `ItemType`, `MachineStatus`, `EventType` 같은 공통 정의가 있습니다.

Model 영역은 실제 시뮬레이션 로직입니다. 중심에는 `Factory`가 있고, `Factory`는 `Inventory`, `ProductionLine`, `EventBus`, `EventLogObserver`, `StatisticsObserver`, `SimClock`을 관리합니다. `ProductionLine`은 작업 queue와 Machine 목록을 가지고 실제 생산을 수행하고, `Machine`은 각 공정 단계를 처리합니다.

여기서 has-a 관계를 먼저 설명하겠습니다. `FactoryController` has a `CarbonationFactory` and `SimulationHistory`입니다. `Factory` has a `SimClock`, `Inventory`, `ProductionLine` list, `EventBus`, Observer objects입니다. `ProductionLine` has machines and task queue입니다. `FactorySnapshot` has other snapshot DTOs입니다. 이런 관계는 composition 또는 aggregation으로 표현했고, 객체의 생명주기와 책임이 어디에 있는지 보여줍니다.

반대로 is-a 관계는 상속 관계입니다. `CarbonationFactory` is a `Factory`입니다. `MixingStation`, `QualityStation`, `BottlingStation`, `PackagingStation` are `Machine`입니다. `IdleState`, `WorkingState`, `BrokenState`, `MaintenanceState` are `MachineState`입니다. `EventLogObserver`와 `StatisticsObserver` are `Observer`입니다. 이 상속 관계 덕분에 Factory나 EventBus는 구체 클래스가 아니라 추상화된 역할을 기준으로 동작할 수 있습니다.

dependency 관계도 있습니다. `FactoryController`는 Model 계층에 의존하지만 View는 Model에 직접 의존하지 않습니다. View는 `ctrl::Controller`와 `FactoryView` 같은 View-friendly DTO에만 의존합니다. Model 내부에서는 `Machine`이 이벤트 발행을 위해 `EventBus` pointer에 의존하고, Panel들은 command 전송을 위해 `Module.Controller`에 의존합니다. 이런 dependency는 필요한 방향으로만 흐르도록 제한했습니다.

## 3.2 Core Simulation Class Diagram

다음은 Core Simulation Class Diagram입니다.

이 다이어그램의 핵심은 `Factory`를 중심으로 생산라인, 기계, 작업, 제품, 재고가 어떻게 협력하는지입니다. `Factory`는 aggregate root 역할을 합니다. 즉, 공장 내부의 시간, 재고, 생산라인, 이벤트 시스템을 한 곳에서 관리하는 중심 객체입니다.

`Factory`는 `SimClock`으로 시간을 갱신하고, `Inventory`로 원자재와 완제품 수량을 관리합니다. 여러 `ProductionLine`은 실제 생산이 일어나는 단위이고, 각 라인은 대기 중인 `ProductionTask` queue와 실제 공정을 처리하는 `Machine` 목록을 가집니다.

`ProductionTask`는 제품 하나의 생산 진행 상태를 나타냅니다. 하나의 `Product`를 참조하고, `currentStepIndex`로 현재 몇 번째 공정까지 진행했는지 관리합니다. 제품마다 필요한 공정 순서가 다르기 때문에, `Product`가 가진 `ProcessStep` 목록을 따라 현재 필요한 기계 역할을 확인합니다.

`Machine`은 실제 공정을 처리하는 추상 기반 클래스입니다. 구체 기계들은 `Machine`을 상속하고, 각자 처리할 수 있는 역할에 따라 작업을 수행합니다. 예를 들어 Mixing 역할의 기계는 혼합 공정을, Bottling 역할의 기계는 병입 공정을 처리합니다.

실행 흐름은 `FactoryController::tick(deltaTime)`에서 시작됩니다. Controller는 직접 생산 로직을 처리하지 않고 `Factory::update()`를 호출합니다. `Factory`는 먼저 `SimClock`을 갱신하고, 각 ProductionLine에 대해 가능한 작업을 Machine에 배정합니다. 이후 Machine update가 진행되고, 공정이 완료되면 task가 다음 step으로 넘어가거나 완제품으로 완료됩니다. 완료된 제품은 `Inventory::addProduct()`로 반영되고, 필요한 이벤트는 `EventBus`로 발행됩니다.

이 구조가 수정하기 쉬운 이유는 생산의 큰 흐름과 세부 역할이 분리되어 있기 때문입니다. 제품을 추가할 때는 ProductCatalog의 ProductDefinition을 추가하면 되고, 기계를 추가할 때는 Machine을 상속한 새 클래스를 만들면 됩니다. Factory의 전체 orchestration은 유지하면서 세부 객체를 확장할 수 있습니다.

## 3.3 Product Domain Class Diagram

다음은 Product Domain Class Diagram입니다.

이 다이어그램은 제품 하나가 어떤 데이터로 정의되고, 그 데이터가 생산 작업과 재고 관리에 어떻게 사용되는지 보여줍니다. 중심 클래스는 `ProductCatalog`, `ProductDefinition`, `Product`, `Inventory`, `ProductionTask`입니다.

`ProductCatalog`는 제품 정의를 모아두는 registry입니다. 각 제품의 ID, 이름, 타입, 필요한 재료, 공정 경로가 `ProductDefinition` 형태로 저장됩니다. `ProductDefinition`은 제품 하나를 만들기 위한 설계도라고 볼 수 있습니다.

`ProductDefinition`에는 두 가지 핵심 데이터가 있습니다. 첫 번째는 `ItemRequirement`입니다. 제품 하나를 만들기 위해 어떤 원자재가 몇 개 필요한지 나타냅니다. 두 번째는 `ProcessStep`입니다. 제품이 어떤 공정 순서로 생산되는지 나타내고, 각 step에는 필요한 `MachineRole`과 기준 처리 시간이 들어 있습니다.

`ProductCatalog`는 이 정의를 바탕으로 실제 `Product` 객체를 생성합니다. `ProductionTask`는 Product를 참조하면서 현재 공정 진행 상태를 관리합니다. `Inventory`는 실제 Product 객체를 저장하지 않고, 원자재는 `ItemType` 기준으로, 완제품은 `ProductId` 기준으로 수량만 저장합니다.

여기서 has-a 관계는 `Product` has item requirements and process steps입니다. `ProductionTask` has a Product reference and current step index입니다. `Inventory` has maps of item quantities and product quantities입니다.

is-a 관계로는 `VoltzClassic`, `HyperBolt`, `AuroraZero`가 `Product`의 구체 제품이고, 원자재 쪽에서는 `Ingredient`, `Water`, `EmptyBottle`, `Label`, `Package`가 `Item` 계층에 속합니다.

이 설계는 Open-Closed Principle에 도움이 됩니다. 새로운 제품을 추가할 때 생산라인이나 Machine 코드를 수정하는 대신 제품 정의를 추가하면 됩니다. 생산 시스템은 Product의 requirements와 route만 읽기 때문에 구체 제품 이름에 강하게 묶이지 않습니다.

## 3.4 Machine State Diagram

다음은 Machine State Diagram입니다.

이 다이어그램의 핵심은 `Machine`이 단순한 상태값 하나로만 동작하지 않고, State Pattern을 이용해 상태별 행동을 별도 클래스로 분리했다는 점입니다.

Machine은 네 가지 상태를 가집니다. `Idle`, `Working`, `Broken`, `Maintenance`입니다. Idle은 작업 대기 상태이고, Working은 생산 진행 상태입니다. Broken은 고장 상태라서 update가 호출되어도 생산이 진행되지 않습니다. Maintenance는 수리 중인 상태이고, 수리가 완료되면 다시 사용 가능한 상태로 돌아갑니다.

`Machine`은 현재 상태를 나타내는 `MachineState` 객체를 가지고 있고, update가 호출되면 실제 동작을 이 State 객체에게 위임합니다. 예를 들어 현재 상태가 `WorkingState`라면 생산 진행 로직이 실행되고, `MaintenanceState`라면 수리 진행 로직이 실행됩니다.

여기서 `MachineStatus` enum은 외부 표시와 저장을 위한 값입니다. DTO Snapshot에서 기계 상태를 보여주거나 Memento로 저장하고 복원할 때는 enum 값이 필요합니다. 반면 실제 행동은 `MachineState` 클래스들이 담당합니다.

이 설계는 Single Responsibility Principle과 Open-Closed Principle에 맞습니다. `Machine`은 기계의 공통 데이터와 상태 전환을 관리하고, 상태별 update 로직은 각 State 클래스가 담당합니다. 새로운 상태가 필요할 때도 Machine 안에 거대한 if문을 추가하는 방식이 아니라, 새로운 MachineState 구현을 추가하는 방식으로 확장할 수 있습니다.

## 3.5 Observer / Event Pattern Diagram

다음은 Observer / Event Pattern Diagram입니다.

이 구조의 핵심은 생산 과정에서 발생한 이벤트를 `EventBus`가 전달하고, 여러 Observer가 그 이벤트를 받아 각자의 역할을 수행한다는 점입니다.

이벤트를 발생시키는 주체는 주로 `ProductionLine`과 `Machine`입니다. 작업이 queue에 들어갔을 때, 작업이 시작되었을 때, 공정이 완료되었을 때, 제품이 완성되었을 때, 기계가 고장났을 때, 수리가 완료되었을 때 이벤트가 발생합니다.

중요한 점은 ProductionLine이나 Machine이 로그나 통계를 직접 수정하지 않는다는 것입니다. 대신 `Event` 객체를 만들고 `EventBus::publish()`를 통해 발행합니다. `Event`에는 발생 시간, 이벤트 타입, 관련 기계 ID, 메시지 같은 정보가 담깁니다.

`EventBus`는 등록된 `Observer` 목록을 가지고 있다가 이벤트가 발행되면 각 Observer의 `onEvent()`를 호출합니다. 대표적인 Observer는 `EventLogObserver`와 `StatisticsObserver`입니다. `EventLogObserver`는 이벤트를 내부 목록에 저장하고, `StatisticsObserver`는 이벤트 타입을 기준으로 완료 제품 수, 기계 고장 횟수 같은 통계를 갱신합니다.

이 설계의 dependency는 매우 명확합니다. 생산 로직은 EventBus에만 의존하고, 로그와 통계의 구체 처리 방식에는 의존하지 않습니다. 새로운 알림 기능이나 모니터링 기능이 필요하다면 새로운 Observer를 추가하면 됩니다. 기존 Machine이나 ProductionLine 코드를 크게 수정할 필요가 없습니다.

이 부분은 Dependency Inversion Principle에도 연결됩니다. 이벤트 처리자는 `Observer` 인터페이스를 기준으로 연결되고, EventBus는 구체 Observer의 내부 구현에 의존하지 않습니다.

## 3.6 Memento Pattern Diagram

다음은 Memento Pattern Diagram입니다.

이 다이어그램의 핵심은 시뮬레이션 상태를 checkpoint로 저장하고, 필요할 때 이전 상태로 되돌릴 수 있도록 역할을 분리했다는 점입니다.

Memento Pattern에서 중요한 역할은 세 가지입니다. 첫 번째는 Originator이고, 이 프로젝트에서는 `Factory`가 담당합니다. Factory는 현재 시뮬레이션 상태를 `FactoryMemento`로 저장할 수 있고, 반대로 FactoryMemento를 받아 자신의 상태를 복원할 수 있습니다.

두 번째는 Memento입니다. 이 역할은 `FactoryMemento`, `LineMemento`, `MachineMemento`가 담당합니다. `FactoryMemento`는 전체 공장 상태를 저장하고, 그 안에 생산라인 상태와 기계 상태가 함께 들어갑니다. `LineMemento`는 생산라인의 queue 정보를 저장하고, `MachineMemento`는 개별 기계의 상태와 체력 같은 정보를 저장합니다.

여기서 Memento 객체들은 public field를 가진 struct가 아니라 private field와 getter를 가진 class입니다. 즉, Factory와 복원 로직은 필요한 값을 읽을 수 있지만, 외부에서 snapshot 내부 값을 직접 바꾸는 방식은 막아 두었습니다. 이 점은 encapsulation을 강화합니다.

세 번째는 Caretaker이고, `SimulationHistory`가 이 역할을 합니다. `SimulationHistory`는 FactoryMemento들을 stack 형태로 보관하고 checkpoint 저장과 undo 기능을 지원합니다. 하지만 Factory 내부 객체를 직접 수정하지는 않습니다. 실제 상태를 어떻게 복원할지는 Originator인 Factory가 담당합니다.

외부에서 이 기능을 사용하는 진입점은 `FactoryController`입니다. `saveCheckpoint()`, `undo()`, `canUndo()`, `historySize()` 같은 메서드를 제공해서 View나 외부 계층은 Memento 내부 구조를 몰라도 상태 저장과 복원을 사용할 수 있습니다.

이 구조는 Single Responsibility Principle에 맞습니다. Factory는 자신의 상태 저장과 복원을 담당하고, SimulationHistory는 저장된 snapshot stack 관리만 담당합니다. 또한 DTO Snapshot과 Memento를 분리했기 때문에 화면 표시용 데이터와 복원용 내부 상태가 섞이지 않습니다.

## 3.7 FE JS View Class Diagram and MVC Boundary

Backend UML에 이어, 실제 화면을 담당하는 JavaScript View 구조도 간단히 보겠습니다.

이 프로젝트의 MVC 매핑은 다음과 같습니다. Model은 `gactorio::*` C++ backend class들이고, Controller는 `ctrl::Controller`입니다. JavaScript에서는 Emscripten embind를 통해 이 Controller가 `Module.Controller`라는 객체로 노출됩니다. View는 `Application`, `AppUI`, `UIComponent`, 그리고 각 Panel class입니다.

중요한 점은 View가 Model을 직접 알지 않는다는 것입니다. JavaScript 파일에서는 `Factory`, `Machine`, `ProductionLine` 같은 Model class를 직접 다루지 않습니다. View는 오직 `Module.Controller`에 command를 보내고, `snapshot()`으로 받은 plain data만 읽어서 화면을 렌더링합니다.

`Application`은 main loop를 담당합니다. 매 frame마다 controller의 `tick()`을 호출하고, 일정 간격으로 `snapshot()`을 가져와 `AppUI`에 전달합니다. `AppUI`는 여러 `UIComponent` panel을 보관하고 `renderAll(snapshot)`으로 전체 panel을 렌더링합니다.

각 화면 영역은 별도 panel class로 나뉩니다. `SimControlPanel`은 pause, reset, speed, checkpoint, undo를 담당합니다. `FactoryPanel`은 production line과 machine card를 렌더링합니다. `ProductsPanel`은 product catalog와 enqueue button을 담당합니다. `InventoryPanel`은 raw item restock과 finished product table을 담당합니다. `EventLogPanel`은 Observer pattern으로 수집된 event log를 보여줍니다.

여기서 is-a 관계는 각 Panel이 `UIComponent`를 상속한다는 점입니다. has-a 관계는 `Application` has an `AppUI`, `AppUI` has UIComponent panels입니다. dependency는 Panel들이 command 전송을 위해 `Module.Controller`에 의존하고, rendering을 위해 snapshot data에 의존하는 구조입니다.

이 View 구조도 확장하기 쉽습니다. 새 panel이 필요하면 `UIComponent`를 상속한 class를 만들고 `main.js`에서 `app.addPanel()`로 추가하면 됩니다. 기존 AppUI나 다른 panel의 코드는 크게 바꿀 필요가 없습니다.

## 3.8 How the Code Is Easy to Extend and Modify

이 코드가 확장과 수정에 쉬운 이유를 정리하겠습니다.

첫째, Controller, DTO, Model, View 경계가 분리되어 있습니다. View는 Model 내부 클래스를 직접 수정하지 않고 Controller command와 snapshot만 사용합니다. 그래서 UI를 바꿔도 생산 로직 변경이 작고, 반대로 Backend 내부 구조를 조정해도 DTO 계약만 유지하면 View 영향이 줄어듭니다.

둘째, 제품 정의와 생산 흐름이 분리되어 있습니다. 새로운 제품을 추가할 때 ProductCatalog의 정의와 공정 route를 추가하면 되고, ProductionLine과 Machine의 핵심 알고리즘은 그대로 사용할 수 있습니다.

셋째, 상태, 이벤트, 복원 기능이 패턴으로 분리되어 있습니다. 기계 상태는 State Pattern, 로그와 통계는 Observer Pattern, checkpoint와 undo는 Memento Pattern이 담당합니다. 따라서 한 기능을 수정할 때 다른 기능으로 영향이 번지는 범위가 줄어듭니다.

넷째, has-a 관계와 is-a 관계가 명확합니다. 소유해야 하는 것은 composition으로 묶고, 역할을 확장해야 하는 것은 inheritance와 interface로 표현했습니다. 이 때문에 객체 생명주기와 확장 지점을 이해하기 쉽습니다.

## 3.9 SOLID Principles

마지막으로 SOLID 원칙 관점에서 보겠습니다.

Single Responsibility Principle입니다. `FactoryController`는 외부 명령 처리와 snapshot 제공을 담당하고, `Factory`는 공장 상태와 시뮬레이션 흐름을 담당합니다. `Inventory`는 재고 관리만, `SimClock`은 시간 관리만, `EventLogObserver`는 로그 저장만, `StatisticsObserver`는 통계 갱신만 담당합니다.

Open-Closed Principle입니다. 새로운 제품은 ProductCatalog 정의를 추가해서 확장할 수 있고, 새로운 기계는 Machine을 상속해 추가할 수 있습니다. 새로운 이벤트 처리 기능은 Observer를 추가하는 방식으로 확장할 수 있고, 새로운 View panel은 UIComponent를 상속해 추가할 수 있습니다.

Liskov Substitution Principle입니다. `MixingStation`, `QualityStation`, `BottlingStation`, `PackagingStation`은 모두 `Machine`으로 다룰 수 있고, 각 MachineState도 `MachineState` 인터페이스로 교체되어 동작할 수 있습니다. Observer 구현체들도 `Observer`로 동일하게 취급됩니다.

Interface Segregation Principle입니다. View는 전체 Model API를 알 필요 없이 `ctrl::Controller`의 View용 command와 `FactoryView` snapshot만 사용합니다. Observer도 `onEvent()`라는 필요한 인터페이스만 구현하면 됩니다. 각 계층은 자신에게 필요한 작은 접점만 사용합니다.

Dependency Inversion Principle입니다. 이벤트 처리는 구체 로그 클래스나 통계 클래스가 아니라 `Observer` 추상화에 의존합니다. View 역시 C++ Model 구체 클래스에 직접 의존하지 않고 Controller와 DTO에 의존합니다. 이 덕분에 상위 흐름은 구체 구현 변경에 덜 민감합니다.

정리하면, Gactorio는 Factory를 중심으로 시뮬레이션 상태를 관리하고, Controller와 DTO로 외부 계층과 Model을 분리했습니다. 또한 State, Observer, Memento Pattern을 사용해 상태 관리, 이벤트 처리, 상태 복원 책임을 분리했습니다. 그래서 요구사항이 늘어나더라도 기존 코드를 크게 흔들지 않고 확장하고 수정할 수 있는 구조입니다.

이상으로 발표를 마치겠습니다. 감사합니다.
