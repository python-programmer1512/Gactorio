# 1 : 파일

| 파일 경로                                         | UML 포함 여부 | 분류         | 파일의 핵심 역할                                                       | 제외한다면 제외 이유                                |     |
| --------------------------------------------- | --------: | ---------- | --------------------------------------------------------------- | ------------------------------------------ | --- |
| `include/model/Factory.hpp`                   |        포함 | Model      | 공장 aggregate, 생산라인/재고/이벤트/시계 관리, Memento Originator             |                                            |     |
| `src/model/Factory.cpp`                       |        포함 | Model      | Factory 동작 구현, 업데이트/복원/생산 처리                                    |                                            |     |
| `include/model/CarbonationFactory.hpp`        |        포함 | Model      | 음료 공장 특화 Factory, 기본 라인/레시피/동적 라인 생성                            |                                            |     |
| `src/model/CarbonationFactory.cpp`            |        포함 | Model      | CarbonationFactory 초기 구성과 Memento 복원 보조 구현                      |                                            |     |
| `include/model/Inventory.hpp`                 |        포함 | Model      | 원자재/완제품 재고 저장, 소비/보충                                            |                                            |     |
| `src/model/Inventory.cpp`                     |        포함 | Model      | Inventory 연산 구현                                                 |                                            |     |
| `include/model/Item.hpp`                      |        포함 | Model      | 원자재 Item 계층과 ItemType 이름 변환                                     |                                            |     |
| `src/model/Item.cpp`                          |        포함 | Model      | Item 구체 타입 구현                                                   |                                            |     |
| `include/model/Product.hpp`                   |        포함 | Model      | Product 추상 클래스, 제품 요구 재료와 공정 단계                                 |                                            |     |
| `src/model/Product.cpp`                       |        포함 | Model      | 제품/공정 단계/요구 재료 구현                                               |                                            |     |
| `include/model/ProductCatalog.hpp`            |        포함 | Model      | 제품 정의 registry, Product factory 함수                              |                                            |     |
| `src/model/ProductCatalog.cpp`                |        포함 | Model      | 제품 카탈로그 데이터와 생성 함수 구현                                           |                                            |     |
| `include/model/Recipe.hpp`                    |        포함 | Model      | 입력/출력과 소요 시간을 가진 Recipe                                         |                                            |     |
| `src/model/Recipe.cpp`                        |        포함 | Model      | Recipe 데이터 조작 구현                                                |                                            |     |
| `include/model/ProductionTask.hpp`            |        포함 | Model      | 한 제품의 공정 진행 상태 추적                                               |                                            |     |
| `src/model/ProductionTask.cpp`                |        포함 | Model      | ProductionTask 단계 진행/완료율 구현                                     |                                            |     |
| `include/model/ProductionLine.hpp`            |        포함 | Model      | 생산라인, 작업 큐, 기계 목록, 완료 제품 수집                                     |                                            |     |
| `src/model/ProductionLine.cpp`                |        포함 | Model      | 라인 업데이트, 작업 배정, 완료 처리                                           |                                            |     |
| `include/model/Machine.hpp`                   |        포함 | Model      | Machine 추상 클래스와 Mixing/Quality/Bottling/Packaging 구체 기계         |                                            |     |
| `src/model/Machine.cpp`                       |        포함 | Model      | 기계 상태 전이, 작업 진행, 고장/수리 로직                                       |                                            |     |
| `include/model/MachineState.hpp`              |        포함 | Model      | State 패턴의 추상 상태                                                 |                                            |     |
| `src/model/MachineState.cpp`                  |        포함 | Model      | 상태 enter/exit 기본 구현                                             |                                            |     |
| `include/model/MachineStates.hpp`             |        포함 | Model      | Idle/Working/Broken/Maintenance 상태 클래스                          |                                            |     |
| `src/model/MachineStates.cpp`                 |        포함 | Model      | 각 MachineState update 구현                                        |                                            |     |
| `include/model/events/Event.hpp`              |        포함 | Event      | 시뮬레이션 이벤트 값 객체                                                  |                                            |     |
| `src/model/events/Event.cpp`                  |        포함 | Event      | Event 생성자/접근자 구현                                                |                                            |     |
| `include/model/events/Observer.hpp`           |        포함 | Event      | Observer 인터페이스                                                  |                                            |     |
| `include/model/events/EventBus.hpp`           |        포함 | Event      | Observer 구독/이벤트 publish                                         |                                            |     |
| `src/model/events/EventBus.cpp`               |        포함 | Event      | EventBus 구현                                                     |                                            |     |
| `include/model/events/EventLogObserver.hpp`   |        포함 | Event      | 이벤트 로그 Observer                                                 |                                            |     |
| `src/model/events/EventLogObserver.cpp`       |        포함 | Event      | 이벤트 누적 구현                                                       |                                            |     |
| `include/model/events/StatisticsObserver.hpp` |        포함 | Event      | 이벤트 기반 통계 Observer                                              |                                            |     |
| `src/model/events/StatisticsObserver.cpp`     |        포함 | Event      | 통계 카운터 갱신 구현                                                    |                                            |     |
| `include/model/memento/FactoryMemento.hpp`    |        포함 | Memento    | Factory/Line/Machine snapshot 구조체                               |                                            |     |
| `include/controller/SimulationHistory.hpp`    |        포함 | Memento    | Memento Caretaker, snapshot stack 관리                            |                                            |     |
| `src/controller/SimulationHistory.cpp`        |        포함 | Memento    | SimulationHistory push/pop/clear 구현                             |                                            |     |
| `include/controller/FactoryCommand.hpp`       |        포함 | Controller | 컨트롤러 명령 결과 enum                                                 |                                            |     |
| `include/controller/FactoryController.hpp`    |        포함 | Controller | BE용 application controller, Factory 조작과 snapshot 제공             |                                            |     |
| `src/controller/FactoryController.cpp`        |        포함 | Controller | 명령 처리, DTO snapshot 변환, Memento facade 구현                       |                                            |     |
| `include/dto/EventSnapshot.hpp`               |        포함 | DTO        | 이벤트 표시용 snapshot                                                |                                            |     |
| `src/dto/EventSnapshot.cpp`                   |        포함 | DTO        | EventSnapshot 구현                                                |                                            |     |
| `include/dto/FactorySnapshot.hpp`             |        포함 | DTO        | 전체 공장 상태 snapshot                                               |                                            |     |
| `src/dto/FactorySnapshot.cpp`                 |        포함 | DTO        | FactorySnapshot 구현                                              |                                            |     |
| `include/dto/InventorySnapshot.hpp`           |        포함 | DTO        | 재고 snapshot DTO                                                 |                                            |     |
| `src/dto/InventorySnapshot.cpp`               |        포함 | DTO        | InventorySnapshot 구현                                            |                                            |     |
| `include/dto/MachineSnapshot.hpp`             |        포함 | DTO        | 기계 상태 snapshot DTO                                              |                                            |     |
| `src/dto/MachineSnapshot.cpp`                 |        포함 | DTO        | MachineSnapshot 구현                                              |                                            |     |
| `include/dto/ProductionLineSnapshot.hpp`      |        포함 | DTO        | 생산라인 snapshot DTO                                               |                                            |     |
| `src/dto/ProductionLineSnapshot.cpp`          |        포함 | DTO        | ProductionLineSnapshot 구현                                       |                                            |     |
| `include/dto/StatisticsSnapshot.hpp`          |        포함 | DTO        | 통계 snapshot DTO                                                 |                                            |     |
| `src/dto/StatisticsSnapshot.cpp`              |        포함 | DTO        | StatisticsSnapshot 구현                                           |                                            |     |
| `include/common/Types.hpp`                    |        포함 | Common     | ID alias, enum 타입 정의                                            |                                            |     |
| `include/common/SimClock.hpp`                 |        포함 | Common     | 시뮬레이션 시간/속도/정지 상태 관리                                            |                                            |     |
| `src/common/SimClock.cpp`                     |        포함 | Common     | SimClock 구현                                                     |                                            |     |
| `include/common/Config.h`                     |        제외 | Common     | 자동 생성 설정 상수                                                     | 클래스/관계가 거의 없는 constexpr 설정값이라 UML 핵심 대상 아님 |     |
| `src/common/Types.cpp`                        |        제외 | Common     | `Types.hpp` include만 존재                                         | 실질 선언/구현 없음                                |     |
| `include/controller/Controller.h`             |        제외 | 기타         | View/WASM 쪽에 노출되는 `ctrl` facade와 View DTO                       | BE 도메인 UML보다는 UI adapter/API 경계 파일         |     |
| `src/controller/Controller.cpp`               |        제외 | 기타         | `ctrl::Controller`가 `gactorio::FactoryController`를 View DTO로 변환 | UI-facing MVC facade 성격, WASM 바인딩의 대상 API  |     |
| `src/web/bindings.cpp`                        |        제외 | 기타         | Emscripten embind로 JS에 `ctrl::Controller` 노출                    | WASM 바인딩 파일                                |     |
| `src/backend_example.cpp`                     |        제외 | 기타         | `main()` 기반 백엔드 사용 예제                                           | 예제 실행 파일용, 도메인 UML 대상 아님                   |     |
| `tests/**/*.cpp`                              |        제외 | 기타         | smoke test 파일들                                                  | 테스트 파일                                     |     |
| `tests/CMakeLists.txt`                        |        제외 | 기타         | 테스트 빌드 설정                                                       | 빌드 파일                                      |     |
| `docs/**`                                     |        제외 | 기타         | HTML/CSS/JS/WASM 산출물 및 FE 코드                                    | docs/FE/UI/Web 산출물                         |     |
# 2 : 타입

| 이름                     | 종류     | 정의 파일                                         | namespace               | UML 포함 여부 | 핵심 역할                                        |
| ---------------------- | ------ | --------------------------------------------- | ----------------------- | --------- | -------------------------------------------- |
| SimClock               | class  | `include/common/SimClock.hpp`                 | `gactorio`              | 포함        | 시뮬레이션 시간, 속도, pause/stop 상태 관리               |
| ItemType               | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 원자재 타입 식별                                    |
| ProductType            | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 제품 타입 식별                                     |
| ProcessType            | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 공정 종류 식별                                     |
| MachineRole            | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 기계 역할 식별                                     |
| MachineStatus          | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 기계 상태 값                                      |
| EventType              | enum   | `include/common/Types.hpp`                    | `gactorio`              | 포함        | 이벤트 종류 값                                     |
| FactoryCommandResult   | enum   | `include/controller/FactoryCommand.hpp`       | `gactorio`              | 포함        | 컨트롤러 명령 결과                                   |
| FactoryController      | class  | `include/controller/FactoryController.hpp`    | `gactorio`              | 포함        | BE application controller, Factory 명령/조회 처리  |
| SimulationHistory      | class  | `include/controller/SimulationHistory.hpp`    | `gactorio`              | 포함        | Memento caretaker, Factory snapshot stack 관리 |
| EventSnapshot          | class  | `include/dto/EventSnapshot.hpp`               | `gactorio`              | 포함        | 이벤트 조회용 DTO                                  |
| FactorySnapshot        | class  | `include/dto/FactorySnapshot.hpp`             | `gactorio`              | 포함        | 전체 공장 상태 DTO                                 |
| InventoryEntrySnapshot | class  | `include/dto/InventorySnapshot.hpp`           | `gactorio`              | 포함        | 재고 항목 DTO                                    |
| InventorySnapshot      | class  | `include/dto/InventorySnapshot.hpp`           | `gactorio`              | 포함        | 재고 목록 DTO                                    |
| MachineSnapshot        | class  | `include/dto/MachineSnapshot.hpp`             | `gactorio`              | 포함        | 기계 상태 DTO                                    |
| ProductionLineSnapshot | class  | `include/dto/ProductionLineSnapshot.hpp`      | `gactorio`              | 포함        | 생산라인 상태 DTO                                  |
| StatisticsSnapshot     | class  | `include/dto/StatisticsSnapshot.hpp`          | `gactorio`              | 포함        | 통계 DTO                                       |
| CarbonationFactory     | class  | `include/model/CarbonationFactory.hpp`        | `gactorio`              | 포함        | 음료 공장 특화 Factory                             |
| Factory                | class  | `include/model/Factory.hpp`                   | `gactorio`              | 포함        | 공장 aggregate root, 라인/재고/이벤트/시간 관리           |
| Inventory              | class  | `include/model/Inventory.hpp`                 | `gactorio`              | 포함        | 원자재와 완제품 재고 관리                               |
| Item                   | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | 원자재 추상 기반 클래스                                |
| Ingredient             | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | Ingredient 원자재                               |
| Water                  | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | Water 원자재                                    |
| EmptyBottle            | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | EmptyBottle 원자재                              |
| Label                  | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | Label 원자재                                    |
| Package                | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | Package 원자재                                  |
| ItemTypeName           | class  | `include/model/Item.hpp`                      | `gactorio`              | 포함        | ItemType 이름 변환 helper                        |
| Machine                | class  | `include/model/Machine.hpp`                   | `gactorio`              | 포함        | 생산 설비 추상 기반 클래스                              |
| MixingStation          | class  | `include/model/Machine.hpp`                   | `gactorio`              | 포함        | Mixing 역할 기계                                 |
| QualityStation         | class  | `include/model/Machine.hpp`                   | `gactorio`              | 포함        | Quality 역할 기계                                |
| BottlingStation        | class  | `include/model/Machine.hpp`                   | `gactorio`              | 포함        | Bottling 역할 기계                               |
| PackagingStation       | class  | `include/model/Machine.hpp`                   | `gactorio`              | 포함        | Packaging 역할 기계                              |
| MachineState           | class  | `include/model/MachineState.hpp`              | `gactorio`              | 포함        | State 패턴 추상 상태                               |
| IdleState              | class  | `include/model/MachineStates.hpp`             | `gactorio`              | 포함        | 기계 Idle 상태                                   |
| WorkingState           | class  | `include/model/MachineStates.hpp`             | `gactorio`              | 포함        | 기계 Working 상태                                |
| BrokenState            | class  | `include/model/MachineStates.hpp`             | `gactorio`              | 포함        | 기계 Broken 상태                                 |
| MaintenanceState       | class  | `include/model/MachineStates.hpp`             | `gactorio`              | 포함        | 기계 Maintenance 상태                            |
| ItemRequirement        | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | 제품 생산에 필요한 원자재 수량                            |
| ProcessStep            | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | 제품 생산 경로의 한 공정 단계                            |
| Product                | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | 완제품 추상 기반 클래스                                |
| VoltzClassic           | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | Voltz Classic 제품                             |
| HyperBolt              | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | Hyper Bolt 제품                                |
| AuroraZero             | class  | `include/model/Product.hpp`                   | `gactorio`              | 포함        | Aurora Zero 제품                               |
| ProductDefinition      | struct | `include/model/ProductCatalog.hpp`            | `gactorio`              | 포함        | 제품 카탈로그 정의 데이터                               |
| CatalogProduct         | class  | `src/model/ProductCatalog.cpp`                | `gactorio::{anonymous}` | 선택 포함     | ProductDefinition 기반 내부 Product 구현체          |
| ProductionLine         | class  | `include/model/ProductionLine.hpp`            | `gactorio`              | 포함        | 작업 큐와 기계 묶음을 가진 생산라인                         |
| ProductionTask         | class  | `include/model/ProductionTask.hpp`            | `gactorio`              | 포함        | 한 제품의 공정 진행 상태                               |
| Recipe                 | class  | `include/model/Recipe.hpp`                    | `gactorio`              | 포함        | 입력/출력/소요시간을 가진 레시피                           |
| Event                  | struct | `include/model/events/Event.hpp`              | `gactorio`              | 포함        | 시뮬레이션 이벤트 값 객체                               |
| EventBus               | class  | `include/model/events/EventBus.hpp`           | `gactorio`              | 포함        | Observer 구독 및 이벤트 발행                         |
| Observer               | class  | `include/model/events/Observer.hpp`           | `gactorio`              | 포함        | 이벤트 Observer 인터페이스                           |
| EventLogObserver       | class  | `include/model/events/EventLogObserver.hpp`   | `gactorio`              | 포함        | 이벤트 로그 누적 Observer                           |
| StatisticsObserver     | class  | `include/model/events/StatisticsObserver.hpp` | `gactorio`              | 포함        | 이벤트 기반 통계 Observer                           |
| MachineMemento         | struct | `include/model/memento/FactoryMemento.hpp`    | `gactorio`              | 포함        | 기계 snapshot 데이터                              |
| LineMemento            | struct | `include/model/memento/FactoryMemento.hpp`    | `gactorio`              | 포함        | 생산라인 snapshot 데이터                            |
| FactoryMemento         | struct | `include/model/memento/FactoryMemento.hpp`    | `gactorio`              | 포함        | Factory 전체 snapshot 데이터                      |

# 3 : 변수

| 클래스/struct명            | 멤버 변수                   | 타입                                            | 접근 지정자  | 소유/참조 의미              | UML 표시 여부 | 비고                                    |
| ---------------------- | ----------------------- | --------------------------------------------- | ------- | --------------------- | --------- | ------------------------------------- |
| SimClock               | currentTime_            | SimulationTime                                | private | 값 소유                  | 표시        | 현재 시뮬레이션 시간                           |
| SimClock               | lastDeltaTime_          | SimulationTime                                | private | 값 소유                  | 선택        | 내부 시간 계산값                             |
| SimClock               | speedMultiplier_        | double                                        | private | 값 소유                  | 표시        | 시뮬레이션 속도                              |
| SimClock               | paused_                 | bool                                          | private | 값 소유                  | 표시        | 일시정지 상태                               |
| SimClock               | stopped_                | bool                                          | private | 값 소유                  | 표시        | 정지 상태                                 |
| FactoryController      | factory_                | std::unique_ptr\<CarbonationFactory>          | private | 단독 소유                 | 표시        | Controller가 Factory 생명주기 소유           |
| FactoryController      | history_                | SimulationHistory                             | private | 값 소유                  | 표시        | Memento caretaker                     |
| SimulationHistory      | snapshots_              | std::vector\<FactoryMemento>                  | private | Memento 값 목록 소유       | 표시        | undo stack                            |
| EventSnapshot          | simulationTime_         | SimulationTime                                | private | 값 소유                  | 표시        | DTO 필드                                |
| EventSnapshot          | type_                   | EventType                                     | private | 값 소유                  | 표시        | DTO 필드                                |
| EventSnapshot          | message_                | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| FactorySnapshot        | simulationTime_         | SimulationTime                                | private | 값 소유                  | 표시        | DTO 필드                                |
| FactorySnapshot        | inventory_              | InventorySnapshot                             | private | 값 소유                  | 표시        | DTO composition                       |
| FactorySnapshot        | statistics_             | StatisticsSnapshot                            | private | 값 소유                  | 표시        | DTO composition                       |
| FactorySnapshot        | productionLines_        | std::vector\<ProductionLineSnapshot>          | private | DTO 목록 소유             | 표시        | DTO composition                       |
| FactorySnapshot        | events_                 | std::vector\<EventSnapshot>                   | private | DTO 목록 소유             | 표시        | DTO composition                       |
| InventoryEntrySnapshot | id_                     | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| InventoryEntrySnapshot | quantity_               | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| InventorySnapshot      | items_                  | std::vector\<InventoryEntrySnapshot>          | private | DTO 목록 소유             | 표시        | DTO composition                       |
| MachineSnapshot        | id_                     | MachineId                                     | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | name_                   | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | typeName_               | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | status_                 | MachineStatus                                 | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | stateName_              | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | progress_               | double                                        | private | 값 소유                  | 표시        | DTO 필드                                |
| MachineSnapshot        | health_                 | double                                        | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | id_                     | ProductionLineId                              | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | name_                   | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | queueLength_            | std::size_t                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | currentTaskName_        | std::string                                   | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | currentTaskProgress_    | double                                        | private | 값 소유                  | 표시        | DTO 필드                                |
| ProductionLineSnapshot | machines_               | std::vector\<MachineSnapshot>                 | private | DTO 목록 소유             | 표시        | DTO composition                       |
| StatisticsSnapshot     | completedProductEvents_ | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| StatisticsSnapshot     | startedTaskEvents_      | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| StatisticsSnapshot     | completedStepEvents_    | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| StatisticsSnapshot     | brokenMachineEvents_    | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| StatisticsSnapshot     | repairedMachineEvents_  | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| StatisticsSnapshot     | stateChangedEvents_     | int                                           | private | 값 소유                  | 표시        | DTO 필드                                |
| CarbonationFactory     | recipes_                | std::vector\<Recipe>                          | private | Recipe 목록 소유          | 표시        | 특화 Factory의 레시피 목록                    |
| CarbonationFactory     | nextLineId_             | LineId                                        | private | 값 소유                  | 표시        | 동적 라인 ID 발급                           |
| CarbonationFactory     | nextMachineId_          | MachineId                                     | private | 값 소유                  | 표시        | 동적 기계 ID 발급                           |
| Factory                | clock_                  | SimClock                                      | private | 값 소유                  | 표시        | simulation clock composition          |
| Factory                | inventory_              | Inventory                                     | private | 값 소유                  | 표시        | inventory composition                 |
| Factory                | productionLines_        | std::vector\<ProductionLine>                  | private | ProductionLine 목록 소유  | 표시        | line composition                      |
| Factory                | machines_               | std::vector\<Machine*>                        | private | 비소유 raw pointer cache | 선택        | productionLines_ 내부 Machine을 가리키는 캐시  |
| Factory                | eventBus_               | EventBus                                      | private | 값 소유                  | 표시        | event publisher                       |
| Factory                | eventLog_               | EventLogObserver                              | private | 값 소유                  | 표시        | observer 소유                           |
| Factory                | statistics_             | StatisticsObserver                            | private | 값 소유                  | 표시        | observer 소유                           |
| Inventory              | items_                  | std::map\<ItemType, int>                      | private | 값 소유                  | 표시        | 원자재 재고                                |
| Inventory              | products_               | std::map\<ProductId, int>                     | private | 값 소유                  | 표시        | 완제품 재고                                |
| Item                   | name_                   | std::string                                   | private | 값 소유                  | 표시        | Item 공통 이름                            |
| Ingredient             | 없음                      | -                                             | -       | -                     | 생략        | Item만 상속                              |
| Water                  | 없음                      | -                                             | -       | -                     | 생략        | Item만 상속                              |
| EmptyBottle            | 없음                      | -                                             | -       | -                     | 생략        | Item만 상속                              |
| Label                  | 없음                      | -                                             | -       | -                     | 생략        | Item만 상속                              |
| Package                | 없음                      | -                                             | -       | -                     | 생략        | Item만 상속                              |
| ItemTypeName           | 없음                      | -                                             | -       | -                     | 생략        | static helper                         |
| Machine                | id_                     | MachineId                                     | private | 값 소유                  | 표시        | 기계 식별자                                |
| Machine                | name_                   | std::string                                   | private | 값 소유                  | 표시        | 기계 이름                                 |
| Machine                | recipe_                 | std::optional\<Recipe>                        | private | optional 값 소유         | 표시        | 할당 가능한 recipe                         |
| Machine                | task_                   | std::shared_ptr\<ProductionTask>              | private | 공유 소유                 | 표시        | 현재 작업                                 |
| Machine                | state_                  | std::unique_ptr\<MachineState>                | private | 현재 상태 단독 소유           | 표시        | State 패턴 핵심                           |
| Machine                | eventBus_               | EventBus*                                     | private | 비소유 raw pointer       | 표시        | Factory/EventBus 참조                   |
| Machine                | status_                 | MachineStatus                                 | private | 값 소유                  | 표시        | 상태 enum                               |
| Machine                | progress_               | double                                        | private | 값 소유                  | 표시        | 현재 step 진행률                           |
| Machine                | health_                 | double                                        | private | 값 소유                  | 표시        | HP                                    |
| Machine                | processingSpeed_        | double                                        | private | 값 소유                  | 표시        | 처리 속도                                 |
| Machine                | breakdownProbability_   | double                                        | private | 값 소유                  | 표시        | 고장 확률                                 |
| Machine                | maintenanceElapsed_     | double                                        | private | 값 소유                  | 선택        | maintenance 내부 타이머                    |
| Machine                | maintenanceDuration_    | double                                        | private | 값 소유                  | 선택        | maintenance 내부 설정                     |
| Machine                | simulationTime_         | SimulationTime                                | private | 값 소유                  | 선택        | 이벤트 시간 누적용 내부값                        |
| MixingStation          | 없음                      | -                                             | -       | -                     | 생략        | Machine 특화 override만 있음               |
| QualityStation         | 없음                      | -                                             | -       | -                     | 생략        | Machine 특화 override만 있음               |
| BottlingStation        | 없음                      | -                                             | -       | -                     | 생략        | Machine 특화 override만 있음               |
| PackagingStation       | 없음                      | -                                             | -       | -                     | 생략        | Machine 특화 override만 있음               |
| MachineState           | 없음                      | -                                             | -       | -                     | 생략        | 상태 인터페이스                              |
| IdleState              | 없음                      | -                                             | -       | -                     | 생략        | 상태 동작만 정의                             |
| WorkingState           | 없음                      | -                                             | -       | -                     | 생략        | 상태 동작만 정의                             |
| BrokenState            | 없음                      | -                                             | -       | -                     | 생략        | 상태 동작만 정의                             |
| MaintenanceState       | 없음                      | -                                             | -       | -                     | 생략        | 상태 동작만 정의                             |
| ItemRequirement        | itemType_               | ItemType                                      | private | 값 소유                  | 표시        | 필요 원자재 타입                             |
| ItemRequirement        | quantity_               | int                                           | private | 값 소유                  | 표시        | 필요 수량                                 |
| ProcessStep            | requiredRole_           | MachineRole                                   | private | 값 소유                  | 표시        | 필요한 기계 역할                             |
| ProcessStep            | baseDurationSeconds_    | SimulationTime                                | private | 값 소유                  | 표시        | 기준 처리 시간                              |
| Product                | id_                     | ProductId                                     | private | 값 소유                  | 표시        | 제품 ID                                 |
| Product                | name_                   | std::string                                   | private | 값 소유                  | 표시        | 제품명                                   |
| Product                | requirements_           | std::vector\<ItemRequirement>                 | private | 값 목록 소유               | 표시        | 재료 요구사항 composition                   |
| Product                | route_                  | std::vector\<ProcessStep>                     | private | 값 목록 소유               | 표시        | 공정 경로 composition                     |
| VoltzClassic           | 없음                      | -                                             | -       | -                     | 생략        | Product 데이터는 base에 저장                 |
| HyperBolt              | 없음                      | -                                             | -       | -                     | 생략        | Product 데이터는 base에 저장                 |
| AuroraZero             | 없음                      | -                                             | -       | -                     | 생략        | Product 데이터는 base에 저장                 |
| ProductDefinition      | id                      | ProductId                                     | public  | 값 소유                  | 표시        | catalog data                          |
| ProductDefinition      | type                    | ProductType                                   | public  | 값 소유                  | 표시        | catalog data                          |
| ProductDefinition      | key                     | std::string                                   | public  | 값 소유                  | 표시        | catalog key                           |
| ProductDefinition      | name                    | std::string                                   | public  | 값 소유                  | 표시        | 제품명                                   |
| ProductDefinition      | tier                    | std::string                                   | public  | 값 소유                  | 표시        | 제품 등급                                 |
| ProductDefinition      | totalDurationSeconds    | double                                        | public  | 값 소유                  | 표시        | 총 소요 시간                               |
| ProductDefinition      | requirements            | std::vector\<ItemRequirement>                 | public  | 값 목록 소유               | 표시        | 재료 요구사항                               |
| ProductDefinition      | route                   | std::vector\<ProcessStep>                     | public  | 값 목록 소유               | 표시        | 공정 경로                                 |
| CatalogProduct         | 없음                      | -                                             | -       | -                     | 생략        | `Product` 상속, cpp 내부 구현 클래스           |
| ProductionLine         | id_                     | ProductionLineId                              | private | 값 소유                  | 표시        | 라인 ID                                 |
| ProductionLine         | name_                   | std::string                                   | private | 값 소유                  | 표시        | 라인 이름                                 |
| ProductionLine         | taskQueue_              | std::deque\<std::shared_ptr\<ProductionTask>> | private | 작업 shared 소유 큐        | 표시        | 대기 작업 큐                               |
| ProductionLine         | completedProducts_      | std::vector\<ProductId>                       | private | 값 목록 소유               | 표시        | 완료 제품 ID 목록                           |
| ProductionLine         | machines_               | std::vector\<std::unique_ptr\<Machine>>       | private | Machine 다형 객체 단독 소유   | 표시        | 라인이 기계들을 composition                  |
| ProductionLine         | eventBus_               | EventBus*                                     | private | 비소유 raw pointer       | 표시        | Factory/EventBus 참조                   |
| ProductionTask         | ownedProduct_           | std::shared_ptr\<Product>                     | private | 선택적 공유 소유             | 표시        | shared product 생성자 사용 시 소유            |
| ProductionTask         | product_                | const Product*                                | private | 비소유 raw pointer       | 선택        | 실제 참조 대상, ownedProduct_ 또는 외부 Product |
| ProductionTask         | currentStepIndex_       | std::size_t                                   | private | 값 소유                  | 표시        | 현재 공정 단계 index                        |
| Recipe                 | id_                     | RecipeId                                      | private | 값 소유                  | 표시        | recipe ID                             |
| Recipe                 | name_                   | std::string                                   | private | 값 소유                  | 표시        | recipe 이름                             |
| Recipe                 | durationSeconds_        | double                                        | private | 값 소유                  | 표시        | 소요 시간                                 |
| Recipe                 | inputs_                 | std::map\<ItemType, int>                      | private | 값 소유                  | 표시        | 입력 재료                                 |
| Recipe                 | outputs_                | std::map\<ProductId, int>                     | private | 값 소유                  | 표시        | 출력 제품                                 |
| Event                  | simulationTime_         | SimulationTime                                | private | 값 소유                  | 표시        | 이벤트 발생 시간                             |
| Event                  | type_                   | EventType                                     | private | 값 소유                  | 표시        | 이벤트 종류                                |
| Event                  | sourceId_               | MachineId                                     | private | 값 소유                  | 표시        | 발생 기계 ID                              |
| Event                  | message_                | std::string                                   | private | 값 소유                  | 표시        | 이벤트 메시지                               |
| EventBus               | observers_              | std::vector\<Observer*>                       | private | 비소유 raw pointer 목록    | 표시        | Observer는 외부/Factory가 소유              |
| Observer               | 없음                      | -                                             | -       | -                     | 생략        | 인터페이스                                 |
| EventLogObserver       | events_                 | std::vector\<Event>                           | private | Event 목록 소유           | 표시        | 이벤트 로그                                |
| StatisticsObserver     | completedProductEvents_ | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| StatisticsObserver     | startedTaskEvents_      | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| StatisticsObserver     | completedStepEvents_    | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| StatisticsObserver     | brokenMachineEvents_    | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| StatisticsObserver     | repairedMachineEvents_  | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| StatisticsObserver     | stateChangedEvents_     | int                                           | private | 값 소유                  | 표시        | 통계 카운터                                |
| MachineMemento         | id_                     | MachineId                                     | private | 값 소유                  | 표시        | accessor `id()` 로만 노출 (public data member 금지 준수) |
| MachineMemento         | health_                 | double                                        | private | 값 소유                  | 표시        | accessor `health()` 로만 노출            |
| MachineMemento         | status_                 | MachineStatus                                 | private | 값 소유                  | 표시        | accessor `status()` 로만 노출           |
| LineMemento            | id_                     | LineId                                        | private | 값 소유                  | 표시        | accessor `id()` 로만 노출               |
| LineMemento            | queueProductIds_        | std::vector\<ProductId>                       | private | 값 목록 소유               | 표시        | accessor `queueProductIds()` 로만 노출  |
| LineMemento            | machines_               | std::vector\<MachineMemento>                  | private | Memento 목록 소유         | 표시        | composition, accessor `machines()`    |
| FactoryMemento         | simulationTime_         | SimulationTime                                | private | 값 소유                  | 표시        | accessor `simulationTime()` 로만 노출   |
| FactoryMemento         | items_                  | std::map\<ItemType, int>                      | private | 값 소유                  | 표시        | accessor `items()` 로만 노출            |
| FactoryMemento         | products_               | std::map\<ProductId, int>                     | private | 값 소유                  | 표시        | accessor `products()` 로만 노출         |
| FactoryMemento         | lines_                  | std::vector\<LineMemento>                     | private | Memento 목록 소유         | 표시        | composition, accessor `lines()` + `addLine()` |
| FactoryMemento         | nextLineId_             | LineId                                        | private | 값 소유                  | 표시        | 동적 라인 ID 복원, accessor `nextLineId()` |
| FactoryMemento         | nextMachineId_          | MachineId                                     | private | 값 소유                  | 표시        | 동적 기계 ID 복원, accessor `nextMachineId()` |
# 4 : 메서드

| 클래스명                                                                                                                | 메서드 시그니처                                                                                                                                                          | 접근 지정자            | virtual/override/const/static 여부 | 역할                                   | UML 표시 여부 |
| ------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------- | -------------------------------- | ------------------------------------ | --------- |
| SimClock                                                                                                            | `update(double)`, `pause()`, `resume()`, `stop()`, `reset()`, `setSpeed(double)`, `setNow(SimulationTime)`                                                        | public            | 일반                               | 시뮬레이션 시간 제어                          | 표시        |
| SimClock                                                                                                            | `now()`, `getDeltaTime()`, `speedMultiplier()`, `isPaused()`, `isStopped()`                                                                                       | public            | `const`                          | 시계 상태 조회                             | 선택        |
| FactoryController                                                                                                   | `FactoryController()`                                                                                                                                             | public            | 생성자                              | 기본 CarbonationFactory 생성             | 표시        |
| FactoryController                                                                                                   | `tick(double)`, `startSimulation()`, `pauseSimulation()`, `resumeSimulation()`, `resetSimulation()`, `stopSimulation()`, `setSimulationSpeed(double)`             | public            | 일반                               | 시뮬레이션 제어 facade                      | 표시        |
| FactoryController                                                                                                   | `enqueueProduct(...)`, `enqueueProductById(...)`, `enqueueAuto(...)`, `addLine()`, `removeLine(...)`, `forceBreak(...)`, `repairMachine(...)`, `restockItem(...)` | public            | 일반                               | Factory 명령 처리                        | 표시        |
| FactoryController                                                                                                   | `snapshot() const`, `getFactorySnapshot() const`, `getEventLogs() const`, `getStatistics() const`                                                                 | public            | `const`                          | DTO 변환/조회                            | 표시        |
| FactoryController                                                                                                   | `saveCheckpoint()`, `undo()`, `canUndo() const`, `historySize() const`                                                                                            | public            | 일부 `const`                       | Memento caretaker facade             | 표시        |
| SimulationHistory                                                                                                   | `push(FactoryMemento)`, `pop()`, `clear()`                                                                                                                        | public            | 일반                               | Memento stack 조작                     | 표시        |
| SimulationHistory                                                                                                   | `canUndo() const`, `size() const`                                                                                                                                 | public            | `const`                          | undo 가능 여부/크기 조회                     | 표시        |
| EventSnapshot / FactorySnapshot / InventorySnapshot / MachineSnapshot / ProductionLineSnapshot / StatisticsSnapshot | 각 DTO 생성자, getter 계열                                                                                                                                              | public            | 대부분 `const`                      | Model 상태를 View/API용 불변 snapshot으로 제공 | 표시        |
| FactorySnapshot                                                                                                     | `addProductionLine(ProductionLineSnapshot)`, `addEvent(EventSnapshot)`                                                                                            | public            | 일반                               | Factory DTO에 하위 snapshot 추가          | 표시        |
| ProductionLineSnapshot                                                                                              | `setCurrentTaskProgress(double)`, `addMachine(MachineSnapshot)`                                                                                                   | public            | 일반                               | Line DTO 구성                          | 표시        |
| CarbonationFactory                                                                                                  | `CarbonationFactory()`                                                                                                                                            | public            | 생성자                              | 기본 음료 공장 구성                          | 표시        |
| CarbonationFactory                                                                                                  | `recipes() const`, `addDynamicLine()`                                                                                                                             | public            | `const`/일반                       | 레시피 조회, 동적 생산라인 생성                   | 표시        |
| CarbonationFactory                                                                                                  | `createMemento() const`, `restoreFromMemento(const FactoryMemento&)`                                                                                              | public            | `override`, 일부 `const`           | Factory 상태 저장/복원                     | 표시        |
| CarbonationFactory                                                                                                  | `createProductById(ProductId) const`, `createLineForMemento(const LineMemento&) const`                                                                            | protected         | `override const`                 | Memento 복원 보조                        | 표시        |
| Factory                                                                                                             | `Factory()`, `virtual ~Factory()`                                                                                                                                 | public            | 생성자, `virtual` 소멸자               | Factory base lifecycle               | 표시        |
| Factory                                                                                                             | `addProductionLine(ProductionLine)`, `removeProductionLine(LineId)`, `enqueueProduct(LineId, shared_ptr<Product>)`, `restockItem(...)`, `update(double)`          | public            | 일반                               | 공장 핵심 도메인 동작                         | 표시        |
| Factory                                                                                                             | `findProductionLine(...)`, `findMachine(MachineId)`                                                                                                               | public            | 일반/`const` overload              | 도메인 객체 탐색                            | 표시        |
| Factory                                                                                                             | `pauseClock()`, `resumeClock()`, `resetClock()`, `stopClock()`, `setClockSpeed(double)`                                                                           | public            | 일반                               | 내부 SimClock 제어                       | 표시        |
| Factory                                                                                                             | `createMemento() const`, `restoreFromMemento(const FactoryMemento&)`                                                                                              | public            | `virtual`, 일부 `const`            | Memento Originator                   | 표시        |
| Factory                                                                                                             | `inventory()`, `productionLines() const`, `machines() const`, `eventLog() const`, `statistics() const`, `eventBus()`                                              | public            | 일부 `const`                       | aggregate 내부 상태 접근                   | 선택        |
| Factory                                                                                                             | `mutableEventLog()`, `mutableStatistics()`                                                                                                                        | protected         | 일반                               | subclass 내부 observer 접근              | 생략 가능     |
| Factory                                                                                                             | `createProductById(...) const`, `createLineForMemento(...) const`                                                                                                 | protected         | `virtual const`                  | 복원 시 subclass hook                   | 표시        |
| Factory                                                                                                             | `rebuildMachineCache()`                                                                                                                                           | private           | 일반                               | `machines_` 캐시 재구성                   | 선택        |
| Inventory                                                                                                           | `addItem(...)`, `addProduct(...)`, `consume(...)`, `replaceContents(...)`                                                                                         | public            | 일반                               | 재고 증가/소비/복원                          | 표시        |
| Inventory                                                                                                           | `hasEnough(...) const`, `canConsume(...) const`, `getQuantity(...) const`, `items() const`, `products() const`                                                    | public            | `const`                          | 재고 조회                                | 표시        |
| Item                                                                                                                | `virtual ~Item()`, `getTypeId() const = 0`, `getName() const = 0`                                                                                                 | public            | `virtual`, pure virtual, `const` | 원자재 추상 인터페이스                         | 표시        |
| Item                                                                                                                | `Item(std::string)`, `storedName() const`                                                                                                                         | protected         | 생성자, `const`                     | base 저장 이름 제공                        | 선택        |
| Ingredient / Water / EmptyBottle / Label / Package                                                                  | 생성자, `getTypeId() const override`, `getName() const override`                                                                                                     | public            | `override const`                 | 구체 Item 타입                           | 표시        |
| ItemTypeName                                                                                                        | `get(ItemType)`                                                                                                                                                   | public            | `static`                         | enum 이름 변환 helper                    | 선택        |
| Machine                                                                                                             | `Machine(...)`, `virtual ~Machine()`                                                                                                                              | public/protected  | 생성자, `virtual` 소멸자               | 기계 base lifecycle                    | 표시        |
| Machine                                                                                                             | `assignTask(shared_ptr<ProductionTask>)`, `setRecipe(const Recipe&)`, `setEventBus(EventBus*)`, `update(double)`                                                  | public            | `update`는 `virtual`              | 작업/레시피/이벤트 연결 및 갱신                   | 표시        |
| Machine                                                                                                             | `forceBreak()`, `repair()`, `incrementalRepair()`, `pause()`, `resume()`, `resetForRestore(...)`                                                                  | public            | 일반                               | 상태 변경, Memento 복원                    | 표시        |
| Machine                                                                                                             | `typeName() const = 0`, `processType() const = 0`, `role() const = 0`, `canAcceptRecipe(const Recipe&) const = 0`                                                 | public            | pure virtual `const`             | 구체 station 계약                        | 표시        |
| Machine                                                                                                             | `id() const`, `name() const`, `progress() const`, `stateName() const`, `hasTask() const`, `currentTask() const`, `recipe() const`, getter 계열                      | public            | `const`                          | 상태 조회                                | 선택        |
| Machine                                                                                                             | `setState(unique_ptr<MachineState>)`, `transitionTo...`, `advanceProduction(double)`, `advanceMaintenance(double)`, `notify(...) const`                           | private/protected | 일부 `const`                       | State 전이, 생산/정비 내부 처리                | 표시 선택     |
| MixingStation / QualityStation / BottlingStation / PackagingStation                                                 | 생성자, `typeName() const override`, `processType() const override`, `role() const override`, `canAcceptRecipe(...) const override`                                  | public            | `override const`                 | 구체 설비 역할 정의                          | 표시        |
| MachineState                                                                                                        | `virtual ~MachineState()`, `enter(Machine&)`, `update(Machine&, double) = 0`, `exit(Machine&)`, `name() const = 0`                                                | public            | `virtual`, pure virtual, `const` | State 패턴 인터페이스                       | 표시        |
| IdleState / WorkingState / BrokenState / MaintenanceState                                                           | `update(Machine&, double) override`, `name() const override`                                                                                                      | public            | `override`, `const`              | 상태별 Machine 동작                       | 표시        |
| ItemRequirement                                                                                                     | `ItemRequirement(ItemType, int)`, `itemType() const`, `quantity() const`                                                                                          | public            | 생성자, `const`                     | 재료 요구사항 값 객체                         | 표시        |
| ProcessStep                                                                                                         | `ProcessStep(MachineRole, SimulationTime)`, `requiredRole() const`, `baseDurationSeconds() const`, `durationSeconds() const`                                      | public            | 생성자, `const`                     | 공정 단계 값 객체                           | 표시        |
| Product                                                                                                             | `virtual ~Product()`, `getProductId() const = 0`, `getName() const = 0`, `getRequirements() const = 0`, `getRoute() const = 0`                                    | public            | `virtual`, pure virtual, `const` | 제품 추상 인터페이스                          | 표시        |
| Product                                                                                                             | `Product(const ProductDefinition&)`, `Product(ProductId, string, vector<...>, vector<...>)`, `stored...() const`                                                  | protected         | 생성자, `const`                     | base 데이터 저장/조회                       | 선택        |
| VoltzClassic / HyperBolt / AuroraZero                                                                               | 생성자, `getProductId() const override`, `getName() const override`, `getRequirements() const override`, `getRoute() const override`                                 | public            | `override const`                 | 구체 제품 정의                             | 표시        |
| ProductDefinition                                                                                                   | 없음                                                                                                                                                                | public fields     | -                                | 제품 카탈로그 데이터 구조                       | 필드만 표시    |
| CatalogProduct                                                                                                      | `CatalogProduct(const ProductDefinition&)`, Product override getter들                                                                                              | public            | `override const`                 | ProductDefinition 기반 내부 Product 구현   | 생략 가능     |
| ProductionLine                                                                                                      | `ProductionLine(ProductionLineId, string)`, move ctor/operator, copy 삭제                                                                                           | public            | 생성자, move, deleted copy          | 라인 lifecycle 및 소유권 제약                | 표시        |
| ProductionLine                                                                                                      | `enqueueProduct(shared_ptr<Product>)`, `assignAvailableTask()`, `collectCompletedProducts()`, `addMachine(unique_ptr<Machine>)`, `update(double)`                 | public            | 일반                               | 작업 큐/기계/완료 제품 처리                     | 표시        |
| ProductionLine                                                                                                      | `setEventBus(EventBus*)`, `findMachine(...)`, `clearQueue()`, `clearCompleted()`, `pendingProductIds() const`                                                     | public            | 일부 `const`                       | 이벤트 연결, 탐색, Memento 보조               | 표시        |
| ProductionLine                                                                                                      | `id() const`, `name() const`, `machines() const`, `queueLength() const`, `currentTask() const`                                                                    | public            | `const`                          | 상태 조회                                | 선택        |
| ProductionTask                                                                                                      | `ProductionTask(const Product&)`, `ProductionTask(shared_ptr<Product>)`                                                                                           | public            | 생성자                              | 제품 작업 생성                             | 표시        |
| ProductionTask                                                                                                      | `currentStep() const`, `advanceStep()`, `isCompleted() const`, `getProgressInRoute() const`                                                                       | public            | 일부 `const`                       | 공정 진행 관리                             | 표시        |
| ProductionTask                                                                                                      | `getProductId() const`, `getProductName() const`, `currentStepIndex() const`, `totalStepCount() const`                                                            | public            | `const`                          | 작업 상태 조회                             | 선택        |
| Recipe                                                                                                              | `Recipe(RecipeId, string, double)`, `addInput(...)`, `addOutput(...)`                                                                                             | public            | 생성자/일반                           | 레시피 구성                               | 표시        |
| Recipe                                                                                                              | `id() const`, `name() const`, `durationSeconds() const`, `inputs() const`, `outputs() const`                                                                      | public            | `const`                          | 레시피 조회                               | 표시        |
| Event                                                                                                               | `Event(SimulationTime, EventType, MachineId, string)`                                                                                                             | public            | 생성자                              | 이벤트 생성                               | 표시        |
| Event                                                                                                               | `simulationTime() const`, `type() const`, `sourceId() const`, `message() const`                                                                                   | public            | `const`                          | 이벤트 값 조회                             | 표시        |
| EventBus                                                                                                            | `subscribe(Observer*)`, `publish(const Event&) const`                                                                                                             | public            | `publish`는 `const`               | Event publish/subscribe              | 표시        |
| Observer                                                                                                            | `virtual ~Observer()`, `onEvent(const Event&) = 0`                                                                                                                | public            | `virtual`, pure virtual          | Observer 인터페이스                       | 표시        |
| EventLogObserver                                                                                                    | `onEvent(const Event&) override`, `events() const`                                                                                                                | public            | `override`, `const`              | 이벤트 로그 누적/조회                         | 표시        |
| StatisticsObserver                                                                                                  | `onEvent(const Event&) override`                                                                                                                                  | public            | `override`                       | 이벤트 기반 통계 갱신                         | 표시        |
| StatisticsObserver                                                                                                  | 각 카운터 getter `...Events() const`                                                                                                                                  | public            | `const`                          | 통계 조회                                | 선택        |
| MachineMemento / LineMemento / FactoryMemento                                                                       | 없음                                                                                                                                                                | public fields     | -                                | Memento snapshot 데이터 구조              | 필드만 표시    |
# 5 : 상속 관계

| 부모 클래스 | 자식 클래스 | 상속 접근 지정자 | 부모가 abstract인지 여부 | 관련 virtual/pure virtual 메서드 | UML 표기 방식 |
|---|---|---:|---|---|---|
| `Factory` | `CarbonationFactory` | `public` | 아니오 | `Factory`는 `virtual ~Factory()`, `virtual createMemento()`, `virtual restoreFromMemento()`, `virtual createProductById()`, `virtual createLineForMemento()` 보유. pure virtual 없음 | `CarbonationFactory ─▷ Factory` 일반화 |
| `Product` | `VoltzClassic` | `public` | 예 | `getProductId() const = 0`, `getName() const = 0`, `getRequirements() const = 0`, `getRoute() const = 0` | `VoltzClassic ─▷ Product` 일반화 |
| `Product` | `HyperBolt` | `public` | 예 | 위와 동일 | `HyperBolt ─▷ Product` 일반화 |
| `Product` | `AuroraZero` | `public` | 예 | 위와 동일 | `AuroraZero ─▷ Product` 일반화 |
| `Product` | `CatalogProduct` | `public` | 예 | 위와 동일 | `CatalogProduct ─▷ Product` 일반화, 단 cpp 내부 구현체라 UML 생략 가능 |
| `Item` | `Ingredient` | `public` | 예 | `getTypeId() const = 0`, `getName() const = 0` | `Ingredient ─▷ Item` 일반화 |
| `Item` | `Water` | `public` | 예 | 위와 동일 | `Water ─▷ Item` 일반화 |
| `Item` | `EmptyBottle` | `public` | 예 | 위와 동일 | `EmptyBottle ─▷ Item` 일반화 |
| `Item` | `Label` | `public` | 예 | 위와 동일 | `Label ─▷ Item` 일반화 |
| `Item` | `Package` | `public` | 예 | 위와 동일 | `Package ─▷ Item` 일반화 |
| `Machine` | `MixingStation` | `public` | 예 | `typeName() const = 0`, `processType() const = 0`, `role() const = 0`, `canAcceptRecipe(const Recipe&) const = 0`; `update(double)`는 virtual이지만 pure 아님 | `MixingStation ─▷ Machine` 일반화 |
| `Machine` | `QualityStation` | `public` | 예 | 위와 동일 | `QualityStation ─▷ Machine` 일반화 |
| `Machine` | `BottlingStation` | `public` | 예 | 위와 동일 | `BottlingStation ─▷ Machine` 일반화 |
| `Machine` | `PackagingStation` | `public` | 예 | 위와 동일 | `PackagingStation ─▷ Machine` 일반화 |
| `MachineState` | `IdleState` | `public` | 예 | `update(Machine&, double) = 0`, `name() const = 0`; `enter()`/`exit()`는 virtual 기본 구현 | `IdleState ─▷ MachineState` 일반화 |
| `MachineState` | `WorkingState` | `public` | 예 | 위와 동일 | `WorkingState ─▷ MachineState` 일반화 |
| `MachineState` | `BrokenState` | `public` | 예 | 위와 동일 | `BrokenState ─▷ MachineState` 일반화 |
| `MachineState` | `MaintenanceState` | `public` | 예 | 위와 동일 | `MaintenanceState ─▷ MachineState` 일반화 |
| `Observer` | `EventLogObserver` | `public` | 예 | `onEvent(const Event&) = 0` | `EventLogObserver ─▷ Observer` 일반화 |
| `Observer` | `StatisticsObserver` | `public` | 예 | `onEvent(const Event&) = 0` | `StatisticsObserver ─▷ Observer` 일반화 |
# 6 : 포함, 소유, 참조 관계
| 소유/참조하는 클래스 | 대상 클래스 | 멤버 타입 | 관계 종류 | 다중성 | 판단 근거 | UML 표기 방식 |
|---|---|---|---|---|---|---|
| `FactoryController` | `CarbonationFactory` | `std::unique_ptr<CarbonationFactory>` | composition | `1` 또는 `0..1` | `include/controller/FactoryController.hpp`, `factory_` | `FactoryController ◆-- CarbonationFactory` |
| `FactoryController` | `SimulationHistory` | `SimulationHistory` | composition | `1` | `include/controller/FactoryController.hpp`, `history_` | `FactoryController ◆-- SimulationHistory` |
| `SimulationHistory` | `FactoryMemento` | `std::vector<FactoryMemento>` | composition | `0..*` | `include/controller/SimulationHistory.hpp`, `snapshots_` | `SimulationHistory ◆-- FactoryMemento` |
| `FactoryMemento` | `LineMemento` | `std::vector<LineMemento>` | composition | `0..*` | `include/model/memento/FactoryMemento.hpp`, `lines` | `FactoryMemento ◆-- LineMemento` |
| `LineMemento` | `MachineMemento` | `std::vector<MachineMemento>` | composition | `0..*` | `include/model/memento/FactoryMemento.hpp`, `machines` | `LineMemento ◆-- MachineMemento` |
| `CarbonationFactory` | `Recipe` | `std::vector<Recipe>` | composition | `0..*` | `include/model/CarbonationFactory.hpp`, `recipes_` | `CarbonationFactory ◆-- Recipe` |
| `Factory` | `SimClock` | `SimClock` | composition | `1` | `include/model/Factory.hpp`, `clock_` | `Factory ◆-- SimClock` |
| `Factory` | `Inventory` | `Inventory` | composition | `1` | `include/model/Factory.hpp`, `inventory_` | `Factory ◆-- Inventory` |
| `Factory` | `ProductionLine` | `std::vector<ProductionLine>` | composition | `0..*` | `include/model/Factory.hpp`, `productionLines_` | `Factory ◆-- ProductionLine` |
| `Factory` | `Machine` | `std::vector<Machine*>` | association | `0..*` | `include/model/Factory.hpp`, `machines_`; raw pointer cache, 실제 소유는 `ProductionLine::machines_` | `Factory --> Machine` |
| `Factory` | `EventBus` | `EventBus` | composition | `1` | `include/model/Factory.hpp`, `eventBus_` | `Factory ◆-- EventBus` |
| `Factory` | `EventLogObserver` | `EventLogObserver` | composition | `1` | `include/model/Factory.hpp`, `eventLog_` | `Factory ◆-- EventLogObserver` |
| `Factory` | `StatisticsObserver` | `StatisticsObserver` | composition | `1` | `include/model/Factory.hpp`, `statistics_` | `Factory ◆-- StatisticsObserver` |
| `ProductionLine` | `ProductionTask` | `std::deque<std::shared_ptr<ProductionTask>>` | aggregation | `0..*` | `include/model/ProductionLine.hpp`, `taskQueue_`; shared_ptr 작업 큐 | `ProductionLine ◇-- ProductionTask` |
| `ProductionLine` | `Product` | `enqueueProduct(std::shared_ptr<Product>)` 경유 | aggregation | `0..*` | `include/model/ProductionLine.hpp`, `enqueueProduct`; task 생성 시 Product shared 소유가 작업으로 전달됨 | `ProductionLine ◇.. Product` |
| `ProductionLine` | `Machine` | `std::vector<std::unique_ptr<Machine>>` | composition | `0..*` | `include/model/ProductionLine.hpp`, `machines_` | `ProductionLine ◆-- Machine` |
| `ProductionLine` | `EventBus` | `EventBus*` | association | `0..1` | `include/model/ProductionLine.hpp`, `eventBus_`; 비소유 raw pointer | `ProductionLine --> EventBus` |
| `Machine` | `Recipe` | `std::optional<Recipe>` | composition | `0..1` | `include/model/Machine.hpp`, `recipe_`; Recipe 값을 optional로 보유 | `Machine ◆-- Recipe` |
| `Machine` | `ProductionTask` | `std::shared_ptr<ProductionTask>` | aggregation | `0..1` | `include/model/Machine.hpp`, `task_`; 라인 큐와 공유 가능 | `Machine ◇-- ProductionTask` |
| `Machine` | `MachineState` | `std::unique_ptr<MachineState>` | composition | `1` 또는 `0..1` | `include/model/Machine.hpp`, `state_`; 현재 상태 객체 단독 소유 | `Machine ◆-- MachineState` |
| `Machine` | `EventBus` | `EventBus*` | association | `0..1` | `include/model/Machine.hpp`, `eventBus_`; 비소유 raw pointer | `Machine --> EventBus` |
| `ProductionTask` | `Product` | `std::shared_ptr<Product>` | aggregation | `0..1` | `include/model/ProductionTask.hpp`, `ownedProduct_`; shared ownership | `ProductionTask ◇-- Product` |
| `ProductionTask` | `Product` | `const Product*` | association | `1` | `include/model/ProductionTask.hpp`, `product_`; 비소유 raw pointer, 실제 참조 대상 | `ProductionTask --> Product` |
| `Product` | `ItemRequirement` | `std::vector<ItemRequirement>` | composition | `0..*` | `include/model/Product.hpp`, `requirements_` | `Product ◆-- ItemRequirement` |
| `Product` | `ProcessStep` | `std::vector<ProcessStep>` | composition | `0..*` | `include/model/Product.hpp`, `route_` | `Product ◆-- ProcessStep` |
| `ProductDefinition` | `ItemRequirement` | `std::vector<ItemRequirement>` | composition | `0..*` | `include/model/ProductCatalog.hpp`, `requirements` | `ProductDefinition ◆-- ItemRequirement` |
| `ProductDefinition` | `ProcessStep` | `std::vector<ProcessStep>` | composition | `0..*` | `include/model/ProductCatalog.hpp`, `route` | `ProductDefinition ◆-- ProcessStep` |
| `ProductCatalog` 개념 | `ProductDefinition` | `static const std::vector<ProductDefinition>` | composition | `0..*` | `src/model/ProductCatalog.cpp`, `productDefinitions()` 내부 `definitions` | `ProductCatalog ◆-- ProductDefinition` |
| `ProductCatalog` 개념 | `Product` | `std::shared_ptr<Product> createProduct(...)` | dependency / aggregation 아님 | 생성 결과 `0..1` | `include/model/ProductCatalog.hpp`, `createProduct`; 객체를 생성해 반환하지만 Catalog가 소유하지 않음 | `ProductCatalog ..> Product` |
| `Inventory` | `ItemType` | `std::map<ItemType, int>` | association | `0..*` | `include/model/Inventory.hpp`, `items_`; enum key 기반 재고 | `Inventory --> ItemType` |
| `Inventory` | `ProductId` | `std::map<ProductId, int>` | association | `0..*` | `include/model/Inventory.hpp`, `products_`; 제품 객체가 아닌 ID 기반 재고 | 보통 생략 |
| `Recipe` | `ItemType` | `std::map<ItemType, int>` | association | `0..*` | `include/model/Recipe.hpp`, `inputs_`; enum key 기반 입력 | `Recipe --> ItemType` |
| `Recipe` | `ProductId` | `std::map<ProductId, int>` | association | `0..*` | `include/model/Recipe.hpp`, `outputs_`; 제품 객체가 아닌 ID 기반 출력 | 보통 생략 |
| `EventBus` | `Observer` | `std::vector<Observer*>` | association | `0..*` | `include/model/events/EventBus.hpp`, `observers_`; 비소유 raw pointer 구독 목록 | `EventBus --> Observer` |
| `EventLogObserver` | `Event` | `std::vector<Event>` | composition | `0..*` | `include/model/events/EventLogObserver.hpp`, `events_` | `EventLogObserver ◆-- Event` |
| `FactorySnapshot` | `InventorySnapshot` | `InventorySnapshot` | composition | `1` | `include/dto/FactorySnapshot.hpp`, `inventory_` | `FactorySnapshot ◆-- InventorySnapshot` |
| `FactorySnapshot` | `StatisticsSnapshot` | `StatisticsSnapshot` | composition | `1` | `include/dto/FactorySnapshot.hpp`, `statistics_` | `FactorySnapshot ◆-- StatisticsSnapshot` |
| `FactorySnapshot` | `ProductionLineSnapshot` | `std::vector<ProductionLineSnapshot>` | composition | `0..*` | `include/dto/FactorySnapshot.hpp`, `productionLines_` | `FactorySnapshot ◆-- ProductionLineSnapshot` |
| `FactorySnapshot` | `EventSnapshot` | `std::vector<EventSnapshot>` | composition | `0..*` | `include/dto/FactorySnapshot.hpp`, `events_` | `FactorySnapshot ◆-- EventSnapshot` |
| `InventorySnapshot` | `InventoryEntrySnapshot` | `std::vector<InventoryEntrySnapshot>` | composition | `0..*` | `include/dto/InventorySnapshot.hpp`, `items_` | `InventorySnapshot ◆-- InventoryEntrySnapshot` |
| `ProductionLineSnapshot` | `MachineSnapshot` | `std::vector<MachineSnapshot>` | composition | `0..*` | `include/dto/ProductionLineSnapshot.hpp`, `machines_` | `ProductionLineSnapshot ◆-- MachineSnapshot` |
# 7 : 소유, 의존 관계
| 사용하는 클래스 | 사용되는 클래스/struct/enum | 사용 위치 | 사용 방식 | UML 표시 여부 | 이유 |
|---|---|---|---|---|---|
| `FactoryController` | `CarbonationFactory` | `src/controller/FactoryController.cpp`, `createDefaultCarbonationFactory()` | `std::make_unique<CarbonationFactory>()`로 생성 | 표시 | Controller가 구체 Factory를 생성하는 핵심 의존 |
| `FactoryController` | `ProductCatalog` / `createProduct` | `src/controller/FactoryController.cpp`, `enqueueProductById()` | `createProduct(productId)` 호출 | 표시 | Controller → Product 생성 registry 의존 |
| `FactoryController` | `Product` | `src/controller/FactoryController.cpp`, `enqueueProductById()` | 생성된 `shared_ptr<Product>`를 Factory에 전달 | 표시 | 제품 생산 요청 흐름의 핵심 |
| `FactoryController` | `ProductionLine` | `src/controller/FactoryController.cpp`, `enqueueProductById()`, `enqueueAutoById()`, `snapshot()` | 라인 탐색, 최소 큐 라인 선택, snapshot 변환 | 표시 | Controller가 Model aggregate 내부를 조회 |
| `FactoryController` | `Machine` | `src/controller/FactoryController.cpp`, `forceBreak()`, `repairMachine()`, `makeMachineSnapshot()` | 기계 조회 후 명령 실행, DTO 변환 | 표시 | Controller → Model 명령/조회 |
| `FactoryController` | `Inventory` | `src/controller/FactoryController.cpp`, `makeInventorySnapshot()` | Inventory를 `InventorySnapshot`으로 변환 | 표시 | Model → DTO 변환 핵심 |
| `FactoryController` | `StatisticsObserver` / `Statistics` | `src/controller/FactoryController.cpp`, `makeStatisticsSnapshot()` | 통계 observer 값을 DTO로 변환 | 표시 | Model → DTO 변환 핵심 |
| `FactoryController` | `EventLogObserver` / `Event` | `src/controller/FactoryController.cpp`, `getEventLogs()`, `snapshot()` | 이벤트 로그 순회 후 DTO 생성 | 표시 | Event → DTO 변환 핵심 |
| `FactoryController` | `FactorySnapshot` | `src/controller/FactoryController.cpp`, `snapshot()` | 전체 snapshot 생성 및 반환 | 표시 | 중점 대상, Controller의 대표 조회 결과 |
| `FactoryController` | `InventorySnapshot` | `src/controller/FactoryController.cpp`, `makeInventorySnapshot()`, `snapshot()` | DTO 생성 | 표시 | FactorySnapshot 구성 요소 |
| `FactoryController` | `StatisticsSnapshot` | `src/controller/FactoryController.cpp`, `makeStatisticsSnapshot()`, `getStatistics()` | DTO 생성 | 표시 | FactorySnapshot 구성 요소 |
| `FactoryController` | `ProductionLineSnapshot` | `src/controller/FactoryController.cpp`, `makeProductionLineSnapshot()` | DTO 생성 | 표시 | Line Model → DTO 변환 |
| `FactoryController` | `MachineSnapshot` | `src/controller/FactoryController.cpp`, `makeMachineSnapshot()` | DTO 생성 | 표시 | Machine Model → DTO 변환 |
| `FactoryController` | `EventSnapshot` | `src/controller/FactoryController.cpp`, `getEventLogs()`, `snapshot()` | DTO 생성 | 표시 | Event → DTO 변환 |
| `FactoryController` | `FactoryMemento` | `src/controller/FactoryController.cpp`, `saveCheckpoint()`, `undo()` | `factory_->createMemento()`, `factory_->restoreFromMemento()` | 표시 | Controller → Memento workflow |
| `FactoryController` | `FactoryCommandResult` | `src/controller/FactoryController.cpp`, command methods | 성공/실패 enum 반환 | 표시 | Controller API 결과 타입 |
| `Factory` | `ProductionLine` | `src/model/Factory.cpp`, `addProductionLine()`, `removeProductionLine()`, `update()`, `restoreFromMemento()` | 라인 추가/삭제/업데이트/복원 | 표시 | Factory aggregate 핵심 의존 |
| `Factory` | `Product` | `src/model/Factory.cpp`, `enqueueProduct()`, `restoreFromMemento()` | 제품 요구사항 소비, 라인 큐에 전달, 복원 시 재생성 | 표시 | 생산 요청과 복원 흐름 핵심 |
| `Factory` | `Inventory` | `src/model/Factory.cpp`, `enqueueProduct()`, `update()`, `restoreFromMemento()` | 재료 consume, 완료 제품 add, 재고 복원 | 표시 | Factory와 Inventory 협력 |
| `Factory` | `ItemRequirement` | `src/model/Factory.cpp`, `enqueueProduct()` | `product->getRequirements()`로 소비 가능성 판단 | 표시 | Product 요구사항 → Inventory 소비 |
| `Factory` | `Machine` | `src/model/Factory.cpp`, `rebuildMachineCache()`, `findMachine()`, `createMemento()`, `restoreFromMemento()` | 라인 내부 기계를 캐시/조회/복원 | 표시 | aggregate 내부 탐색 및 Memento |
| `Factory` | `EventBus` | `src/model/Factory.cpp`, 생성자, `addProductionLine()` | observer 구독, 라인에 EventBus 전달 | 표시 | Event publish/subscribe 연결 핵심 |
| `Factory` | `EventLogObserver` | `src/model/Factory.cpp`, 생성자 | `eventBus_.subscribe(&eventLog_)` | 표시 | EventBus observer 등록 |
| `Factory` | `StatisticsObserver` | `src/model/Factory.cpp`, 생성자 | `eventBus_.subscribe(&statistics_)` | 표시 | EventBus observer 등록 |
| `Factory` | `FactoryMemento` | `src/model/Factory.cpp`, `createMemento()`, `restoreFromMemento()` | snapshot 생성/복원 | 표시 | Memento Originator 핵심 |
| `Factory` | `LineMemento` | `src/model/Factory.cpp`, `createMemento()`, `restoreFromMemento()` | 라인 단위 snapshot 생성/매칭 | 표시 | Memento 내부 구조 사용 |
| `Factory` | `MachineMemento` | `src/model/Factory.cpp`, `createMemento()`, `restoreFromMemento()` | 기계 상태 snapshot 생성/복원 | 표시 | Memento 내부 구조 사용 |
| `Factory` | `ProductId` | `src/model/Factory.cpp`, `restoreFromMemento()` | queue product id로 Product 재생성 | 선택 | ID 기반 의존, 클래스 관계보다는 데이터 의존 |
| `CarbonationFactory` | `ProductionLine` | `src/model/CarbonationFactory.cpp`, line 생성 helper, `addDynamicLine()`, `createLineForMemento()` | 생산라인 생성/복원 | 표시 | 구체 Factory가 line topology를 만듦 |
| `CarbonationFactory` | `MixingStation`, `QualityStation`, `BottlingStation`, `PackagingStation` | `src/model/CarbonationFactory.cpp`, line 생성 helper | `std::make_unique<...Station>()`로 기계 생성 | 표시 | 구체 설비 구성 생성 의존 |
| `CarbonationFactory` | `ProductCatalog` / `createProduct` | `src/model/CarbonationFactory.cpp`, 생성자, `createProductById()` | 기본 작업 enqueue 및 Memento 복원용 Product 생성 | 표시 | ProductCatalog → Product 생성 경로 사용 |
| `CarbonationFactory` | `ProductType` | `src/model/CarbonationFactory.cpp`, 생성자 | 기본 제품 `VoltzClassic` 생성 | 선택 | enum 값 의존 |
| `CarbonationFactory` | `FactoryMemento` | `src/model/CarbonationFactory.cpp`, `createMemento()`, `restoreFromMemento()` | next ID 추가 저장/복원 | 표시 | subclass Memento 확장 |
| `CarbonationFactory` | `LineMemento` | `src/model/CarbonationFactory.cpp`, `createLineForMemento()` | snapshot 기반 라인 재생성 | 표시 | Memento 복원 hook |
| `ProductionLine` | `Product` | `src/model/ProductionLine.cpp`, `enqueueProduct()` | Product로 `ProductionTask` 생성 | 표시 | 라인 큐 진입점 |
| `ProductionLine` | `ProductionTask` | `src/model/ProductionLine.cpp`, `enqueueProduct()`, `assignAvailableTask()`, `currentTask()` | 작업 생성, 큐 관리, 기계에 할당 | 표시 | 생산 흐름 핵심 |
| `ProductionLine` | `Machine` | `src/model/ProductionLine.cpp`, `addMachine()`, `assignAvailableTask()`, `update()`, `collectCompletedProducts()` | 기계 추가, 작업 배정, 완료 확인 | 표시 | Line → Machine 협력 핵심 |
| `ProductionLine` | `EventBus` | `src/model/ProductionLine.cpp`, `setEventBus()`, `enqueueProduct()` | EventBus 전달 및 이벤트 publish | 표시 | Model → Event 의존 |
| `ProductionLine` | `Event` | `src/model/ProductionLine.cpp`, `enqueueProduct()` | `Event(...)` 생성 후 publish | 표시 | 이벤트 발행 객체 |
| `ProductionLine` | `EventType` | `src/model/ProductionLine.cpp`, `enqueueProduct()` | `EventType::TaskEnqueued` 사용 | 표시 | 이벤트 종류 의존 |
| `Machine` | `ProductionTask` | `src/model/Machine.cpp`, `assignTask()`, `advanceProduction()` | 작업 수락/진행/완료 판단 | 표시 | Machine 핵심 동작 |
| `Machine` | `Recipe` | `src/model/Machine.cpp`, `setRecipe()`, `advanceProduction()` | recipe 기반 작업 처리 가능 | 표시 | Machine 처리 대상 |
| `Machine` | `MachineState` 계층 | `src/model/Machine.cpp`, 생성자, transition methods | `make_unique<IdleState/BrokenState/...>()`, `setState()` | 표시 | State 패턴 핵심 |
| `Machine` | `EventBus` | `src/model/Machine.cpp`, `setEventBus()`, `notify()` | EventBus 보관 후 publish 호출 | 표시 | Model → Event publish |
| `Machine` | `Event` | `src/model/Machine.cpp`, `notify()` | `Event(simulationTime_, type, id_, message)` 생성 | 표시 | 이벤트 발행 객체 |
| `Machine` | `EventType` | `src/model/Machine.cpp`, `notify()` 및 각 상태 변화 | TaskStarted, StepCompleted, MachineBroken 등 사용 | 표시 | 이벤트 종류 의존 |
| `Machine` | `MachineStatus` | `src/model/Machine.cpp`, transition/restore/pause/resume | 상태 값 변경/복원 | 표시 | 상태 모델 핵심 enum |
| `MachineState` 계층 | `Machine` | `src/model/MachineStates.cpp`, `update(Machine&, double)` | 상태 객체가 Machine을 갱신 | 표시 | State → Context 의존 |
| `ProductionTask` | `Product` | `src/model/ProductionTask.cpp`, 생성자/getter/currentStep | Product route와 id/name 조회 | 표시 | 작업이 제품 정의에 의존 |
| `ProductionTask` | `ProcessStep` | `src/model/ProductionTask.cpp`, `currentStep()` | 현재 공정 단계 반환 | 표시 | 생산 진행 핵심 |
| `Inventory` | `ItemRequirement` | `src/model/Inventory.cpp`, `hasEnough()`, `consume(vector<ItemRequirement>)` | 요구사항을 map으로 집계 후 소비 | 표시 | Product 요구사항을 재고에 반영 |
| `Inventory` | `ItemType` | `src/model/Inventory.cpp`, 전반 | map key, 수량 관리 | 선택 | enum 데이터 의존 |
| `Recipe` | `ItemType` | `src/model/Recipe.cpp`, `addInput()` | 입력 원자재 타입 저장 | 선택 | enum 데이터 의존 |
| `Recipe` | `ProductId` | `src/model/Recipe.cpp`, `addOutput()` | 출력 제품 ID 저장 | 선택 | ID 데이터 의존 |
| `Product` | `ProductDefinition` | `src/model/Product.cpp`, `Product(const ProductDefinition&)` | catalog definition으로 Product base 초기화 | 표시 | ProductCatalog 데이터 → Product 객체 |
| `Product` 구체 클래스 | `ProductCatalog` / `findProductDefinition` | `src/model/Product.cpp`, `definitionFor(ProductType)` | 제품 타입별 definition 조회 | 표시 | 구체 Product 생성 데이터 의존 |
| `ProductCatalog` | `ProductDefinition` | `src/model/ProductCatalog.cpp`, `productDefinitions()` | static catalog 데이터 생성/반환 | 표시 | catalog 핵심 데이터 |
| `ProductCatalog` | `CatalogProduct` | `src/model/ProductCatalog.cpp`, `createProduct()` | `make_shared<CatalogProduct>` | 표시 | Product 생성 구현 |
| `ProductCatalog` | `Product` | `src/model/ProductCatalog.cpp`, `createProduct()` | `shared_ptr<Product>` 반환 | 표시 | 중점 대상, factory dependency |
| `ProductCatalog` | `ItemRequirement`, `ProcessStep` | `src/model/ProductCatalog.cpp`, `productDefinitions()` | 제품 정의 내부 route/requirements 구성 | 표시 | 제품 데이터 구성 |
| `EventBus` | `Observer` | `src/model/events/EventBus.cpp`, `subscribe()`, `publish()` | observer 등록, `onEvent(event)` 호출 | 표시 | publish/subscribe 핵심 |
| `EventBus` | `Event` | `src/model/events/EventBus.cpp`, `publish(const Event&)` | 이벤트 전달 | 표시 | Event dispatch 핵심 |
| `EventLogObserver` | `Event` | `src/model/events/EventLogObserver.cpp`, `onEvent()` | 이벤트 저장 | 표시 | Observer 구현 |
| `StatisticsObserver` | `Event` | `src/model/events/StatisticsObserver.cpp`, `onEvent()` | 이벤트 타입별 통계 증가 | 표시 | Observer 구현 |
| `StatisticsObserver` | `EventType` | `src/model/events/StatisticsObserver.cpp`, `onEvent()` | switch로 이벤트 종류 분기 | 표시 | 이벤트 기반 통계 핵심 |
| `FactorySnapshot` | `InventorySnapshot`, `StatisticsSnapshot`, `ProductionLineSnapshot`, `EventSnapshot` | `include/dto/FactorySnapshot.hpp`, `src/dto/FactorySnapshot.cpp` | 생성자/추가 메서드로 하위 DTO 보유 | 표시 | DTO aggregate |
| `ProductionLineSnapshot` | `MachineSnapshot` | `include/dto/ProductionLineSnapshot.hpp`, `src/dto/ProductionLineSnapshot.cpp` | line DTO에 machine DTO 추가 | 표시 | DTO composition |
# 8 : Controller, DTO, Model 경계
| 요소                                                | 분류                             | 외부 노출 여부                    | 연결 대상                                                                                | 관계 종류                              | UML 표기 제안                                                                                                                                            |            |
| ------------------------------------------------- | ------------------------------ | --------------------------- | ------------------------------------------------------------------------------------ | ---------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| `FactoryController`                               | Controller                     | BE API로 노출                  | `CarbonationFactory`, `SimulationHistory`, DTO들                                      | Model 조작, DTO 생성, Memento 관리       | `Controller` 패키지에 배치. `FactoryController --> FactorySnapshot`, `FactoryController --> CarbonationFactory`, `FactoryController ◆-- SimulationHistory` |            |
| `ctrl::Controller`                                | 제외                             | View/WASM 전용 외부 API         | `FactoryController`, `ctrl::*View`                                                   | UI adapter/facade                  | BE UML에서는 제외. 별도 “WASM/UI Adapter” 다이어그램에만 표시                                                                                                        |            |
| `FactoryCommandResult`                            | Controller                     | `FactoryController` API 반환값 | `FactoryController`                                                                  | 명령 결과 enum                         | `Controller` 패키지 안 enum으로 표시                                                                                                                         |            |
| `SimulationHistory`                               | Controller / Memento caretaker | 직접 외부 노출 아님                 | `FactoryMemento`                                                                     | Memento stack 소유                   | `Controller` 또는 `Memento` 패키지에 배치. `SimulationHistory ◆-- FactoryMemento`                                                                            |            |
| `FactorySnapshot`                                 | DTO                            | 외부 노출                       | `InventorySnapshot`, `StatisticsSnapshot`, `ProductionLineSnapshot`, `EventSnapshot` | 값 복사 DTO aggregate                 | `DTO` 패키지. `FactoryController ..> FactorySnapshot`, `FactorySnapshot ◆-- 하위 DTO`                                                                     |            |
| `InventorySnapshot`                               | DTO                            | 외부 노출                       | `InventoryEntrySnapshot`                                                             | 값 복사 DTO                           | `DTO` 패키지. Model 참조 없음                                                                                                                               |            |
| `InventoryEntrySnapshot`                          | DTO                            | 외부 노출                       | 없음                                                                                   | 값 객체                               | `DTO` 패키지                                                                                                                                            |            |
| `StatisticsSnapshot`                              | DTO                            | 외부 노출                       | 없음                                                                                   | 값 객체                               | `DTO` 패키지                                                                                                                                            |            |
| `ProductionLineSnapshot`                          | DTO                            | 외부 노출                       | `MachineSnapshot`                                                                    | 값 복사 DTO                           | `DTO` 패키지. `ProductionLineSnapshot ◆-- MachineSnapshot`                                                                                              |            |
| `MachineSnapshot`                                 | DTO                            | 외부 노출                       | 없음                                                                                   | 값 객체                               | `DTO` 패키지                                                                                                                                            |            |
| `EventSnapshot`                                   | DTO                            | 외부 노출                       | `EventType`                                                                          | 이벤트 값 복사 DTO                       | `DTO` 패키지. Model `Event` 포인터/참조 없음                                                                                                                   |            |
| `Factory`                                         | Model                          | 직접 외부 노출 아님                 | `Inventory`, `ProductionLine`, `EventBus`, `Observer`, `SimClock`, `FactoryMemento`  | aggregate root, Memento originator | `Model` 패키지 중심. 외부는 `FactoryController`를 통해 접근                                                                                                       |            |
| `CarbonationFactory`                              | Model                          | 직접 외부 노출 아님                 | `Factory`, `Recipe`, `ProductionLine`, station classes, ProductCatalog               | 구체 Factory                         | `Model` 패키지. `CarbonationFactory --                                                                                                                  | > Factory` |
| `Inventory`                                       | Model                          | 직접 외부 노출 아님                 | `ItemRequirement`, `ItemType`, `ProductId`                                           | 재고 도메인 객체                          | `Model` 패키지. DTO 변환은 Controller가 수행                                                                                                                  |            |
| `ProductionLine`                                  | Model                          | 직접 외부 노출 아님                 | `ProductionTask`, `Machine`, `Product`, `EventBus`                                   | 생산라인 도메인 객체                        | `Model` 패키지. `ProductionLine ◆-- Machine`, `ProductionLine ◇-- ProductionTask`                                                                       |            |
| `Machine` 및 station 계층                            | Model                          | 직접 외부 노출 아님                 | `MachineState`, `ProductionTask`, `Recipe`, `EventBus`                               | 설비 도메인/State context               | `Model` 패키지. `Machine ◆-- MachineState`                                                                                                              |            |
| `Product` 및 제품 계층                                 | Model                          | 직접 외부 노출 아님                 | `ItemRequirement`, `ProcessStep`                                                     | 제품 도메인                             | `Model` 패키지. `Product` abstract로 표시                                                                                                                  |            |
| `ProductCatalog` 함수군 / `ProductDefinition`        | Model                          | 직접 외부 노출 아님                 | `Product`, `ProductDefinition`                                                       | 제품 registry/factory                | `Model` 패키지 또는 `Catalog` 하위 그룹. `ProductCatalog ..> Product`                                                                                         |            |
| `Event`, `EventBus`, `Observer` 계층                | Model                          | 직접 외부 노출 아님                 | `Machine`, `ProductionLine`, `Factory`                                               | Model 내부 event system              | `Model::Event` 하위 패키지. DTO와는 `FactoryController`가 변환                                                                                                 |            |
| `FactoryMemento`, `LineMemento`, `MachineMemento` | Model / Memento                | 직접 외부 노출 아님                 | `Factory`, `SimulationHistory`                                                       | snapshot 값 구조                      | `Memento` 패키지. `Factory ..> FactoryMemento`, `SimulationHistory ◆-- FactoryMemento`                                                                  |            |
| `SimClock`                                        | Model / Common                 | 직접 외부 노출 아님                 | `Factory`                                                                            | 시뮬레이션 시간 값 객체                      | `Common` 또는 `Model` 보조 클래스로 배치                                                                                                                       |            |
| `Types.hpp` enum들                                 | Common                         | 일부 API 타입으로 노출              | Controller/DTO/Model 전반                                                              | 공통 enum/value type                 | `Common` 패키지에 enum 박스 배치                                                                                                                             |            |

## 8-1 : MVC 관점 배치

```text
[Controller]
  FactoryController
  FactoryCommandResult
  SimulationHistory

        .. creates / returns ..

[DTO]
  FactorySnapshot
  InventorySnapshot
  ProductionLineSnapshot
  MachineSnapshot
  EventSnapshot
  StatisticsSnapshot

        .. reads / commands ..

[Model]
  Factory / CarbonationFactory
  ProductionLine / Machine / MachineState
  Product / Inventory / Recipe
  EventBus / Observer / Event
  FactoryMemento
  SimClock / common enums
```
# 9 : Factory, CarbonationFactory, ProductionLine, Machine, ProductionTask의 관계
## 9-1 : 핵심 구조 설명
`Factory`는 시뮬레이션의 aggregate root입니다. 내부에 `SimClock`, `Inventory`, 여러 `ProductionLine`, `EventBus`, `EventLogObserver`, `StatisticsObserver`를 직접 소유하고, `machines_`는 실제 소유가 아니라 `ProductionLine` 내부 `Machine`들을 빠르게 찾기 위한 raw pointer 캐시입니다.

`CarbonationFactory`는 `Factory`를 상속해 음료 공장 도메인으로 특화합니다. 생성자에서 초기 재고를 채우고, `MixingStation -> QualityStation -> BottlingStation -> PackagingStation` 4개 기계로 구성된 기본 `ProductionLine`을 만들며, `ProductCatalog::createProduct(ProductType::VoltzClassic)`로 시작 작업을 큐에 넣습니다. 또 동적 라인 생성과 Memento 복원 시 라인 topology 재생성을 담당합니다.

`ProductionLine`은 `std::deque<std::shared_ptr<ProductionTask>> taskQueue_`로 작업 큐를 관리하고, `std::vector<std::unique_ptr<Machine>> machines_`로 기계를 단독 소유합니다. `assignAvailableTask()`는 큐의 작업을 훑으며 현재 step의 `requiredRole()`을 처리할 수 있는 idle machine에 같은 `ProductionTask` shared_ptr를 할당합니다.

`Machine`은 현재 `ProductionTask`를 `shared_ptr`로 들고, 현재 상태 객체를 `std::unique_ptr<MachineState>`로 소유합니다. `Recipe`는 `std::optional<Recipe>`로 가질 수 있지만, 현재 핵심 생산 흐름은 `ProductionTask -> Product -> ProcessStep` 기반입니다. `Machine::update()`는 현재 `MachineState::update()`에 위임하고, `WorkingState`에서 `advanceProduction()`이 호출되어 step 진행, 완료, 고장, 이벤트 발행을 처리합니다.

`ProductionTask`는 `Product`를 추적합니다. 생성 방식에 따라 `ownedProduct_`가 `shared_ptr<Product>`를 소유하고, `product_` raw pointer는 실제 조회 대상으로 쓰입니다. 현재 공정 위치는 `currentStepIndex_`로 관리하며, `currentStep()`은 `product_->getRoute().at(currentStepIndex_)`를 반환합니다.

## 9-2 : Class Diagram용 관계 표

| 소스 | 대상 | 관계 | 다중성 | 근거 | Class Diagram 표시 |
|---|---|---|---|---|---|
| `CarbonationFactory` | `Factory` | 상속 | `1` | `class CarbonationFactory final : public Factory` | 일반화 화살표 |
| `Factory` | `SimClock` | composition | `1` | `Factory::clock_` | 표시 |
| `Factory` | `Inventory` | composition | `1` | `Factory::inventory_` | 표시 |
| `Factory` | `ProductionLine` | composition | `0..*` | `std::vector<ProductionLine> productionLines_` | 표시 |
| `Factory` | `Machine` | association/cache | `0..*` | `std::vector<Machine*> machines_` | 선택 표시 |
| `Factory` | `EventBus` | composition | `1` | `Factory::eventBus_` | 표시 |
| `Factory` | `EventLogObserver` | composition | `1` | `Factory::eventLog_` | 표시 |
| `Factory` | `StatisticsObserver` | composition | `1` | `Factory::statistics_` | 표시 |
| `CarbonationFactory` | `Recipe` | composition | `0..*` | `std::vector<Recipe> recipes_` | 표시 |
| `CarbonationFactory` | `MixingStation`, `QualityStation`, `BottlingStation`, `PackagingStation` | dependency/create | 각 라인당 1개씩 | `makeBeverageLine()`에서 `make_unique` | 점선 dependency |
| `CarbonationFactory` | `ProductCatalog/Product` | dependency/create | `0..*` | `createProduct(...)` 호출 | 점선 dependency |
| `ProductionLine` | `Machine` | composition | `0..*` | `std::vector<std::unique_ptr<Machine>> machines_` | 표시 |
| `ProductionLine` | `ProductionTask` | aggregation | `0..*` | `std::deque<std::shared_ptr<ProductionTask>> taskQueue_` | 표시 |
| `ProductionLine` | `EventBus` | association | `0..1` | `EventBus* eventBus_` | 표시 |
| `Machine` | `ProductionTask` | aggregation | `0..1` | `std::shared_ptr<ProductionTask> task_` | 표시 |
| `Machine` | `Recipe` | composition | `0..1` | `std::optional<Recipe> recipe_` | 선택 표시 |
| `Machine` | `MachineState` | composition | `1` | `std::unique_ptr<MachineState> state_` | 표시 |
| `Machine` | `EventBus` | association | `0..1` | `EventBus* eventBus_` | 표시 |
| `ProductionTask` | `Product` | aggregation/association | `1` | `shared_ptr<Product> ownedProduct_`, `const Product* product_` | 표시 |
| `ProductionTask` | `ProcessStep` | dependency | `0..1 current` | `currentStep()`가 Product route의 step 반환 | 점선 dependency |
| `Product` | `ProcessStep` | composition | `0..*` | `Product::route_` | Product 쪽에 표시 |
| `Product` | `ItemRequirement` | composition | `0..*` | `Product::requirements_` | Product 쪽에 표시 |

## 9-3 : Sequence Diagram 후보 흐름

1. **Tick / update 흐름**
```text
FactoryController::tick(delta)
  -> CarbonationFactory/Factory::update(delta)
    -> SimClock::update(delta)
    -> ProductionLine::assignAvailableTask() for each line
      -> Machine::canAcceptTask()
      -> ProductionTask::currentStep()
      -> Machine::canProcess(step.requiredRole())
      -> Machine::assignTask(task)
        -> Machine::transitionToWorking()
        -> EventBus::publish(TaskStarted / StateChanged)
    -> Machine::update(delta) for each cached machine
      -> MachineState::update(machine, delta)
      -> Machine::advanceProduction(delta)
        -> ProductionTask::advanceStep()
        -> EventBus::publish(StepCompleted / ProductCompleted / MachineBroken)
    -> ProductionLine::collectCompletedProducts()
    -> Inventory::addProduct(productId, 1)
    -> ProductionLine::assignAvailableTask()
```
이건 **sequence diagram에 가장 적합**합니다. 호출 순서와 상태 변화가 핵심입니다.

2. **제품 enqueue 흐름**
```text
FactoryController::enqueueProductById(lineId, productId)
  -> ProductCatalog::createProduct(productId)
  -> Factory::enqueueProduct(lineId, product)
    -> Factory::findProductionLine(lineId)
    -> Inventory::consume(product.requirements)
    -> ProductionLine::enqueueProduct(product)
      -> ProductionTask(product)
      -> EventBus::publish(TaskEnqueued)
```
이 흐름도 sequence diagram 후보입니다. Controller, Catalog, Factory, Inventory, Line, Event가 모두 만나는 좋은 시나리오입니다.

3. **CarbonationFactory 초기화 흐름**
```text
CarbonationFactory::CarbonationFactory()
  -> productDefinitions()
  -> Recipe 생성 및 recipes_ 저장
  -> Inventory::addItem(...)
  -> makeBeverageLine()
    -> ProductionLine(...)
    -> make_unique<MixingStation/QualityStation/BottlingStation/PackagingStation>
    -> ProductionLine::addMachine(...)
  -> createProduct(VoltzClassic)
  -> ProductionLine::enqueueProduct(...)
  -> Factory::addProductionLine(line)
    -> ProductionLine::setEventBus(&eventBus_)
    -> machines_ cache 구성
```
생성 책임을 보여주려면 sequence diagram, 구조만 보여주려면 class diagram의 dependency로 충분합니다.

4. **Machine step 처리 흐름**
```text
Machine::update(delta)
  -> current MachineState::update(machine, delta)
    -> WorkingState::update(...)
      -> Machine::advanceProduction(delta)
        -> ProductionTask::currentStep()
        -> progress 증가
        -> ProductionTask::advanceStep()
        -> task completed?
          -> EventBus::publish(ProductCompleted)
          -> Machine::transitionToIdle()
```
State 패턴 설명용 sequence diagram으로 좋습니다.

5. **Memento 복원 흐름**
```text
FactoryController::undo()
  -> SimulationHistory::pop()
  -> Factory::restoreFromMemento(memento)
    -> SimClock::setNow(...)
    -> Inventory::replaceContents(...)
    -> ProductionLine::clearQueue()
    -> Machine::resetForRestore(...)
    -> CarbonationFactory::createProductById(productId)
    -> ProductionLine::enqueueProduct(product)
```
# 10 : Product / ProductCatalog / Recipe / Inventory / Type enum 관계 조사
## 10-1 : 제품 도메인 구조 요약

제품 도메인은 `Types.hpp`의 ID alias/enum을 기반으로 합니다. `ProductCatalog`가 실제 제품 정의(`ProductDefinition`)를 중앙에서 보유하고, `createProduct()`가 `Product` 객체를 생성합니다. `Product`는 생산에 필요한 재료(`ItemRequirement`)와 공정 경로(`ProcessStep`)를 값으로 가지고, `Inventory`는 실제 `Item` 객체를 저장하지 않고 `ItemType -> quantity`, `ProductId -> quantity` 형태로 수량만 저장합니다.

| 요소 A | 요소 B | 관계 | 사용 방식 | UML 표시 여부 | 비고 |
|---|---|---|---|---|---|
| `Types.hpp` | `ItemId`, `ProductId`, `RecipeId`, `MachineId`, `LineId`, `ProductionLineId` | 공통 ID alias | 도메인 객체 식별자 타입 | 표시 | `Common` 패키지에 type alias로 정리 |
| `ItemType` | `Item` 계층 | enum 식별 | `Item::getTypeId()`에서 구체 재료 타입 반환 | 표시 | `Ingredient`, `Water`, `EmptyBottle`, `Label`, `Package` |
| `ItemType` | `Inventory` | key 관계 | `std::map<ItemType, int> items_` | 표시 | 원자재 재고 저장 방식 |
| `ItemType` | `ItemRequirement` | 값 필드 | 제품 생산에 필요한 재료 타입 | 표시 | `ItemRequirement::itemType_` |
| `ItemType` | `Recipe` | key 관계 | `std::map<ItemType, int> inputs_` | 표시 | Recipe 입력 재료 |
| `ItemType` | `Factory::restockItem()` | enum validation | 보충 가능한 원자재인지 switch로 검사 | 선택 | 새 재료 추가 시 수정 필요 |
| `ItemType` | `ItemTypeName` | static lookup | enum을 표시 이름으로 변환 | 선택 | UI/DTO 표시 보조 |
| `ProductType` | `ProductCatalog` | enum 식별 | `findProductDefinition(ProductType)`, `createProduct(ProductType)` | 표시 | 제품 타입 기반 생성 |
| `ProductType` | `ProductDefinition` | 값 필드 | `ProductDefinition::type` | 표시 | catalog entry의 제품 타입 |
| `ProductType` | 구체 `Product` 클래스 | 생성 기준 | `VoltzClassic`, `HyperBolt`, `AuroraZero` 생성자에서 definition 조회 | 표시 | 현재 구체 제품 클래스도 catalog 정의를 사용 |
| `ProductId` | `Product` | ID 관계 | `Product::id_`, `getProductId()` | 표시 | 완제품 식별자 |
| `ProductId` | `Inventory` | key 관계 | `std::map<ProductId, int> products_` | 표시 | 완제품 재고 저장 방식 |
| `ProductId` | `Recipe` | output key | `std::map<ProductId, int> outputs_` | 표시 | Recipe 출력 제품 |
| `ProductId` | `ProductionTask` | 조회 결과 | 완료 제품 ID 반환 | 표시 | `getProductId()` |
| `ProductId` | `FactoryMemento` | snapshot data | queue/product inventory 복원에 사용 | 표시 | Product 객체가 아니라 ID 저장 |
| `ProductCatalog` | `ProductDefinition` | composition | `productDefinitions()` 내부 static vector로 제품 정의 보유 | 표시 | 논리적 catalog 클래스처럼 표현 가능 |
| `ProductCatalog` | `Product` | factory dependency | `createProduct(ProductId/ProductType)`가 `shared_ptr<Product>` 반환 | 표시 | 핵심 생성 관계 |
| `ProductCatalog` | `CatalogProduct` | 생성 | `std::make_shared<CatalogProduct>(*definition)` | 선택 | cpp 내부 구현 클래스라 생략 가능 |
| `ProductCatalog` | `ItemRequirement` | composition | 각 `ProductDefinition::requirements` 구성 | 표시 | 제품별 재료 요구사항 |
| `ProductCatalog` | `ProcessStep` | composition | 각 `ProductDefinition::route` 구성 | 표시 | 제품별 공정 경로 |
| `Product` | `ItemRequirement` | composition | `std::vector<ItemRequirement> requirements_` | 표시 | 제품이 필요한 재료 목록 보유 |
| `Product` | `ProcessStep` | composition | `std::vector<ProcessStep> route_` | 표시 | 제품이 공정 단계 목록 보유 |
| `Product` | `ProductDefinition` | 생성 의존 | `Product(const ProductDefinition&)`로 id/name/requirements/route 복사 | 표시 | catalog data를 제품 객체로 변환 |
| `Product` | `Recipe` | 직접 관계 약함 | Product와 Recipe가 같은 catalog data에서 파생됨 | 선택 | 직접 멤버/호출 관계는 약함 |
| `Recipe` | `ItemType` | composition-like value | `inputs_`에 원자재 타입과 수량 저장 | 표시 | 값 기반 관계 |
| `Recipe` | `ProductId` | value relation | `outputs_`에 제품 ID와 수량 저장 | 표시 | Product 객체 직접 참조 아님 |
| `CarbonationFactory` | `Recipe` | composition | `std::vector<Recipe> recipes_` | 표시 | 현재 runtime 생산에는 거의 쓰이지 않고 inspection 성격 |
| `CarbonationFactory` | `ProductCatalog` | dependency | 생성자에서 `productDefinitions()`, `createProduct()` 호출 | 표시 | 초기 recipe 구성과 starter product 생성 |
| `Factory` | `Product` | dependency | enqueue 시 product requirements를 Inventory에 consume | 표시 | 생산 시작 흐름 |
| `Factory` | `Inventory` | composition/use | 원자재 소비, 완제품 추가 | 표시 | 제품 생산과 재고 연결 핵심 |
| `Inventory` | `ItemRequirement` | dependency | `consume(vector<ItemRequirement>)`에서 요구사항 집계 | 표시 | 제품 요구사항을 재고 차감으로 변환 |
| `ProcessStep` | `MachineRole` | enum field | `requiredRole_`로 필요한 station 역할 저장 | 표시 | ProductionTask/Machine 배정 기준 |
| `ProcessType` | `Machine` 계층 | enum return | 각 station의 `processType()` 반환값 | 표시 | 제품 route는 `MachineRole`을 쓰고, station 식별에는 `ProcessType`도 사용 |
| `MachineRole` | `ProcessStep` | enum field | 공정 단계가 필요한 기계 역할 지정 | 표시 | `ProductionLine::assignAvailableTask()`의 핵심 매칭 기준 |
| `MachineRole` | `Machine` 계층 | enum return/check | `Machine::canProcess(role)`, station별 `role()` | 표시 | 작업 배정 기준 |

## 10-2 : ProductCatalog의 Product 생성 방식

`ProductCatalog`는 `src/model/ProductCatalog.cpp`에서 `static const std::vector<ProductDefinition> definitions`를 갖고 있습니다. 각 definition에는 다음이 들어갑니다.

```cpp
ProductId id;
ProductType type;
std::string key;
std::string name;
std::string tier;
double totalDurationSeconds;
std::vector<ItemRequirement> requirements;
std::vector<ProcessStep> route;
```

`createProduct(ProductId)`는 ID로 definition을 찾고, 내부 구현 클래스 `CatalogProduct`를 `std::make_shared<CatalogProduct>(*definition)`로 만들어 `std::shared_ptr<Product>`로 반환합니다. 즉 외부는 `Product` 추상 타입만 보고, 실제 데이터는 catalog definition에서 복사됩니다.

## 10-3 : Inventory 저장 방식

`Inventory`는 `Item` 객체나 `Product` 객체를 소유하지 않습니다.

```cpp
std::map<ItemType, int> items_;
std::map<ProductId, int> products_;
```

원자재는 `ItemType` 기준 수량, 완제품은 `ProductId` 기준 수량으로 저장합니다. 제품 생산 시작 시 `Factory::enqueueProduct()`가 `product->getRequirements()`를 `Inventory::consume()`에 넘겨 원자재를 차감하고, 완료 시 `Inventory::addProduct(productId, 1)`로 완제품을 증가시킵니다.

## 10-4 : 새 제품 추가 시 영향 파일

| 변경 목적 | 영향 파일 | 해야 할 일 |
|---|---|---|
| 새 제품 enum 추가 | `include/common/Types.hpp` | `ProductType` 값 추가, 대응 `ProductId` 결정 |
| 제품 catalog data 추가 | `src/model/ProductCatalog.cpp` | `productDefinitions()`에 `ProductDefinition` 추가 |
| 제품 route 시간 상수 추가 | `include/common/Config.h` 또는 config 생성 원본 | total/step duration 상수 추가 |
| 구체 Product 클래스를 계속 유지한다면 | `include/model/Product.hpp`, `src/model/Product.cpp` | 새 Product subclass 추가 |
| Controller/API에서 제품 선택 지원 | `include/controller/FactoryController.hpp`, `src/controller/FactoryController.cpp` | 보통 `ProductId` 기반 API는 수정 적음, `ProductType` API는 enum만 영향 |
| UI adapter까지 포함한다면 | `include/controller/Controller.h`, `src/controller/Controller.cpp`, `src/web/bindings.cpp` | BE UML 제외 대상이지만 실제 UI 노출에는 필요 |

## 10-5 : 새 재료 추가 시 영향 파일

| 변경 목적                | 영향 파일                                          | 해야 할 일                                   |
| -------------------- | ---------------------------------------------- | ---------------------------------------- |
| 새 재료 enum 추가         | `include/common/Types.hpp`                     | `ItemType` 값 추가                          |
| 새 Item 클래스 추가        | `include/model/Item.hpp`, `src/model/Item.cpp` | 구체 `Item` subclass와 `getTypeId()` 구현     |
| 이름 변환 추가             | `src/model/Item.cpp`                           | `ItemTypeName::get()` switch 추가          |
| 초기 재고 추가             | `src/model/CarbonationFactory.cpp`             | 생성자에서 `inventory().addItem()` 추가         |
| restock 허용           | `src/model/Factory.cpp`                        | `Factory::restockItem()` switch에 새 타입 추가 |
| 제품 요구사항 반영           | `src/model/ProductCatalog.cpp`                 | 새 제품/기존 제품 `ItemRequirement`에 추가         |
| Memento/Inventory 저장 | 기존 구조 유지                                       | `map<ItemType, int>`라 구조 변경은 보통 불필요      |

# 11 : Memento 패턴 구조
## 11-1 : 패턴 역할별 클래스 표

| 역할 | 클래스/struct | 파일 | 설명 |
|---|---|---|---|
| Originator | `Factory` | `include/model/Factory.hpp`, `src/model/Factory.cpp` | 공장 상태를 `FactoryMemento`로 생성하고, 전달받은 memento로 상태를 복원 |
| Originator subclass | `CarbonationFactory` | `include/model/CarbonationFactory.hpp`, `src/model/CarbonationFactory.cpp` | `Factory`의 Memento 기능을 확장해 `nextLineId`, `nextMachineId`까지 저장/복원 |
| Memento | `FactoryMemento` | `include/model/memento/FactoryMemento.hpp` | Factory 전체 snapshot 데이터 |
| 하위 Memento | `LineMemento` | `include/model/memento/FactoryMemento.hpp` | 생산라인 단위 snapshot |
| 하위 Memento | `MachineMemento` | `include/model/memento/FactoryMemento.hpp` | 기계 단위 snapshot |
| Caretaker | `SimulationHistory` | `include/controller/SimulationHistory.hpp`, `src/controller/SimulationHistory.cpp` | `FactoryMemento` stack을 소유하고 push/pop 제공 |
| Facade | `FactoryController` | `include/controller/FactoryController.hpp`, `src/controller/FactoryController.cpp` | 외부에 `saveCheckpoint()`, `undo()`, `canUndo()`, `historySize()` 제공 |
| Restore helper | `ProductionLine` | `include/model/ProductionLine.hpp`, `src/model/ProductionLine.cpp` | queue/완료 목록 초기화, pending product id 제공 |
| Restore helper | `Machine` | `include/model/Machine.hpp`, `src/model/Machine.cpp` | `resetForRestore()`로 HP/status 복원, in-flight task 제거 |
| Restore helper | `Inventory` | `include/model/Inventory.hpp`, `src/model/Inventory.cpp` | `replaceContents()`로 재고 map 복원 |
| Restore helper | `SimClock` | `include/common/SimClock.hpp`, `src/common/SimClock.cpp` | `setNow()`로 시뮬레이션 시간 복원 |

`exportState()`, `restoreState()`라는 이름의 메서드는 현재 BE 코드에서 발견되지 않았습니다. 실제 대응 메서드는 `createMemento()` / `restoreFromMemento()`입니다.

## 11-2 : Memento struct 필드 표

| struct | 필드 | 타입 | 의미 |
|---|---|---|---|
| `MachineMemento` | `id` | `MachineId` | 복원 대상 기계 식별자 |
| `MachineMemento` | `health` | `double` | 저장 시점 HP |
| `MachineMemento` | `status` | `MachineStatus` | 저장 시점 기계 상태 |
| `LineMemento` | `id` | `LineId` | 생산라인 ID |
| `LineMemento` | `queueProductIds` | `std::vector<ProductId>` | 대기 중인 제품 ID 목록 |
| `LineMemento` | `machines` | `std::vector<MachineMemento>` | 라인에 속한 기계 snapshot 목록 |
| `FactoryMemento` | `simulationTime` | `SimulationTime` | 저장 시점 시뮬레이션 시간 |
| `FactoryMemento` | `items` | `std::map<ItemType, int>` | 원자재 재고 |
| `FactoryMemento` | `products` | `std::map<ProductId, int>` | 완제품 재고 |
| `FactoryMemento` | `lines` | `std::vector<LineMemento>` | 생산라인 snapshot 목록 |
| `FactoryMemento` | `nextLineId` | `LineId` | `CarbonationFactory`의 다음 라인 ID |
| `FactoryMemento` | `nextMachineId` | `MachineId` | `CarbonationFactory`의 다음 기계 ID |

현재 Memento 범위는 시간, 재고, 라인 큐, 기계 HP/status입니다. 주석상 **in-flight `ProductionTask`는 보존하지 않고**, 복원 시 기계는 task를 버리고 queue를 다시 채워 작업이 재개되도록 설계되어 있습니다. 통계와 이벤트 로그도 복원 대상이 아니라 “실제 지나온 기록”으로 유지됩니다.

## 11-3 : Memento 관련 메서드 표

| 클래스 | 메서드 | 역할 |
|---|---|---|
| `Factory` | `virtual FactoryMemento createMemento() const` | `clock_`, `inventory_`, `productionLines_`, `machines_` 상태를 snapshot으로 생성 |
| `Factory` | `virtual void restoreFromMemento(const FactoryMemento&)` | 시간/재고/라인/기계 상태/queue 복원 |
| `Factory` | `virtual std::shared_ptr<Product> createProductById(ProductId) const` | 복원 시 product id를 Product 객체로 재생성하기 위한 hook |
| `Factory` | `virtual std::optional<ProductionLine> createLineForMemento(const LineMemento&) const` | snapshot에 있는 라인이 현재 topology에 없을 때 재생성하는 hook |
| `CarbonationFactory` | `FactoryMemento createMemento() const override` | base memento 생성 후 `nextLineId`, `nextMachineId` 추가 |
| `CarbonationFactory` | `void restoreFromMemento(const FactoryMemento&) override` | base 복원 후 다음 ID 값 복원 |
| `CarbonationFactory` | `createProductById(ProductId) const override` | `ProductCatalog::createProduct(id)`로 제품 재생성 |
| `CarbonationFactory` | `createLineForMemento(const LineMemento&) const override` | memento의 machine id로 beverage line 재구성 |
| `SimulationHistory` | `void push(FactoryMemento)` | checkpoint 저장 |
| `SimulationHistory` | `std::optional<FactoryMemento> pop()` | 가장 최근 checkpoint를 꺼냄 |
| `SimulationHistory` | `bool canUndo() const` | stack 비어있는지 확인 |
| `SimulationHistory` | `void clear()` | stack 초기화 |
| `SimulationHistory` | `std::size_t size() const` | 저장된 checkpoint 개수 |
| `FactoryController` | `void saveCheckpoint()` | `history_.push(factory_->createMemento())` |
| `FactoryController` | `bool undo()` | `history_.pop()` 후 `factory_->restoreFromMemento(*m)` |
| `FactoryController` | `bool canUndo() const` | `history_.canUndo()` 위임 |
| `FactoryController` | `std::size_t historySize() const` | `history_.size()` 위임 |
| `ProductionLine` | `std::vector<ProductId> pendingProductIds() const` | queue를 product id 목록으로 저장 |
| `ProductionLine` | `void clearQueue()` | 복원 전 queue 제거 |
| `ProductionLine` | `void clearCompleted()` | 완료 목록 초기화 |
| `Machine` | `void resetForRestore(double, MachineStatus)` | task 제거, progress 초기화, HP/status/state 복원 |
| `Inventory` | `void replaceContents(...)` | 원자재/완제품 map 전체 교체 |
| `SimClock` | `void setNow(SimulationTime)` | 저장된 시간으로 이동 |

`SimulationHistory`는 **undo stack 하나만** 관리합니다. `std::vector<FactoryMemento> snapshots_`에 push하고, undo 시 back을 move해서 pop합니다. 현재 redo stack은 없습니다.

## 11-4 : DTO Snapshot과 Memento 분리

| 구분           | DTO Snapshot                                                                                                               | Memento                                                           |
| ------------ | -------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| 대표 타입        | `FactorySnapshot`, `ProductionLineSnapshot`, `MachineSnapshot`, `InventorySnapshot`, `EventSnapshot`, `StatisticsSnapshot` | `FactoryMemento`, `LineMemento`, `MachineMemento`                 |
| 목적           | 외부 조회/API/View 표시용                                                                                                         | 내부 상태 저장/복원용                                                      |
| 생성 위치        | `FactoryController::snapshot()`와 helper 함수들                                                                                | `Factory::createMemento()`, `CarbonationFactory::createMemento()` |
| 복원 가능 여부     | 복원에 사용하지 않음                                                                                                                | `restoreFromMemento()`로 복원                                        |
| 포함 데이터       | 현재 상태 + DTO용 표시 정보 + 이벤트/통계                                                                                                | 복원에 필요한 최소 상태                                                     |
| 이벤트/통계       | 포함해서 외부 조회 가능                                                                                                              | 의도적으로 저장/복원하지 않음                                                  |
| Model 포인터/참조 | 없음, 값 복사                                                                                                                   | 없음, 값 복사                                                          |
| 외부 노출        | BE Controller API로 노출                                                                                                      | `FactoryController` 내부 history에 저장, 외부 직접 노출 아님                   |

# 12 : Event, Observer 패턴 구조
| 요소 | 역할 | 연결 대상 | 관계 종류 | UML 표시 여부 | 이유 |
|---|---|---|---|---|---|
| `Event` | Event | `EventType`, `MachineId`, message, simulation time | 값 객체 | 표시 | Observer 패턴에서 전달되는 이벤트 payload |
| `EventType` | Event data | `Event`, `StatisticsObserver`, `Machine`, `ProductionLine` | enum dependency | 표시 | 이벤트 종류 분기와 발행에 핵심 |
| `EventBus` | EventBus | `Observer` | association, 비소유 참조 | 표시 | `std::vector<Observer*> observers_`로 구독자 포인터만 보관 |
| `Observer` | Observer | `Event` | interface dependency | 표시 | `onEvent(const Event&)` 순수 가상 메서드 |
| `EventLogObserver` | Subscriber / Observer | `Observer`, `Event` | inheritance + composition | 표시 | `Observer` 구현체, `std::vector<Event> events_`로 로그 저장 |
| `StatisticsObserver` | Subscriber / Observer | `Observer`, `EventType` | inheritance + dependency | 표시 | `Observer` 구현체, 이벤트 타입별 통계 카운터 증가 |
| `Factory` | Subscriber owner / wiring | `EventBus`, `EventLogObserver`, `StatisticsObserver` | composition + subscription | 표시 | `eventBus_`, `eventLog_`, `statistics_`를 소유하고 생성자에서 subscribe |
| `Factory` | EventBus owner | `EventBus` | composition | 표시 | EventBus 생명주기를 Factory가 소유 |
| `Factory` | Subscriber owner | `EventLogObserver`, `StatisticsObserver` | composition | 표시 | Observer 객체 생명주기를 Factory가 소유 |
| `Factory` | EventLog/Statistics accessor | `FactoryController` | read association | 선택 | DTO 변환을 위해 `eventLog()`, `statistics()` 제공 |
| `ProductionLine` | Publisher | `EventBus`, `Event`, `EventType::TaskEnqueued` | association + publish dependency | 표시 | `enqueueProduct()`에서 작업 enqueue 이벤트 발행 |
| `Machine` | Publisher | `EventBus`, `Event`, 여러 `EventType` | association + publish dependency | 표시 | 작업 시작/완료, 고장, 수리, 상태 전이 이벤트 발행 |
| `Machine` | EventBus reference holder | `EventBus*` | association, 비소유 참조 | 표시 | Factory가 라인에 EventBus를 주고, 라인이 Machine에 전달 |
| `ProductionLine` | EventBus reference holder | `EventBus*` | association, 비소유 참조 | 표시 | `setEventBus(EventBus*)`로 전달받아 보관 |
| `EventLogObserver` | DTO source | `EventSnapshot` | 변환 dependency | 표시 | `FactoryController::getEventLogs()`와 `snapshot()`에서 Event를 EventSnapshot으로 변환 |
| `StatisticsObserver` | DTO source | `StatisticsSnapshot` | 변환 dependency | 표시 | `FactoryController::getStatistics()`와 `snapshot()`에서 통계 DTO 생성 |
| `FactorySnapshot` | DTO aggregate | `EventSnapshot`, `StatisticsSnapshot` | composition | 표시 | 외부 조회용 snapshot에 이벤트/통계 포함 |
| `EventBus` | `EventLogObserver`, `StatisticsObserver` | 직접 소유 아님 | association만 | 표시 | 소유는 `Factory`, EventBus는 raw pointer 목록만 보유 |
| `FactoryController` | DTO converter | `EventLogObserver`, `StatisticsObserver`, `EventSnapshot`, `StatisticsSnapshot` | dependency | 표시 | Observer 결과를 외부 DTO로 노출하는 경계 |
| `EventLog` alias | Alias | `EventLogObserver` | type alias | 생략 가능 | `using EventLog = EventLogObserver`; 별도 클래스 아님 |
| `Statistics` alias | Alias | `StatisticsObserver` | type alias | 생략 가능 | `using Statistics = StatisticsObserver`; 별도 클래스 아님 |

## 12-1 : 이벤트 발행자

현재 실제 이벤트 발행자는 두 곳입니다.

| 발행자 | 발행 위치 | 이벤트 |
|---|---|---|
| `ProductionLine` | `src/model/ProductionLine.cpp`, `enqueueProduct()` | `TaskEnqueued` |
| `Machine` | `src/model/Machine.cpp`, `assignTask()`, `pause()`, `resume()`, `advanceProduction()`, `advanceMaintenance()`, `transitionToBroken()`, `onStateTransition()` | `TaskEnqueued`, `TaskStarted`, `MachinePaused`, `StepCompleted`, `ProductCompleted`, `MachineRepaired`, `MachineBroken`, `StateChanged` |

`Factory` 자체는 이벤트를 직접 publish하지 않고, `EventBus`와 subscriber를 소유하고 연결합니다.

## 12-2 : 구독 구조

`Factory::Factory()`에서 다음처럼 구독이 설정됩니다.

```cpp
eventBus_.subscribe(&eventLog_);
eventBus_.subscribe(&statistics_);
```

따라서 `EventBus`는 observer를 **소유하지 않습니다**. `Observer*` raw pointer 목록으로 참조만 하고, 실제 `EventLogObserver`, `StatisticsObserver`의 생명주기는 `Factory`가 값 멤버로 소유합니다.

## 12-3 : DTO Snapshot 연결

`EventLogObserver`와 `StatisticsObserver`는 DTO를 직접 알지 못합니다. DTO 변환은 `FactoryController`가 담당합니다.

```text
EventLogObserver::events()
  -> FactoryController::getEventLogs()
  -> std::vector<EventSnapshot>

StatisticsObserver counters
  -> FactoryController::getStatistics()
  -> StatisticsSnapshot

FactoryController::snapshot()
  -> FactorySnapshot에 StatisticsSnapshot과 EventSnapshot 목록 추가
```

즉 Observer 계층은 Model 내부 패턴이고, DTO는 Controller 계층에서 값 복사로 만들어집니다.
# 13 : Machine 상태 관리 구조
## 13-1 : 상태 클래스 목록

| 요소 | 구분 | 역할 | update 동작 | Class Diagram 표시 |
|---|---|---|---|---|
| `MachineState` | 실제 State Pattern 추상 상태 | 모든 기계 상태의 인터페이스 | `update(Machine&, double) = 0`, `name() const = 0`; `enter/exit`는 기본 no-op | 표시 |
| `IdleState` | 실제 State | 대기 상태 | `machine.hasTask()` 또는 `machine.recipe().has_value()`이면 `transitionToWorking("task available")` | 표시 |
| `WorkingState` | 실제 State | 작업 진행 상태 | `machine.advanceProduction(deltaTime)` 호출 | 표시 |
| `BrokenState` | 실제 State | 고장 상태 | 아무 동작 없음 | 표시 |
| `MaintenanceState` | 실제 State | 수리/정비 상태 | `machine.advanceMaintenance(deltaTime)` 호출 | 표시 |
| `MachineStatus` | 단순 enum 상태 | 외부 조회/조건 판단/DTO/Memento용 상태값 | 로직 객체는 아니고 `status_` 값으로 저장 | enum으로 표시 |
| `Machine` | State context | 현재 `MachineState` 객체와 `MachineStatus` 값을 함께 보유 | `update()`에서 현재 state 객체에 위임 | 표시 |

핵심은 **실제 행동은 `MachineState` 객체가 담당**하고, **`MachineStatus` enum은 조회, 저장, 복원, 조건 검사에 쓰이는 병행 상태값**이라는 점입니다.

## 13-2 : Machine의 상태 보유/전환 방식

`Machine`은 두 가지 상태 정보를 동시에 들고 있습니다.

```cpp
std::unique_ptr<MachineState> state_;
MachineStatus status_ = MachineStatus::Idle;
```

상태 객체 전환은 `setState(std::unique_ptr<MachineState>)`로 수행합니다. 기존 state의 `exit()`를 호출하고, 새 state를 저장한 뒤 `enter()`를 호출합니다. 실제 전이는 `transitionToIdle`, `transitionToWorking`, `transitionToBroken`, `transitionToMaintenance` private helper가 담당하며, 이때 `status_` enum과 `state_` 객체가 함께 바뀝니다.

## 13-3 : 상태 전이 표

| 시작 상태 | 트리거/메서드 | 조건 | 도착 상태 | 주요 동작 |
|---|---|---|---|---|
| 초기 | `Machine(...)` 생성자 | `initialHealth > 0` | `Idle` | `state_ = IdleState`, `status_ = Idle` |
| 초기 | `Machine(...)` 생성자 | `initialHealth <= 0` | `Broken` | `state_ = BrokenState`, `status_ = Broken` |
| `Idle` | `assignTask(task)` | `canAcceptTask()`, task step 존재, role 매칭 | `Working` | task 저장, progress 0, `TaskStarted`, `StateChanged` 이벤트 |
| `Idle` | `IdleState::update()` | task 또는 recipe 존재 | `Working` | `transitionToWorking("task available")` |
| `Working` | `WorkingState::update()` | 매 tick | `Working` 유지 또는 전이 | `advanceProduction(deltaTime)` |
| `Working` | `advanceProduction()` | HP damage로 `health_ <= 0` | `Broken` | progress 0, task 보존, `MachineBroken`, `StateChanged` |
| `Working` | `advanceProduction()` | 현재 step 완료, task 미완료 | `Idle` | `advanceStep()`, task reset, 다음 step 배정 대기 |
| `Working` | `advanceProduction()` | product 전체 완료 | `Idle` | `ProductCompleted`, task reset |
| `Idle`/`Working` | `pause()` | `Broken`, `Maintenance`가 아닐 때 | `Idle` | `MachinePaused` 이벤트 |
| `Idle`/`Working` | `resume()` | `Broken`, `Maintenance`가 아닐 때 | `Working` | `TaskStarted` 이벤트 |
| 임의 | `forceBreak()` | debug/test hook | `Broken` | task 제거 후 `transitionToBroken("forced breakdown")` |
| `Broken` 또는 임의 | `repair()` | 명시 호출 | `Maintenance` | maintenance 타이머 초기화 |
| `Maintenance` | `MaintenanceState::update()` | 수리 시간 미만 | `Maintenance` | `advanceMaintenance(deltaTime)`로 경과 시간 증가 |
| `Maintenance` | `advanceMaintenance()` | 수리 시간 완료, task 있음 | `Working` | HP 복구, task 재시작 |
| `Maintenance` | `advanceMaintenance()` | 수리 시간 완료, task 없음 | `Idle` | HP 복구, 정비 완료 |
| 임의 | `resetForRestore(health, Broken)` | Memento 복원 | `Broken` | task 제거, HP/status 복원 |
| 임의 | `resetForRestore(health, Maintenance)` | Memento 복원 | `Maintenance` | task 제거, maintenance state 복원 |
| 임의 | `resetForRestore(health, Working/Idle/기타)` | Memento 복원 | `Idle` | in-flight task는 복원하지 않음 |

참고로 `stop`은 `Machine` 상태 전이가 아니라 `SimClock`/Factory 시뮬레이션 제어 쪽입니다. `FactoryController::stopSimulation()`은 `Factory::stopClock()`을 통해 `SimClock::stop()`만 호출합니다.