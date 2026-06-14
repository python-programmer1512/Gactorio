# Gactorio BE 발표 대본

조원: 고원규, 조용빈  
최종 수정 일자: 26.06.14

---

## 1. 프로젝트와 BE 범위

안녕하세요. 지금부터 Gactorio 프로젝트의 Backend 구조와 UML 설계를 설명하겠습니다.

Gactorio는 에너지 드링크 생산 공장을 시뮬레이션하는 C++ 프로젝트입니다. Backend는 생산라인, 기계, 제품, 재고, 시뮬레이션 시간, 이벤트 로그, 통계, 그리고 checkpoint와 undo 기능을 관리합니다.

이번 발표의 범위는 C++ Backend입니다. 화면 렌더링이나 JavaScript View는 핵심 BE 모델에 포함하지 않고, Backend는 `FactoryController`를 통해 명령을 받고 DTO Snapshot을 통해 현재 상태를 외부에 전달합니다.

발표는 전체 구조, 핵심 시뮬레이션 구조, tick/update 흐름, 제품 도메인, 그리고 State, Observer, Memento 패턴 순서로 진행하겠습니다.

---

## 2. Overall Class Diagram

먼저 Backend 전체 클래스 구조를 보겠습니다.

Backend는 크게 `controller`, `dto`, `common`, `model` 영역으로 나누어져 있습니다.

외부 요청은 `FactoryController`로 들어옵니다. `FactoryController`는 사용자의 명령을 검증하고, 내부의 `Factory` 모델을 조작한 뒤, 결과를 `FactorySnapshot` 같은 DTO 형태로 반환합니다.

DTO 영역은 외부에 전달되는 읽기 전용 상태 객체입니다. 예를 들어 `FactorySnapshot`은 전체 공장 상태를 나타내고, 그 안에는 재고, 생산라인, 기계, 이벤트, 통계 정보가 포함됩니다. 중요한 점은 DTO가 `Factory`, `Machine`, `Inventory` 같은 Model 객체를 직접 노출하지 않고, 필요한 값을 복사해서 전달한다는 점입니다.

`common` 영역에는 공통 타입과 시뮬레이션 시간 관련 클래스가 있습니다. `Types.hpp`에는 `ProductId`, `MachineId`, `LineId`, `MachineStatus`, `EventType` 같은 공통 정의가 들어 있고, `SimClock`은 시간, 배속, 일시정지 상태를 관리합니다.

가장 큰 영역인 `model`은 실제 시뮬레이션 로직을 담당합니다. 중심에는 `Factory`가 있고, `Factory`는 `Inventory`, 여러 `ProductionLine`, `EventBus`, Observer, `SimClock`을 관리합니다.

또한 현재 구현에는 config 기반 생성 구조도 포함되어 있습니다. `FactoryConfig`, `DefinitionRegistry`, `FactoryBuilder`, `StationFactory`를 통해 JSON 설정에서 제품, 기계, 생산라인, 시나리오를 구성할 수 있습니다.

마지막으로 Memento 관련 구조가 있습니다. `FactoryMemento` 계층은 checkpoint와 undo를 위해 공장의 내부 상태를 저장하고, `SimulationHistory`는 이 Memento들을 stack 형태로 보관합니다.

정리하면, Overall Diagram은 Backend가 `FactoryController`를 경계로 외부 요청을 받고, `Factory`를 중심으로 시뮬레이션 상태를 관리하며, DTO, Event, Config, Memento 구조를 분리해서 사용하는 모습을 보여줍니다.

---

## 3. Core Simulation Class Diagram

다음은 실제 시뮬레이션의 중심 구조입니다.

`Factory`는 aggregate root 역할을 합니다. 즉, 공장 내부의 시간, 재고, 생산라인, 이벤트 시스템을 한 곳에서 관리하는 최상위 객체입니다.

`Factory`는 `SimClock`, `Inventory`, 여러 `ProductionLine`, `EventBus`, `EventLogObserver`, `StatisticsObserver`를 소유합니다. 그리고 생산라인 안에 들어 있는 기계들을 빠르게 찾기 위해 non-owning `Machine*` 캐시도 유지합니다.

`ProductionLine`은 실제 생산이 일어나는 단위입니다. 내부에는 대기 중인 `ProductionTask` 큐와 공정을 처리하는 `Machine` 목록이 있습니다. 생산 요청이 들어오면 제품 작업이 queue에 쌓이고, 라인은 현재 공정에 맞는 idle 상태의 기계에 작업을 배정합니다.

`ProductionTask`는 제품 하나의 생산 진행 상태입니다. 어떤 `Product`를 만들고 있는지, 현재 몇 번째 `ProcessStep`까지 진행했는지, 현재 step의 input이 소비되었는지 등을 관리합니다.

`Machine`은 실제 공정을 처리하는 추상 기반 클래스입니다. 현재 구현의 기본 기계 타입은 `MixingStation`, `QualityStation`, `BottlingStation`, `PackagingStation`이고, config 기반으로 생성되는 `ConfiguredStation`도 있습니다.

기계는 `MachineState`를 소유하고, `update()`가 호출되면 현재 상태 객체에게 동작을 위임합니다. 그래서 생산라인이나 공장 update 루프는 구체 기계 타입을 일일이 검사하지 않고, `Machine` 인터페이스를 통해 다형적으로 동작합니다.

이 구조에서 중요한 점은 소유 관계입니다. `Factory`는 `ProductionLine`을 소유하고, `ProductionLine`은 `unique_ptr<Machine>`으로 기계들을 소유합니다. 반면 `ProductionTask`는 queue와 machine 사이에서 공유될 수 있으므로 `shared_ptr` 기반으로 연결됩니다.

---

## 4. Tick / Update Sequence Diagram

이제 한 번의 시뮬레이션 tick이 어떤 순서로 진행되는지 보겠습니다.

외부에서 `FactoryController::tick(deltaTime)`을 호출하면, Controller는 직접 생산 로직을 수행하지 않고 내부 `Factory::update(deltaTime)`을 호출합니다.

`Factory::update()`가 시작되면 먼저 `SimClock`을 갱신합니다. `SimClock`은 전달받은 실제 시간을 시뮬레이션 시간으로 변환하고, pause나 stop 상태라면 시간이 진행되지 않도록 제어합니다.

그다음 `Factory`는 각 `ProductionLine`에 대해 작업 배정을 시도합니다. 작업 queue에 대기 중인 `ProductionTask`가 있고, 해당 step을 처리할 수 있는 idle 기계가 있으면 그 작업을 기계에 배정합니다. 배정된 기계는 `Working` 상태로 전환됩니다.

이후 기계들의 `update()`가 호출됩니다. 기계는 현재 상태에 따라 동작합니다. `WorkingState`라면 생산 진행률을 올리고, `BrokenState`라면 생산을 멈추며, `MaintenanceState`라면 수리 시간을 진행합니다.

공정이 완료되면 `ProductionTask`는 다음 step으로 이동합니다. 모든 step이 끝나면 제품 생산이 완료되고, `Factory`는 완제품을 `Inventory`에 반영합니다. 필요한 경우 작업 시작, step 완료, 제품 완료, 기계 고장, 수리 완료 같은 이벤트도 `EventBus`를 통해 발행됩니다.

마지막으로 `Factory`는 다시 작업 배정을 시도합니다. 방금 작업을 끝낸 기계가 idle 상태가 되었을 수 있기 때문에, 다음 tick까지 기다리지 않고 이어서 다음 작업을 배정하기 위한 구조입니다.

전체 흐름은 시간 갱신, 작업 배정, 기계 update, 공정 진행, 재고 반영, 이벤트 발행, 재배정 순서로 이해할 수 있습니다.

---

## 5. Product Domain Class Diagram

다음은 제품 도메인 구조입니다.

제품 도메인의 핵심은 `ProductCatalog`, `ProductDefinition`, `Product`, `Recipe`, `ProcessStep`, `ProductionTask`, `Inventory`입니다.

`ProductCatalog`는 제품 정의를 모아두는 카탈로그 역할을 합니다. 기본 제품으로는 `VoltzClassic`, `HyperBolt`, `AuroraZero`가 있으며, 각 제품은 ID, 이름, 기본 recipe id, 필요한 재료, 공정 route를 가집니다.

제품의 생산 route는 여러 `ProcessStep`으로 구성됩니다. 각 step은 어떤 종류의 공정인지, 처리 시간이 얼마인지, step 단위 input과 output이 무엇인지 정의합니다. 예를 들어 Mixing, Quality, Bottling, Packaging 같은 흐름으로 제품이 이동합니다.

`Recipe`와 config 영역은 JSON 기반 제품 정의와 연결됩니다. `FactoryConfig` 안에는 item, product, station, recipe, line, scenario 정의가 들어 있고, `DefinitionRegistry`가 이를 id 기준으로 검증하고 찾아줍니다.

`ProductionTask`는 실제 생산 중인 제품 작업입니다. 하나의 Product를 참조하고, `currentStepIndex`를 통해 현재 어느 공정까지 진행했는지를 관리합니다. 이를 통해 현재 작업이 어떤 역할의 `Machine`에 배정되어야 하는지 판단할 수 있습니다.

`Inventory`는 실제 Product 객체를 저장하지 않고 수량만 관리합니다. 원자재는 item id 또는 legacy `ItemType` 기준으로, 완제품은 `ProductId` 기준으로 관리합니다. 공정에서 input이 필요하면 소비하고, output이 발생하면 재고에 반영합니다.

정리하면, 제품 도메인은 제품을 데이터로 정의하고, 그 정의를 기반으로 `ProductionTask`가 공정 route를 따라 생산라인을 이동하도록 만든 구조입니다.

---

## 6. Machine State Diagram

이제 Machine의 상태 관리 구조를 보겠습니다.

Machine은 State Pattern을 사용합니다. 즉, 상태를 단순 enum 값으로만 두지 않고, 상태별 행동을 `MachineState` 하위 클래스에 분리했습니다.

현재 상태 클래스는 `IdleState`, `WorkingState`, `BrokenState`, `MaintenanceState`입니다.

`IdleState`는 기계가 작업을 받을 수 있는 대기 상태입니다. 작업이 배정되면 `WorkingState`로 전환됩니다.

`WorkingState`는 실제 생산을 진행하는 상태입니다. 이 상태에서는 현재 배정된 task의 진행률이 증가하고, 공정 완료 조건을 만족하면 task가 다음 step으로 넘어갑니다.

`BrokenState`는 기계가 고장난 상태입니다. 이 상태에서는 생산이 진행되지 않습니다. 사용자는 repair 명령을 통해 기계를 수리 상태로 보낼 수 있습니다.

`MaintenanceState`는 수리 중인 상태입니다. 수리 시간이 진행되고, 조건이 충족되면 기계 체력이 회복되어 다시 idle 또는 작업 재개 상태로 돌아갑니다.

여기서 `MachineStatus` enum은 외부 표시와 저장을 위한 값입니다. DTO Snapshot에서 기계 상태를 보여주거나, Memento로 상태를 저장하고 복원할 때 사용됩니다. 반면 실제 update 행동은 `MachineState` 객체가 담당합니다.

이 설계 덕분에 `Machine` 클래스 안에 거대한 상태별 if문을 만들지 않아도 되고, 상태별 행동을 독립적으로 설명하고 확장할 수 있습니다.

---

## 7. Observer / Event Pattern Diagram

다음은 Observer와 Event 구조입니다.

생산 과정에서 발생한 이벤트는 `EventBus`를 통해 전달되고, 여러 Observer가 그 이벤트를 받아 각자의 역할을 수행합니다.

이벤트를 발생시키는 주체는 주로 `ProductionLine`과 `Machine`입니다. 예를 들어 작업이 queue에 들어갔을 때, 작업이 시작되었을 때, step이 완료되었을 때, 제품이 완성되었을 때, 기계가 고장났거나 수리되었을 때 이벤트가 발생합니다.

중요한 점은 생산 로직이 로그나 통계를 직접 수정하지 않는다는 것입니다. `Machine`이나 `ProductionLine`은 `Event`를 만들고 `EventBus::publish()`를 호출할 뿐입니다.

`EventBus`는 등록된 `Observer*` 목록을 가지고 있다가, 이벤트가 발행되면 각 Observer의 `onEvent()`를 호출합니다.

대표적인 Observer는 `EventLogObserver`와 `StatisticsObserver`입니다. `EventLogObserver`는 이벤트를 저장해서 이벤트 로그를 만들고, `StatisticsObserver`는 이벤트 타입을 기준으로 완료 제품 수, 시작된 작업 수, 완료된 step 수, 고장 횟수, 수리 횟수 같은 통계를 갱신합니다.

`Factory`는 `EventBus`와 Observer들을 소유하고 연결합니다. 그래서 생산 도중 발생한 이벤트들이 자동으로 로그와 통계에 반영됩니다.

이 구조의 장점은 생산 로직과 부가 기능이 분리된다는 점입니다. 나중에 알림 기능이나 디버깅 모니터링 기능이 필요해도, 기존 생산 로직을 크게 바꾸지 않고 새로운 Observer를 추가하는 방식으로 확장할 수 있습니다.

---

## 8. Memento Pattern Diagram

다음은 Memento Pattern 구조입니다.

이 프로젝트는 checkpoint와 undo 기능을 위해 Memento Pattern을 사용합니다. 핵심 역할은 Originator, Memento, Caretaker입니다.

Originator는 `Factory`입니다. `Factory`는 현재 시뮬레이션 상태를 `FactoryMemento`로 저장할 수 있고, 저장된 `FactoryMemento`를 받아 자신의 상태를 복원할 수 있습니다.

Memento 역할은 `FactoryMemento`, `LineMemento`, `MachineMemento`, `ProductionTaskMemento`, `StepOutputMemento`가 담당합니다.

`FactoryMemento`는 시뮬레이션 시간, 재고, 생산라인 상태를 저장합니다. `LineMemento`는 line id, queue, scenario, queue capacity, task 정보를 저장합니다. `MachineMemento`는 기계 id, health, status, progress, station metadata, 배정된 task index 등을 저장합니다.

이 Memento들은 public field 구조체가 아니라 private field와 getter를 가진 class입니다. 즉, 외부에서 snapshot 내부 값을 직접 바꾸는 방식은 막고, 복원 로직이 필요한 값만 읽을 수 있게 했습니다.

Caretaker는 `SimulationHistory`입니다. `SimulationHistory`는 `FactoryMemento`들을 stack 형태로 보관하고, checkpoint 저장과 undo 기능을 지원합니다. 중요한 점은 `SimulationHistory`가 memento 내용을 해석하지 않는다는 것입니다. 실제 상태를 어떻게 복원할지는 Originator인 `Factory`가 담당합니다.

외부에서 이 기능을 사용하는 진입점은 `FactoryController`입니다. `saveCheckpoint()`, `undo()`, `canUndo()`, `historySize()` 같은 메서드를 통해 외부 계층은 Model 내부를 직접 조작하지 않고도 상태 저장과 복원을 사용할 수 있습니다.

DTO Snapshot과 Memento도 분리되어 있습니다. DTO는 화면이나 API에 현재 상태를 보여주기 위한 값 객체이고, Memento는 내부 상태를 복원하기 위한 저장 객체입니다. 따라서 undo에는 `FactorySnapshot`이 아니라 `FactoryMemento` 계층을 사용합니다.

---

## 9. Config 기반 확장 구조

현재 Backend는 hard-coded 기본 공장뿐 아니라 config 기반 공장 생성도 지원합니다.

`FactoryConfig`는 JSON에서 읽어온 설정 데이터입니다. 여기에는 factory settings, items, products, stations, recipes, production lines, initial inventory, startup tasks, scenarios가 들어 있습니다.

`FactoryConfigLoader`는 JSON 파일이나 문자열을 읽어 `FactoryConfig`로 변환합니다. `DefinitionRegistry`는 각 정의를 id 기준으로 인덱싱하고, station이나 recipe가 서로 올바르게 연결되어 있는지 검증합니다.

`FactoryBuilder`는 검증된 config를 실제 runtime 객체로 바꿉니다. 생산라인을 만들고, `StationFactory`를 통해 기계를 생성하고, 초기 재고와 시작 작업을 세팅합니다.

이 구조 덕분에 새 제품이나 새 생산라인을 추가할 때 모든 C++ update 루프를 수정할 필요가 줄어듭니다. 설정 데이터와 builder 경로를 통해 공장 구성을 확장할 수 있습니다.

---

## 10. 설계 장점과 마무리

마지막으로 설계 장점을 정리하겠습니다.

첫째, Backend는 `FactoryController`와 DTO를 통해 외부 계층과 Model을 분리했습니다. UI는 Model 객체를 직접 만지지 않고 명령과 snapshot만 사용합니다.

둘째, `Factory`, `ProductionLine`, `Machine`, `ProductionTask`, `Inventory`가 각자의 책임을 나누고 있습니다. 공장은 전체 상태를 관리하고, 라인은 작업 배정을 담당하며, 기계는 자신의 상태에 따라 공정을 처리합니다.

셋째, State Pattern으로 기계 상태 행동을 분리했고, Observer Pattern으로 이벤트 로그와 통계를 생산 로직에서 분리했으며, Memento Pattern으로 checkpoint와 undo 책임을 분리했습니다.

넷째, config 기반 구조를 통해 제품, station, recipe, line, scenario를 데이터로 확장할 수 있게 했습니다.

결과적으로 Gactorio Backend는 생산 로직, 상태 관리, 이벤트 처리, 복원 기능, 설정 기반 확장이 분리되어 유지보수와 확장이 쉬운 구조가 되었습니다.

이상으로 Backend 발표를 마치겠습니다. 감사합니다.
