# UML ↔ 코드 일치성 엄밀 분석 (2026-06-13)

검토 대상: `presentation/UML_raw/*.drawio` 8종과 현재 C++/JS 코드.
방법: 각 다이어그램의 클래스 박스(필드/메서드/접근지정자)와 모든 화살표(source→target,
화살촉 종류)를 코드의 실제 선언과 1:1로 대조. drawio XML을 파싱하여 모든 edge가 실제 노드를
가리키는지(dangling edge 없음)까지 자동 검증함.

## 1. 최종 다이어그램 목록 (8종, 모두 코드와 일치)

| 파일 | 종류 | 대응 코드 | 상태 |
| --- | --- | --- | --- |
| `BE_Overall_Class_Diagram.drawio` | 클래스 | Model/Controller/DTO/Common 전체 | 수정 완료 |
| `Core_Simulation_Class_Diagram.drawio` | 클래스 | Factory/Line/Machine/Task/Product/State | 일치 (변경 없음) |
| `Product_Domain_Class_Diagram.drawio` | 클래스 | Product/Item/Recipe/Inventory/Catalog | 일치 (변경 없음) |
| `Memento_Pattern_Diagram.drawio` | 클래스 | FactoryMemento/LineMemento/MachineMemento + Caretaker | 수정 완료 |
| `Observer_Event_Pattern_Diagram.drawio` | 클래스 | EventBus/Observer/EventLog/Statistics/Event | 수정 완료 |
| `Machine_State_Diagram.drawio` | 상태 | MachineState 4종 + 전이 | 수정 완료 |
| `Tick_Update_Sequence_Diagram.drawio` | 시퀀스 | `Factory::update()` 호출 흐름 | 일치 (변경 없음) |
| `FE_JS_View_Class_Diagram.drawio` | 클래스 | `docs/js/*` View + Controller 경계 | 신규 검증 (VIEW UML) |

> 참고: 이전에 임시로 만든 `*_REVISED.drawio` 와 그 `.bkp` 백업은 정식 파일에 통합한 뒤
> 삭제했습니다. 이제 다이어그램 1종당 파일 1개만 존재하여, 채점 시 "어느 파일이 정본인가"
> 혼동이 없습니다.

## 2. 발견하여 수정한 불일치 (총 7건)

### 2.1 Memento public field → private + accessor (가장 중요)
- 문제: 기존 `Memento_Pattern_Diagram.drawio` 와 `BE_Overall` 의 Memento 박스가
  `+ simulationTime`, `+ items` 처럼 **public 필드**로 표기되어 있었음.
- 과제 요구: "a code reviewer must not be able to find a public data member on any
  simulation class" (public data member 금지).
- 코드 현황: `include/model/memento/FactoryMemento.hpp` 는 이미 `private:` 필드 +
  `id()/health()/status()/...` accessor 구조. 즉 **코드가 옳고 UML이 틀렸던** 케이스.
- 조치: 두 다이어그램의 Memento 박스를 `- id_ : MachineId` + `+ id() const` 형태로
  교정. `UML_Info_BE_kor.md` 의 Memento 변수 행도 private + accessor로 갱신.

### 2.2 BE_Overall: 잘못 연결된 composition 화살표 3건
편집 중 좌표가 어긋나 source/target이 엉킨 edge를 코드 기준으로 바로잡음.
- `e-machine-state` (state_ 소유): source가 **EventBus** → **Machine** 으로 교정.
  실제 코드는 `Machine` 이 `state_: unique_ptr<MachineState>` 를 소유함.
- 떠돌이 edge `Machine ◆→ ProductCatalog`: 실제 그런 소유 관계 없음 → **삭제**.
- 떠돌이 edge `ProductCatalog ◆→ EventBus`: 실제 그런 소유 관계 없음 → **삭제**.

### 2.3 Observer 다이어그램: observers_ 연결 오류 2건
- `observers_ (non-owning)` 연결선의 source가 **EventType** 으로 잘못 지정됨 →
  **EventBus** 로 교정. 코드: `EventBus::observers_: vector<Observer*>`.
- 떠돌이 edge `EventBus →(빈 라벨) EventType`: 의미 없음 → **삭제**.
  (EventType 사용은 `Event.type_`, `StatisticsObserver::onEvent`의 switch로만 표현됨.)

### 2.4 Machine 상태 다이어그램: 전이 오류 2건 + 초기 의사상태 보강
- `e-maintenance-working` (수리 완료 후 작업 재개): source가 **Broken** 으로 잘못
  지정됨 → **Maintenance** 로 교정. 코드: `MaintenanceState::update` →
  `advanceMaintenance()` → task가 남아있으면 `transitionToWorking(...)`.
- 떠돌이 edge `Maintenance →(빈 라벨) Broken`: 코드에 없는 전이 → **삭제**.
- 생성자 초기 전이가 `Working` 박스에서 나가는 것으로 그려져 혼동을 주어, UML 표준의
  **initial pseudostate(검은 점)** 노드를 추가하고 `[health>0]→Idle`,
  `[health<=0]→Broken` 으로 정리. 코드: `Machine::Machine(...)` 에서 health>0이면
  IdleState, health<=0이면 BrokenState.

## 3. 일치 확인된 핵심 관계 (코드 근거)

- **is-a (상속, 빈 삼각형)**: `MixingStation/QualityStation/BottlingStation/
  PackagingStation : Machine`, `Idle/Working/Broken/MaintenanceState : MachineState`,
  `VoltzClassic/HyperBolt/AuroraZero/CatalogProduct : Product`,
  `Ingredient/Water/EmptyBottle/Label/Package : Item`,
  `EventLogObserver/StatisticsObserver : Observer`,
  `CarbonationFactory : Factory`. (모두 `include/model/*` 헤더의 `: public` 일치)
- **has-a (composition, 채운 다이아몬드)**:
  `Factory ◆ ProductionLine/Inventory/SimClock/EventBus/EventLog/Statistics`,
  `ProductionLine ◆ Machine(unique_ptr)`, `Machine ◆ MachineState(unique_ptr)`,
  `Product ◆ ItemRequirement/ProcessStep(vector)`,
  `FactoryMemento ◆ LineMemento ◆ MachineMemento`,
  `SimulationHistory ◆ FactoryMemento`. (헤더의 멤버 선언과 일치)
- **aggregation (빈 다이아몬드)**: `Machine ◇ ProductionTask(shared_ptr task_)`,
  `ProductionLine ◇ ProductionTask(deque)`, `ProductionTask ◇ Product`. (공유 소유)
- **dependency (점선 화살표)**: `Factory ⇢ FactoryMemento(create/restore)`,
  `ProductCatalog ⇢ Product(createProduct)`, `Controller ⇢ DTO(snapshot 생성)`.
- **non-owning association (실선 열린 화살표)**: `Machine/ProductionLine → EventBus*`,
  `EventBus → Observer*`, `Factory.machines_: vector<Machine*>` 캐시.

## 4. VIEW UML (`FE_JS_View_Class_Diagram.drawio`)

JS View 계층의 UML이 없어 새로 작성/검증함. 코드와의 대응:
- `UIComponent`(추상, `bind()/render()`) ← 5개 Panel 상속. (`docs/js/UIComponent.js`)
- `AppUI ◆ UIComponent[] (#panels)`, `Application ◆ AppUI (#ui)`.
- `Application → Module.Controller`(`tick()/snapshot()`), Panel ⇢ Controller(commands),
  Panel ⇢ FactoryView/ProductOption(render용 read-only DTO), Panel ⇢ DOM(innerHTML/이벤트).
- `Module.Controller --embind--> ctrl::Controller --(PImpl)--> gactorio::* Model`.
- MVC 매핑: Model=`gactorio::*`, Controller=`ctrl::Controller`/`Module.Controller`,
  View=`Application/AppUI/UIComponent/Panels`.

## 5. 자동 검증 결과
- 8개 drawio 전부 XML 파싱 성공.
- 8개 drawio 전부 모든 edge의 source/target이 실제 노드를 가리킴 (dangling edge 0건).
