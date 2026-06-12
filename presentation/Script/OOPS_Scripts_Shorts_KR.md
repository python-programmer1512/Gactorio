# JP 객지프 발표 대본 축약본

조원 : 고원규, 조용빈
최종 수정 일자 : 26.06.13

---

# 1. 프로젝트와 BE 범위

안녕하세요. 지금부터 Gactorio Project의 Backend UML 구조에 대해 발표하겠습니다.

Gactorio는 음료 생산 공장을 시뮬레이션하는 C++ 프로젝트입니다. Backend는 생산라인, 기계, 제품, 재고, 시뮬레이션 시간, 이벤트 로그, 통계, 그리고 undo 기능을 관리합니다.

---

# 2. Overall Class Diagram

전체 클래스 구조를 보겠습니다.

Backend는 Controller, DTO, Common, Model 영역으로 나누었습니다. 
외부 요청이 FactoryController로 들어오고, FactoryController가 Model을 조작한 뒤, 그 결과를 DTO Snapshot 형태로 반환합니다.

FactoryController는 Backend의 진입점입니다. 
사용자가 시뮬레이션을 진행하거나, 제품을 추가하거나, checkpoint와 undo 기능을 사용할 때 접근하는 클래스입니다. 
내부적으로 Factory를 소유하고, undo 기능을 위해 SimulationHistory도 관리합니다.

DTO 영역은 외부에 전달되는 읽기 전용 상태 객체입니다. 
FactorySnapshot은 전체 공장 상태를 대표하며, 재고, 생산라인, 기계, 이벤트, 통계 정보를 포함합니다. 
DTO는 Model 객체를 직접 노출하지 않고, 현재 상태를 값으로 복사해 전달합니다.

Common 영역에는 SimClock과 공통 타입이 위치합니다. 
SimClock은 시간, 배속, 일시정지 상태를 관리하고, 
Types.hpp에는 ProductId, MachineId, ItemType, MachineStatus, EventType 같은 공통 정의가 들어 있습니다.

Model 영역은 실제 시뮬레이션 로직을 담당합니다. 
중심에는 Factory가 있고, Factory는 Inventory, ProductionLine, EventBus, Observer, SimClock을 관리합니다. 
ProductionLine은 작업 큐와 기계들을 가지고 실제 생산을 수행하고, Machine은 각 공정 단계를 처리합니다.

Memento 영역은 checkpoint와 undo를 위해 공장의 내부 상태를 저장합니다. 
FactoryMemento는 상태를 저장하고, SimulationHistory는 이를 stack 형태로 보관합니다.

---

# 3. Core Simulation Class Diagram

실제 시뮬레이션의 중심 구조를 보겠습니다.

Factory는 aggregate root 역할을 하며, 시간, 재고, 생산라인, 이벤트 시스템을 한 곳에서 관리합니다.

Factory는 SimClock, Inventory, 여러 ProductionLine, EventBus를 관리합니다. 
SimClock은 시뮬레이션 시간과 배속을 담당하고, Inventory는 원자재와 완제품 수량을 관리합니다. 
ProductionLine은 실제 생산이 일어나는 단위이며, EventBus는 생산 중 발생한 이벤트를 전달합니다.

ProductionLine은 대기 중인 ProductionTask 큐와 실제 공정을 처리하는 Machine 목록을 가집니다. 
어떤 제품 작업이 기다리고 있는지 관리하고, 그 작업을 처리할 수 있는 기계에 배정합니다.

ProductionTask는 제품 하나의 생산 진행 상태를 나타냅니다. 
특정 Product를 참조하고, 현재 몇 번째 공정 단계까지 진행했는지를 currentStepIndex로 관리합니다. 
제품마다 필요한 공정 순서가 다르기 때문에, Product가 가진 ProcessStep 목록을 따라 현재 필요한 공정을 확인합니다.

Machine은 실제 공정을 처리하는 추상 클래스입니다. 
구체 기계들은 Machine을 상속하고, 각자 처리할 수 있는 역할에 따라 작업을 수행합니다. 
작업이 배정되면 Machine은 현재 상태에 따라 생산을 진행하고, 공정이 완료되면 다음 단계로 넘기거나 제품 완료를 알립니다.

여기서 Machine의 상태별 동작은 MachineState 객체로 분리되어 있습니다. 
Machine은 Idle, Working, Broken, Maintenance 같은 상태를 가지고, 
실제 상태별 update 동작은 각 State 객체에게 위임합니다.

---

# 4. Tick/Update Sequence Diagram

시뮬레이션이 실제 실행 중에 어떤 순서로 동작하는지 살펴보겠습니다.

이 다이어그램은 FactoryController::tick(deltaTime)이 호출되었을 때, 한 번의 update가 어떻게 진행되는지를 보여줍니다.

먼저 외부에서 FactoryController의 tick()을 호출합니다. 
FactoryController는 직접 생산 로직을 처리하지 않고, 내부 Factory의 update()를 호출합니다.

Factory::update()가 시작되면 가장 먼저 SimClock을 갱신합니다. 
SimClock은 전달받은 deltaTime을 시뮬레이션 시간 기준으로 반영하고, pause나 stop 상태라면 시간이 진행되지 않도록 제어합니다.

그다음 Factory는 각 ProductionLine에 대해 assignAvailableTask()를 호출합니다. 
작업 큐에 대기 중인 ProductionTask가 있고, 해당 공정을 처리할 수 있는 idle 상태의 Machine이 있다면 작업을 배정합니다. 
배정된 Machine은 Working 상태로 전환됩니다.

이후 Factory는 Machine 목록을 순회하면서 각 Machine의 update()를 호출합니다. 
Machine은 자신의 현재 상태에 따라 동작하며, 상태별 처리는 MachineState 객체에게 위임됩니다. 
Working 상태라면 생산 진행률을 증가시키고, Maintenance 상태라면 수리를 진행하며, Broken 상태라면 생산을 멈춘 상태로 유지됩니다.

공정이 완료되면 ProductionTask는 다음 step으로 이동합니다. 
모든 공정이 끝나면 제품 생산이 완료되고, Factory는 Inventory::addProduct()를 호출해 완제품 재고를 증가시킵니다. 
필요한 경우 제품 완료나 기계 상태 변경 이벤트도 EventBus를 통해 발행됩니다.

마지막으로 Factory는 다시 assignAvailableTask()를 호출합니다. 
방금 작업을 끝낸 기계가 Idle 상태가 되었을 수 있기 때문에, 다음 tick까지 기다리지 않고 바로 다음 작업을 배정하기 위한 구조입니다.

---

# 5. Product Domain Class Diagram

제품 도메인 구조를 보겠습니다.

이 다이어그램의 중심에는 ProductCatalog, ProductDefinition, Product, Inventory, ProductionTask가 있습니다.

ProductCatalog는 제품 정의를 모아두는 카탈로그 역할을 합니다. 
각 제품의 ID, 이름, 타입, 필요한 재료, 공정 경로가 ProductDefinition 형태로 저장됩니다. 
제품 하나를 만들기 위한 설계도라고 볼 수 있습니다.

ProductDefinition에는 두 가지 핵심 정보가 있습니다. 
첫 번째는 ItemRequirement입니다. 
이는 제품 하나를 만들기 위해 어떤 재료가 몇 개 필요한지를 나타냅니다. 
두 번째는 ProcessStep입니다. 
이는 제품이 어떤 공정 순서로 생산되는지를 나타내며, 
각 step에는 필요한 기계 역할과 기준 처리 시간이 들어 있습니다.

ProductCatalog는 ProductDefinition을 바탕으로 실제 Product 객체를 생성합니다. 
생산라인이나 기계는 구체 제품 구현을 직접 알 필요 없이, 
Product가 제공하는 ID, 이름, 요구 재료, 공정 경로만 사용합니다.

ProductionTask는 실제 생산 중인 제품 작업입니다. 
하나의 Product를 참조하고, currentStepIndex를 통해 현재 몇 번째 공정까지 진행했는지를 관리합니다. 
이를 통해 현재 작업이 어떤 역할의 Machine에 배정되어야 하는지 판단할 수 있습니다.

Inventory는 실제 Product 객체를 저장하지 않고, 원자재는 ItemType 기준으로, 완제품은 ProductId 기준으로 수량만 저장합니다. 
생산 요청이 들어오면 Factory는 Product의 요구 재료를 확인하고 Inventory에서 재료를 소비합니다. 
제품이 완성되면 ProductId 기준으로 완제품 수량을 증가시킵니다.

---

# 6. Machine State Diagram

Machine의 상태 관리 구조를 보겠습니다.

Machine은 State Pattern을 이용해 상태별 행동을 별도의 클래스로 분리했습니다.

Machine은 Idle, Working, Broken, Maintenance 네 가지 상태를 가집니다.

Machine은 현재 상태를 나타내는 MachineState 객체를 가지고 있고, update가 호출되면 실제 동작을 이 객체에게 위임합니다.

반면 MachineStatus enum은 외부 표시와 저장을 위한 상태값으로 사용됩니다. DTO Snapshot에서 기계 상태를 보여주거나, Memento로 저장하고 복원할 때 MachineStatus 값이 필요합니다.

기계의 상태별 동작을 별도 클래스로 분리했기 때문에 
Machine 클래스가 모든 상태 로직을 직접 처리하지 않아도 되고, 
새로운 상태나 동작을 추가할 때도 구조를 명확하게 유지할 수 있습니다.

---

# 7. Observer / Event Pattern Diagram

Observer / Event 구조를 보겠습니다.

생산 과정에서 발생한 이벤트를 EventBus가 전달하고, 
여러 Observer들이 그 이벤트를 받아 각자의 역할을 수행합니다.

이벤트를 발생시키는 주체는 주로 ProductionLine과 Machine입니다. 
ProductionLine은 작업이 큐에 들어갔을 때 이벤트를 발생시킬 수 있고, 
Machine은 작업 시작, 공정 완료, 제품 완료, 기계 고장, 수리 완료 같은 이벤트를 발생시킬 수 있습니다.

이때 ProductionLine이나 Machine이 로그나 통계를 직접 수정하지는 않습니다. 
대신 Event 객체를 만들고, EventBus::publish()를 통해 이벤트를 발행합니다. 
Event에는 발생 시간, 이벤트 타입, 관련 기계 ID, 메시지 같은 정보가 담깁니다.

EventBus는 등록된 Observer 목록을 가지고 있다가, 
이벤트가 발행되면 각 Observer의 onEvent()를 호출합니다.

대표적인 Observer는 EventLogObserver와 StatisticsObserver입니다. 
EventLogObserver는 전달받은 이벤트를 내부 목록에 저장하여 이벤트 로그 역할을 합니다. 
StatisticsObserver는 이벤트 타입을 기준으로 완료 제품 수나 기계 고장 횟수 같은 통계를 갱신합니다.

Factory는 EventBus와 Observer들을 관리합니다. 
EventLogObserver와 StatisticsObserver를 EventBus에 등록하고, 
생산 중 발생한 이벤트들이 두 Observer에게 전달되도록 합니다.

생산 로직과 로그·통계 기능을 분리하였기 때문에 알림 기능이나 모니터링 기능을 추가할 때도 
새로운 Observer를 추가하는 방식으로 확장할 수 있습니다.

---

# 8. Memento Pattern Diagram

Memento Pattern 구조를 보겠습니다.

시뮬레이션 상태를 checkpoint로 저장하고, 필요할 때 이전 상태로 되돌릴 수 있도록 역할을 분리했습니다.

Memento Pattern에서 중요한 역할은 Originator, Memento, Caretaker입니다.

이 프로젝트에서 Originator는 Factory입니다. 
Factory는 현재 시뮬레이션 상태를 FactoryMemento로 저장할 수 있고, 
FactoryMemento를 받아 자신의 상태를 복원할 수 있습니다.

Memento 역할은 FactoryMemento, LineMemento, MachineMemento가 담당합니다. 
FactoryMemento는 전체 공장 상태를 저장하고, LineMemento는 생산라인의 작업 큐 정보를 저장하며, 
MachineMemento는 개별 기계의 상태와 체력 같은 정보를 저장합니다.

Caretaker 역할은 SimulationHistory가 담당합니다. 
SimulationHistory는 FactoryMemento들을 stack 형태로 보관하고, checkpoint 저장과 undo 기능을 지원합니다. 
Factory 내부 구조를 직접 수정하지는 않습니다.

외부에서 이 기능을 사용하는 진입점은 FactoryController입니다. 
FactoryController는 saveCheckpoint(), undo(), canUndo(), historySize() 같은 메서드를 통해 checkpoint와 undo 기능을 제공합니다.

저장 흐름은 FactoryController::saveCheckpoint()가 호출되면, 
Factory가 현재 시간, 재고, 생산라인 queue, 기계 상태 등 복원에 필요한 정보를 모아 FactoryMemento를 생성하고, 
SimulationHistory가 이를 stack에 저장하는 방식입니다.

undo 흐름은 반대로 진행됩니다. 
FactoryController::undo()가 호출되면 SimulationHistory에서 가장 최근 checkpoint를 꺼내고, Factory::restoreFromMemento()에 전달합니다. 
Factory는 저장된 시간, 재고, 생산라인 상태, 기계 상태를 기준으로 내부 상태를 복원합니다.

DTO Snapshot과 Memento가 분리되어 있습니다. 
DTO Snapshot은 화면이나 API에 현재 상태를 보여주기 위한 값 객체이고, Memento는 내부 상태를 복원하기 위한 저장 객체입니다.
이를 통해 외부 계층은 Model 내부를 직접 조작하지 않고도 시뮬레이션 상태를 이전 시점으로 되돌릴 수 있습니다.

---

# 9. 설계 장점과 마무리

이 프로젝트는 Factory를 중심으로 시뮬레이션 상태를 관리하고, FactoryController와 DTO를 통해 외부 계층과 Model을 분리했습니다.

또한 Machine 상태 관리는 State Pattern, 이벤트 로그와 통계 처리는 Observer Pattern, checkpoint와 undo 기능은 Memento Pattern으로 구현했습니다.

결과적으로 생산 로직, 상태 관리, 이벤트 처리, 복원 기능의 책임이 분리되어 유지보수와 확장이 쉬운 구조가 되었습니다.

이상으로 발표를 마치겠습니다. 감사합니다.
