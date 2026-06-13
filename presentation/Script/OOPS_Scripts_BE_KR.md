# JP 객지프 발표 대본
조원 : 고원규, 조용빈
최종 수정 일자 : 26.06.13

# 1. 프로젝트와 BE 범위

안녕하세요. 지금부터 Gactorio Project의 Backend UML 구조에 대해 발표하겠습니다.

Gactorio는 음료 생산 공장을 시뮬레이션하는 C++ 프로젝트입니다.
이 프로젝트의 Backend는 생산라인, 기계, 제품, 재고, 시뮬레이션 시간, 이벤트 로그, 통계, 그리고 undo 기능을 관리합니다.

이번 발표의 UML 분석 범위는 Backend 구조입니다.
구체적으로는 Factory, ProductionLine, Machine, Product, Inventory 같은 Model 계층과, 외부 요청을 처리하는 FactoryController, 화면이나 API에 전달되는 DTO Snapshot, 그리고 Event, Observer, Memento 관련 구조를 포함합니다.

발표는 먼저 전체 클래스 다이어그램을 통해 Backend의 큰 구조를 살펴보겠습니다.
그다음 실제 시뮬레이션의 핵심 구조와 tick/update 흐름을 설명하고, 이후 제품 도메인과 주요 설계 패턴을 순서대로 보겠습니다.

특히 이 프로젝트에서 중요한 설계 포인트는 세 가지입니다.
Machine 상태 관리를 위한 State Pattern, 이벤트 로그와 통계 처리를 위한 Observer Pattern, 그리고 checkpoint와 undo 기능을 위한 Memento Pattern입니다.

즉, 이번 발표에서는 Gactorio Backend가 어떤 클래스들로 구성되어 있고, 이 클래스들이 시뮬레이션 과정에서 어떻게 협력하는지를 UML을 중심으로 설명하겠습니다.


# 2. Overall Class Diagram

이번 장에서는 Gactorio Backend의 전체 클래스 구조를 먼저 보겠습니다.

이 다이어그램은 Backend를 크게 Controller, DTO, Common, Model 영역으로 나누어 보여줍니다.
전체 흐름에서 가장 중요한 점은 외부 요청이 FactoryController로 들어오고, FactoryController가 Model을 조작한 뒤, 그 결과를 DTO Snapshot 형태로 외부에 반환한다는 것입니다.

먼저 Controller 영역의 FactoryController는 Backend의 진입점 역할을 합니다.
사용자가 시뮬레이션을 진행하거나, 제품을 추가하거나, checkpoint와 undo 기능을 사용할 때 직접 접근하는 클래스입니다.
FactoryController는 내부적으로 Factory를 소유하고, undo 기능을 위해 SimulationHistory도 함께 관리합니다.

DTO 영역은 외부에 전달되는 읽기 전용 상태 객체들입니다.
FactorySnapshot은 전체 공장 상태를 대표하고, 그 안에 재고, 생산라인, 기계, 이벤트, 통계 정보가 포함됩니다.
중요한 점은 DTO가 Model 객체를 직접 노출하지 않고, 현재 상태를 값으로 복사해서 전달한다는 것입니다.

Common 영역에는 시뮬레이션 시간과 공통 타입이 위치합니다.
SimClock은 시뮬레이션 시간, 배속, 일시정지 상태를 관리하고, Types.hpp에는 제품 ID, 기계 ID, 아이템 타입, 기계 상태, 이벤트 타입 같은 공통 정의가 들어 있습니다.

가장 큰 Model 영역은 실제 시뮬레이션 로직을 담당합니다.
중심에는 Factory가 있고, Factory는 Inventory, ProductionLine, EventBus, Observer, SimClock을 관리합니다.
ProductionLine은 작업 큐와 기계들을 가지고 실제 생산을 수행하고, Machine은 각 공정 단계를 처리합니다.

오른쪽에는 Memento 관련 구조가 배치되어 있습니다.
FactoryMemento 계층은 checkpoint와 undo를 위해 공장의 내부 상태를 저장하고, SimulationHistory는 이 Memento들을 stack 형태로 보관합니다.
현재 Memento 객체들은 단순 공개 struct가 아니라 private field와 getter를 가진 class로 구성해서, 저장된 상태 값을 외부에서 직접 수정하지 않도록 했습니다.

정리하면, 이 전체 클래스 다이어그램은 Gactorio Backend가 FactoryController를 경계로 외부 요청을 받고, Factory를 중심으로 시뮬레이션 상태를 관리하며, DTO, Event, Memento 구조를 분리해서 사용하는 구조임을 보여줍니다.


# 3. Core Simulation Class Diagram

이번 장에서는 실제 시뮬레이션의 중심이 되는 Core Simulation 구조를 보겠습니다.

이 다이어그램의 핵심은 Factory를 중심으로 생산라인, 기계, 작업, 제품, 재고가 어떻게 연결되는지 보여주는 것입니다.
Gactorio Backend에서 Factory는 시뮬레이션의 aggregate root 역할을 합니다. 즉, 공장 내부의 시간, 재고, 생산라인, 이벤트 시스템을 한 곳에서 관리하는 중심 객체입니다.

먼저 Factory는 SimClock, Inventory, 여러 ProductionLine, 그리고 EventBus를 관리합니다.
SimClock은 시뮬레이션 시간과 배속, 일시정지 상태를 담당하고, Inventory는 원자재와 완제품 수량을 관리합니다.
ProductionLine은 실제 생산이 일어나는 단위이고, EventBus는 생산 중 발생한 이벤트를 전달하는 역할을 합니다.

ProductionLine은 크게 두 가지 요소를 가집니다.
하나는 대기 중인 작업을 저장하는 ProductionTask 큐이고, 다른 하나는 실제 공정을 처리하는 Machine 목록입니다.
즉, 생산라인은 어떤 제품 작업이 기다리고 있는지 관리하고, 그 작업을 처리할 수 있는 기계에 배정합니다.

ProductionTask는 제품 하나의 생산 진행 상태를 나타냅니다.
각 작업은 특정 Product를 참조하고, 현재 몇 번째 공정 단계까지 진행했는지를 currentStepIndex로 관리합니다.
제품마다 필요한 공정 순서가 다르기 때문에, ProductionTask는 Product가 가진 ProcessStep 목록을 따라 현재 필요한 공정을 확인합니다.

Machine은 실제 공정을 처리하는 추상 기반 클래스입니다.
구체 기계들은 Machine을 상속하고, 각자 처리할 수 있는 역할에 따라 작업을 수행합니다.
작업이 배정되면 Machine은 현재 상태에 따라 생산을 진행하고, 공정이 완료되면 다음 단계로 넘기거나 제품 완료를 알립니다.

여기서 중요한 점은 Machine의 상태별 동작이 MachineState 객체로 분리되어 있다는 것입니다.
Machine은 Idle, Working, Broken, Maintenance 같은 상태를 가지고, 실제 상태별 update 동작은 각 State 객체에게 위임합니다.
이 부분은 뒤의 State Pattern 장에서 더 자세히 설명하겠습니다.

전체 생산 흐름을 간단히 정리하면 다음과 같습니다.
Factory가 생산라인과 재고를 관리하고, ProductionLine이 작업을 기계에 배정합니다.
Machine은 자신의 상태와 역할에 따라 공정을 처리하고, 완료된 제품은 다시 Inventory에 반영됩니다.

이 다이어그램에서 강조할 점은 소유 관계입니다.
Factory는 ProductionLine을 소유하고, ProductionLine은 Machine들을 소유합니다.
반면 작업은 큐와 기계 사이에서 공유될 수 있으므로 shared pointer 기반으로 연결됩니다.

정리하면, Core Simulation 구조는 Gactorio Backend에서 실제 생산 로직이 어떤 객체들의 협력으로 이루어지는지를 보여주는 핵심 다이어그램입니다.


# 4. Tick/Update Sequence Diagram

이번 장에서는 앞에서 본 Core Simulation 구조가 실제 실행 중에 어떤 순서로 동작하는지 살펴보겠습니다.

이 다이어그램은 FactoryController::tick(deltaTime)이 호출되었을 때, 한 번의 시뮬레이션 update가 어떻게 진행되는지를 보여줍니다.
즉, 앞 장의 Class Diagram이 “어떤 객체들이 연결되어 있는가”를 보여줬다면, 이번 Sequence Diagram은 “그 객체들이 어떤 순서로 호출되는가”를 보여줍니다.

먼저 외부에서는 FactoryController의 tick()을 호출합니다.
FactoryController는 직접 생산 로직을 처리하지 않고, 내부에 있는 Factory의 update()를 호출합니다.
이처럼 Controller는 외부 명령을 Model에 전달하는 진입점 역할을 합니다.

Factory::update()가 시작되면 가장 먼저 SimClock을 갱신합니다.
SimClock은 전달받은 deltaTime을 시뮬레이션 시간 기준으로 반영하고, 현재 시간이 얼마나 진행되었는지 관리합니다.
만약 pause나 stop 상태라면 이 단계에서 시간이 진행되지 않도록 제어됩니다.

그다음 Factory는 각 ProductionLine에 대해 assignAvailableTask()를 호출합니다.
이 단계에서는 작업 큐에 대기 중인 ProductionTask가 있고, 해당 작업의 현재 공정을 처리할 수 있는 idle 상태의 Machine이 있다면 작업을 배정합니다.
작업이 배정된 Machine은 Working 상태로 전환되고, 이후 update 단계에서 실제 생산을 진행합니다.

이후 Factory는 Machine 목록을 순회하면서 각 Machine의 update()를 호출합니다.
Machine은 자신의 현재 상태에 따라 동작하는데, 이때 상태별 처리는 MachineState 객체에게 위임됩니다.
예를 들어 Working 상태라면 생산 진행률을 증가시키고, Maintenance 상태라면 수리를 진행하며, Broken 상태라면 생산을 멈춘 상태로 유지됩니다.

생산이 진행되면서 ProductionTask는 현재 ProcessStep을 확인하고, 공정이 끝나면 다음 step으로 이동합니다.
모든 공정이 완료되면 제품 생산이 완료되고, 해당 결과가 ProductionLine을 통해 Factory에 전달됩니다.

기계 업데이트가 끝난 뒤, Factory는 완료된 제품들을 수집합니다.
그리고 Inventory::addProduct()를 호출해서 완제품 재고를 증가시킵니다.
이 과정에서 필요한 경우 제품 완료 이벤트나 기계 상태 변경 이벤트도 EventBus를 통해 발행됩니다.

마지막으로 Factory는 다시 한 번 assignAvailableTask()를 호출합니다.
이렇게 하는 이유는 방금 어떤 기계가 작업을 끝내고 Idle 상태가 되었을 수 있기 때문입니다.
즉, 다음 tick까지 기다리지 않고 가능한 작업을 바로 이어서 배정할 수 있도록 하기 위한 구조입니다.

전체 흐름을 정리하면 다음과 같습니다.
FactoryController가 tick 명령을 받고, Factory가 시간을 갱신한 뒤, ProductionLine이 작업을 배정합니다.
이후 Machine들이 상태에 따라 생산을 진행하고, 완료된 제품은 Inventory에 반영됩니다.
마지막으로 새로 비어 있는 기계가 있으면 다시 작업을 배정합니다.

따라서 한 번의 tick 흐름은
시간 갱신 → 작업 배정 → 기계 update → 공정 진행 → 완료 제품 재고 반영 → 재배정
순서로 이루어집니다.

이 Sequence Diagram은 Gactorio Backend의 정적인 클래스 구조가 실제 시뮬레이션 루프 안에서 어떻게 협력하는지를 보여주는 핵심 다이어그램입니다.


# 5. Product Domain Class Diagram

이번 장에서는 제품 도메인 구조를 보겠습니다.

앞 장까지는 tick이 호출되었을 때 생산라인과 기계가 어떻게 동작하는지를 봤습니다.
이번 장에서는 그 생산 대상인 제품이 어떻게 정의되고, 그 정의가 실제 생산 흐름과 어떻게 연결되는지를 설명하겠습니다.

이 다이어그램의 중심에는 ProductCatalog, ProductDefinition, Product, Inventory, 그리고 ProductionTask가 있습니다.

먼저 ProductCatalog는 제품 정의를 모아두는 카탈로그 역할을 합니다.
각 제품의 ID, 이름, 타입, 필요한 재료, 그리고 공정 경로가 ProductDefinition 형태로 저장됩니다.
즉, ProductDefinition은 제품 하나를 만들기 위한 설계도라고 볼 수 있습니다.

ProductDefinition에서 중요한 정보는 크게 두 가지입니다.
첫 번째는 ItemRequirement입니다.
ItemRequirement는 제품 하나를 생산하기 위해 어떤 재료가 몇 개 필요한지를 나타냅니다.

두 번째는 ProcessStep입니다.
ProcessStep은 제품이 어떤 공정 순서로 생산되는지를 나타냅니다.
각 step에는 필요한 기계 역할과 기준 처리 시간이 들어 있습니다.
따라서 제품마다 어떤 기계를 어떤 순서로 거쳐야 하는지가 이 공정 경로에 정의됩니다.

ProductCatalog는 이 ProductDefinition을 바탕으로 실제 Product 객체를 생성합니다.
생산라인이나 기계는 구체 제품 구현을 직접 알 필요 없이, Product가 제공하는 ID, 이름, 요구 재료, 공정 경로만 사용합니다.

ProductionTask는 실제 생산 중인 제품 작업을 나타냅니다.
각 ProductionTask는 하나의 Product를 참조하고, 현재 몇 번째 공정까지 진행했는지를 currentStepIndex로 관리합니다.
그리고 현재 ProcessStep을 확인해서, 이 작업이 어떤 역할의 Machine에 배정되어야 하는지 판단할 수 있게 합니다.

Inventory는 재고를 관리합니다.
다만 실제 Product 객체를 저장하는 것이 아니라, 원자재는 ItemType 기준으로, 완제품은 ProductId 기준으로 수량만 저장합니다.
생산 요청이 들어오면 Factory는 Product의 요구 재료를 확인하고 Inventory에서 재료를 소비합니다.
제품이 완성되면 ProductId를 기준으로 완제품 수량을 증가시킵니다.

전체 흐름을 정리하면 다음과 같습니다.
ProductCatalog가 제품 정의를 관리하고, ProductDefinition에는 필요한 재료와 공정 경로가 들어 있습니다.
Factory는 이 정보를 바탕으로 Inventory에서 재료를 소비하고, ProductionTask는 Product의 ProcessStep을 따라 생산을 진행합니다.
마지막으로 완성된 제품은 Inventory에 반영됩니다.

따라서 이 Product Domain 구조는 제품 하나가 어떤 데이터로 정의되고, 그 데이터가 생산 작업과 재고 관리에 어떻게 사용되는지를 보여주는 다이어그램입니다.


# 6. Machine State Diagram

이번 장에서는 Machine의 상태 관리 구조를 보겠습니다.

이 다이어그램의 핵심은 Machine이 단순한 상태값 하나로만 동작하는 것이 아니라, State Pattern을 이용해 상태별 행동을 별도의 클래스로 분리했다는 점입니다.

Machine은 크게 네 가지 상태를 가집니다.
Idle, Working, Broken, Maintenance입니다.

먼저 Idle은 기계가 대기 중인 상태입니다.
처리할 수 있는 작업이 배정되면 Working 상태로 전환됩니다.

Working은 실제 생산을 진행하는 상태입니다.
이 상태에서는 현재 배정된 ProductionTask의 공정 진행률이 증가하고, 공정이 끝나면 다음 step으로 넘어가거나 제품 완료를 처리합니다.

Broken은 기계가 고장난 상태입니다.
이 상태에서는 update가 호출되어도 생산이 진행되지 않습니다.
기계를 다시 사용하려면 repair()를 통해 수리 과정으로 넘어가야 합니다.

Maintenance는 수리 중인 상태입니다.
이 상태에서는 수리 시간이 진행되고, 수리가 완료되면 기계의 체력이 회복됩니다.
이후 상황에 따라 다시 Idle 상태로 돌아가거나 작업을 이어갈 수 있습니다.

여기서 중요한 설계 포인트는 상태별 동작을 Machine 내부의 큰 if문으로 처리하지 않는다는 것입니다.
Machine은 현재 상태를 나타내는 MachineState 객체를 가지고 있고, update가 호출되면 실제 동작을 이 State 객체에게 위임합니다.

예를 들어 현재 상태가 WorkingState라면 생산 진행 로직이 실행되고, MaintenanceState라면 수리 진행 로직이 실행됩니다.
BrokenState라면 생산을 진행하지 않고, IdleState라면 새 작업을 받을 수 있는 상태로 유지됩니다.

이처럼 MachineState는 상태별 행동을 담당합니다.
반면 MachineStatus enum은 외부 표시와 저장을 위한 상태값으로 사용됩니다.
예를 들어 DTO Snapshot에서 기계 상태를 보여주거나, Memento로 저장하고 복원할 때는 MachineStatus 값이 필요합니다.

따라서 이 구조에서는 MachineState 객체가 실제 행동을 담당하고, MachineStatus enum은 조회, 표시, 저장을 위한 값으로 사용된다고 볼 수 있습니다.

상태 전이를 간단히 정리하면 다음과 같습니다.
기계는 기본적으로 Idle 상태에서 시작합니다.
작업이 배정되면 Working 상태가 되고, 생산이 끝나면 다시 Idle 상태로 돌아갑니다.
생산 중 고장이 발생하면 Broken 상태가 되고, repair가 호출되면 Maintenance 상태로 이동합니다.
수리가 완료되면 다시 사용 가능한 상태로 복귀합니다.

정리하면, Machine State 구조는 기계의 상태별 동작을 별도 클래스로 분리한 구조입니다.
이를 통해 Machine 클래스가 모든 상태 로직을 직접 처리하지 않아도 되고, 새로운 상태나 상태별 동작을 추가할 때도 구조를 더 명확하게 유지할 수 있습니다.


# 7. Observer / Event Pattern Diagram

이번 장에서는 Gactorio Backend의 Observer / Event 구조를 보겠습니다.

이 다이어그램의 핵심은 생산 과정에서 발생한 이벤트를 EventBus가 전달하고, 여러 Observer들이 그 이벤트를 받아 각자의 역할을 수행한다는 점입니다.

먼저 이벤트를 발생시키는 주체는 주로 ProductionLine과 Machine입니다.
ProductionLine은 작업이 큐에 들어갔을 때 이벤트를 발생시킬 수 있고, Machine은 작업 시작, 공정 완료, 제품 완료, 기계 고장, 수리 완료 같은 이벤트를 발생시킬 수 있습니다.

이때 ProductionLine이나 Machine이 이벤트를 직접 로그에 저장하거나 통계를 수정하지는 않습니다.
대신 Event 객체를 만들고, EventBus::publish()를 통해 이벤트를 발행합니다.
Event 객체에는 발생 시간, 이벤트 타입, 관련 기계 ID, 메시지 같은 정보가 담깁니다.

EventBus는 이벤트를 전달하는 dispatcher 역할을 합니다.
EventBus는 등록된 Observer 목록을 가지고 있다가, 이벤트가 발행되면 각 Observer의 onEvent()를 호출합니다.

Observer 인터페이스를 구현하는 대표 클래스는 EventLogObserver와 StatisticsObserver입니다.

EventLogObserver는 전달받은 이벤트를 내부 목록에 저장합니다.
이 객체는 시뮬레이션 중 어떤 일이 발생했는지를 기록하는 이벤트 로그 역할을 합니다.

StatisticsObserver는 이벤트를 저장하기보다는, 이벤트 타입을 기준으로 통계를 갱신합니다.
예를 들어 제품 완료 이벤트가 발생하면 완료 제품 수를 증가시키고, 기계 고장 이벤트가 발생하면 고장 횟수를 증가시키는 방식입니다.

이 구조에서 Factory는 EventBus와 Observer들을 관리합니다.
Factory는 EventLogObserver와 StatisticsObserver를 EventBus에 등록하고, 이후 생산 중 발생한 이벤트들이 두 Observer에게 전달되도록 합니다.

중요한 점은 생산 로직과 기록, 통계 기능이 분리되어 있다는 것입니다.
Machine과 ProductionLine은 “이벤트가 발생했다”는 사실만 알리면 되고, 그 이벤트를 로그로 저장할지 통계로 집계할지는 Observer가 담당합니다.

또한 DTO와 Observer도 직접 연결되어 있지 않습니다.
외부에 상태를 보여줄 때는 FactoryController가 Observer의 결과를 읽고, EventSnapshot이나 StatisticsSnapshot 같은 DTO로 변환해서 전달합니다.

전체 흐름을 정리하면 다음과 같습니다.
ProductionLine이나 Machine에서 이벤트가 발생합니다.
EventBus가 이 이벤트를 Observer들에게 전달합니다.
EventLogObserver는 이벤트 기록을 저장하고, StatisticsObserver는 통계를 갱신합니다.
마지막으로 FactoryController가 이 정보를 DTO Snapshot 형태로 외부에 제공합니다.

정리하면, Observer / Event 구조는 이벤트를 발생시키는 생산 로직과 이벤트를 처리하는 로그·통계 기능을 분리한 구조입니다.
이 덕분에 나중에 알림 기능이나 디버깅용 모니터링 기능을 추가하더라도, 기존 생산 로직을 크게 바꾸지 않고 새로운 Observer를 추가하는 방식으로 확장할 수 있습니다.


# 8. Memento Pattern Diagram

이번 장에서는 Gactorio Backend의 Memento Pattern 구조를 보겠습니다.

이 다이어그램의 핵심은 시뮬레이션 상태를 checkpoint로 저장하고, 필요할 때 이전 상태로 되돌릴 수 있도록 역할을 분리했다는 점입니다.

Memento Pattern에서 중요한 역할은 세 가지입니다.
첫 번째는 Originator입니다.
이 프로젝트에서는 Factory가 Originator 역할을 합니다.
Factory는 현재 시뮬레이션 상태를 FactoryMemento로 저장할 수 있고, 반대로 FactoryMemento를 받아 자신의 상태를 복원할 수 있습니다.

두 번째는 Memento입니다.
이 역할은 FactoryMemento, LineMemento, MachineMemento가 담당합니다.
FactoryMemento는 전체 공장 상태를 저장하고, 그 안에 생산라인 상태와 기계 상태가 함께 들어갑니다.
LineMemento는 생산라인의 작업 큐 정보를 저장하고, MachineMemento는 개별 기계의 상태와 체력 같은 정보를 저장합니다.
여기서 Memento 객체들은 public field를 가진 struct가 아니라, private field와 getter를 가진 class입니다.
즉 Factory와 복원 로직은 필요한 값을 읽을 수 있지만, 외부에서 snapshot 내부 값을 직접 바꾸는 방식은 막아 두었습니다.

세 번째는 Caretaker입니다.
이 역할은 SimulationHistory가 담당합니다.
SimulationHistory는 FactoryMemento들을 stack 형태로 보관하고, checkpoint 저장과 undo 기능을 지원합니다.
즉, SimulationHistory는 상태를 보관하지만, Factory 내부 구조를 직접 수정하지는 않습니다.

외부에서 이 기능을 사용하는 진입점은 FactoryController입니다.
FactoryController는 saveCheckpoint(), undo(), canUndo(), historySize() 같은 메서드를 통해 checkpoint와 undo 기능을 제공합니다.
따라서 외부 계층은 Memento 내부 구조를 몰라도 FactoryController를 통해 상태 저장과 복원을 사용할 수 있습니다.

저장 흐름은 다음과 같습니다.
먼저 FactoryController::saveCheckpoint()가 호출됩니다.
그러면 FactoryController는 Factory에게 현재 상태를 FactoryMemento로 만들어 달라고 요청합니다.
Factory는 현재 시간, 재고, 생산라인 queue, 기계 상태 등 복원에 필요한 정보를 모아 FactoryMemento를 생성합니다.
생성 과정에서 FactoryMemento에는 LineMemento가 추가되고, CarbonationFactory는 동적 라인과 기계 생성을 위해 nextLineId와 nextMachineId도 함께 저장합니다.
이후 SimulationHistory가 이 Memento를 stack에 저장합니다.

undo 흐름은 반대 방향으로 진행됩니다.
FactoryController::undo()가 호출되면, SimulationHistory에서 가장 최근 checkpoint를 꺼냅니다.
그리고 이 FactoryMemento를 Factory::restoreFromMemento()에 전달합니다.
Factory는 저장되어 있던 시간, 재고, 생산라인 상태, 기계 상태를 기준으로 자신의 내부 상태를 복원합니다.

여기서 중요한 설계 포인트는 DTO Snapshot과 Memento가 분리되어 있다는 점입니다.
DTO Snapshot은 화면이나 API에 현재 상태를 보여주기 위한 값 객체입니다.
반면 Memento는 내부 상태를 복원하기 위한 저장 객체입니다.
따라서 FactorySnapshot 같은 DTO는 undo에 사용하지 않고, 복원에는 FactoryMemento 계층만 사용합니다.

또 하나의 포인트는 SimulationHistory가 Factory의 내부 객체를 직접 다루지 않는다는 것입니다.
SimulationHistory는 단지 저장된 Memento를 보관하고 꺼내는 역할만 합니다.
실제 상태를 어떻게 저장하고 복원할지는 Originator인 Factory가 담당합니다.
이 덕분에 상태 관리 책임이 Factory 안에 유지되고, history 관리 로직과 복원 로직이 분리됩니다.

정리하면, 이 Memento 구조는 Factory가 자신의 상태를 저장하고 복원하는 Originator 역할을 하고, SimulationHistory가 그 상태들을 보관하는 Caretaker 역할을 하는 구조입니다.
그리고 FactoryController가 외부에 checkpoint와 undo 기능을 제공함으로써, 외부 계층은 Model 내부를 직접 조작하지 않고도 시뮬레이션 상태를 이전 시점으로 되돌릴 수 있습니다.


# 9. 설계 장점과 마무리
마지막으로 Gactorio Backend 구조를 정리하겠습니다.
이 프로젝트는 Factory를 중심으로 시뮬레이션 상태를 관리하고, FactoryController와 DTO를 통해 외부 계층과 Model을 분리했습니다.
또한 Machine 상태 관리는 State Pattern, 
이벤트 로그와 통계 처리는 Observer Pattern, 
checkpoint와 undo 기능은 Memento Pattern으로 구현했습니다.
결과적으로 생산 로직, 상태 관리, 이벤트 처리, 복원 기능의 책임이 분리되어 유지보수와 확장이 쉬운 구조가 되었습니다.
이상으로 발표를 마치겠습니다. 감사합니다.
