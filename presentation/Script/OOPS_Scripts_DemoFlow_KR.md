# Gactorio 발표 대본 — 데모 진행 순서판 (Demo Flow)

조원: 고원규(20265010), 조용빈(20265260)
최종 수정: 2026.06.13

> 이 대본은 교수님이 공지한 데모 진행 순서(1~4단계)에 정확히 맞춰 작성한 "진행용"
> 대본입니다. UML 다이어그램 설명 중심의 상세본은 `OOPS_Scripts_KR.md` 를, JS View
> 보완 설명은 `OOPS_Scripts_FE_JS_MVC_Addendum_KR.md` 를 함께 참고하세요.
>
> 데모 전 체크리스트:
> - [ ] 노트북에서 `docs/index.html` 을 정적 서버로 띄워(예: `python -m http.server`)
>       실제 동작 확인 (브라우저는 file:// 로는 .wasm 을 막음).
> - [ ] drawio 8개 다이어그램 이미지를 미리 띄워둠 (UML_raw → 이미지로 내보내기).
> - [ ] 발표 노트북·프로젝터·인터넷(GitHub Pages) 사전 점검.

---

## 0. 한 줄 요약 (오프닝 10초)

"Gactorio 는 에너지 드링크 공장을 시뮬레이션하는 C++ OOP 프로젝트입니다. Model 과
Controller 는 C++ 로 작성해 WebAssembly 로 빌드했고, View 는 HTML/CSS/JS 로 만들어
브라우저(GitHub Pages)에서 서버 없이 돌아갑니다. 오늘은 UI 시연 → 시나리오 →
클래스 구조(관계·확장성·SOLID) 순으로 발표하겠습니다."

---

## 1단계 — 애플리케이션 실행 & UI 시연 (1분)

(화면: 브라우저에서 Gactorio 페이지를 띄운 상태)

"먼저 실제로 동작하는 UI 를 보여드리겠습니다. 화면은 5개 영역으로 구성됩니다.

1. **Simulation Control(좌상단)**: 시뮬레이션 시간, Pause/Resume, Reset, 배속
   슬라이더(0.1×~5×), 그리고 Memento 기능인 Save Checkpoint / Undo 버튼이 있습니다.
2. **Factory(중앙)**: 생산라인이 컨베이어 벨트처럼 그려지고, 각 기계가 카드로
   표시됩니다. 카드에는 상태(색으로 구분), 진행률 막대, HP 막대가 있습니다.
3. **Products(우측)**: 제품 3종(Voltz Classic / Hyper Bolt / Aurora Zero) 버튼.
   누르면 큐가 가장 짧은 라인에 자동으로 생산 작업이 들어갑니다.
4. **Inventory(우측)**: 원자재와 완제품 재고. 원자재는 +5 버튼으로 보충할 수 있습니다.
5. **Event Log(좌하단)**: Observer 패턴으로 모인 이벤트가 최신순으로 쌓입니다.

(시연) Pause 를 눌러 멈추고, 배속을 올리고, 제품 버튼을 눌러 작업을 추가하면
기계가 Idle→Working 으로 바뀌며 진행률이 차오르고, 완료되면 Inventory 의 완제품
수량이 올라가는 것을 볼 수 있습니다. 통계 카운터도 함께 갱신됩니다."

(핵심 한마디) "이 UI 의 버튼은 절대 Model 을 직접 만지지 않습니다. 모두
`Module.Controller` 라는 단일 경계를 통해서만 명령을 보냅니다 — 이 점은 3단계에서
구조로 설명드리겠습니다."

---

## 2단계 — 구현한 시나리오 2가지 이상 설명 (1분)

"과제에서 요구한 시나리오 중 두 가지를 구현했습니다.

**시나리오 ① Normal flow (정상 생산 흐름)**
- 균형 잡힌 4단계 파이프라인입니다: Mixing → Quality → Bottling → Packaging.
- 제품을 enqueue 하면 ProductionLine 이 역할(role)이 맞는 유휴 기계에 작업을 배정하고,
  각 기계가 `update()` 로 자기 공정을 진행합니다. 한 단계가 끝나면 다음 역할의 기계가
  이어받고, 마지막 Packaging 까지 끝나면 완제품이 재고에 추가됩니다.
- 핵심 OOP 개념: **다형적 update()**. Factory 의 루프는 `Machine*` 기반 포인터로
  `update()` 만 호출할 뿐, 어떤 구체 기계인지 if/switch 로 따지지 않습니다.

**시나리오 ② Random breakdowns (무작위 고장 + 자동 정비)**
- 기계는 가동 중 매 틱 작은 확률로 HP 손상을 입습니다. HP 가 0 이 되면 자동으로
  Broken 상태가 되고 생산이 멈춥니다(이때 작업은 보존됩니다).
- Repair All 버튼을 누르면 Maintenance(정비) 상태로 들어가 일정 시간 후 HP 가 전량
  회복되고, 보존돼 있던 작업을 그 단계 처음부터 자동 재개합니다.
- 핵심 OOP 개념: **State machine**. Idle/Working/Broken/Maintenance 각 상태가 별도
  클래스이고, 전이 조건이 그 안에 캡슐화돼 있습니다.

(추가) Force Break 버튼으로 고장을 강제 유발해 시나리오 ②를 즉석에서 시연할 수
있습니다. Conveyor 의 용량 오버플로 시나리오는 과제 안내대로 생략하고, 위 두
시나리오에 집중했습니다."

---

## 3단계 — 클래스 다이어그램 설명 (13분)

> 진행 순서: (A) MVC 큰 그림 + C++↔JS 연결 → (B) is-a → (C) has-a →
> (D) dependencies → (E) 확장성 → (F) SOLID. 각 항목마다 해당 UML 을 띄웁니다.

### (A) MVC 전체 구조와 C++ ↔ JS 연결 — 2분
(UML: `BE_Overall_Class_Diagram`, `FE_JS_View_Class_Diagram`)

"이 프로젝트는 MVC 를 세 계층으로 나눴습니다.
- **Model** = `gactorio::*` (Factory, ProductionLine, Machine, Inventory, Product,
  EventBus, Memento ...). 시뮬레이션의 모든 상태와 규칙.
- **Controller** = `ctrl::Controller`. View 가 보는 단 하나의 경계.
- **View** = `docs/js` 의 Application / AppUI / UIComponent / 5개 Panel.

**C++ 과 JS 는 어떻게 연결되나요?** 핵심은 Emscripten 의 embind 입니다.
1. C++ 의 Model+Controller 를 `emcc` 로 빌드하면 `gactorio.wasm`(바이너리)과
   `gactorio.js`(글루)가 나옵니다.
2. `src/web/bindings.cpp` 의 `EMSCRIPTEN_BINDINGS` 블록이 `ctrl::Controller` 와 DTO
   들을 JS 에 노출합니다. 그래서 JS 에서 `new Module.Controller()` 로 C++ 객체를
   만들고 `controller.tick()`, `controller.snapshot()` 을 호출할 수 있습니다.
3. **명령**은 JS → `Module.Controller` → `ctrl::Controller` → `gactorio::FactoryController`
   → Model 로 한 방향으로 내려갑니다.
4. **조회**는 `snapshot()` 이 `FactoryView` 라는 plain 구조체(문자열·숫자만)를 값으로
   복사해 JS 에 돌려줍니다. 포인터나 가변 참조는 절대 넘기지 않습니다.

이 경계 덕분에 **View 의 어떤 JS 파일도 `Factory`·`Machine` 같은 Model 심볼을 직접
보지 못합니다.** `ctrl::Controller` 는 PImpl(`struct Impl`)로 모든 `gactorio::` 타입을
숨깁니다. 즉 컴파일러 수준에서 UI/백엔드 분리가 강제됩니다.

(과제 연계) 과제는 원래 Dear ImGui UI 를 명시했지만, 'UI와 백엔드를 단 하나의 파일만
양쪽을 본다'는 분리 원칙은 그대로 지켰습니다. 우리 프로젝트에서 그 '단 하나의 경계'가
바로 `Module.Controller`(=`ctrl::Controller`)입니다."

### (B) is-a 관계 (상속) — 2분
(UML: `Core_Simulation`, `Product_Domain`, `Observer_Event_Pattern`)

"상속(generalization, 빈 삼각형)은 다음 다섯 군데에 있습니다. 모두 '추상 root →
(필요 시 중간 추상) → 구체 클래스' 구조입니다.

1. **Machine** ◁ MixingStation / QualityStation / BottlingStation / PackagingStation.
   `Machine` 은 추상 클래스(순수 가상 `typeName()/role()/processType()/canAcceptRecipe()`).
2. **MachineState** ◁ IdleState / WorkingState / BrokenState / MaintenanceState.
3. **Product** ◁ VoltzClassic / HyperBolt / AuroraZero (+ 내부 CatalogProduct).
   이게 과제가 요구한 '제품 시작/끝을 나타내는 추상 제품 + 2개 이상 구체 제품'입니다.
4. **Item** ◁ Ingredient / Water / EmptyBottle / Label / Package.
5. **Observer** ◁ EventLogObserver / StatisticsObserver.
그리고 Factory 레벨에서도 **Factory** ◁ CarbonationFactory 상속이 있습니다.

과제 요구인 '최소 2단계 상속'은 Factory◁CarbonationFactory, Machine◁구체스테이션처럼
추상 base 와 구체 파생으로 충족합니다. View 쪽에서도 **UIComponent** ◁ 5개 Panel 로
같은 패턴이 반복됩니다."

### (C) has-a 관계 (composition / aggregation) — 2분
(UML: `BE_Overall`, `Core_Simulation`, `Memento_Pattern`)

"소유 관계는 두 종류로 구분했습니다.

**Composition(채운 다이아몬드 ◆ = 강한 소유, 수명 함께):**
- Factory ◆ ProductionLine / Inventory / SimClock / EventBus / EventLog / Statistics
- ProductionLine ◆ Machine (`vector<unique_ptr<Machine>>`)
- Machine ◆ MachineState (`unique_ptr<MachineState>`) — State 패턴의 현재 상태
- Product ◆ ItemRequirement / ProcessStep (`vector`)
- FactoryMemento ◆ LineMemento ◆ MachineMemento
- SimulationHistory ◆ FactoryMemento (undo 스택)

**Aggregation / 공유(빈 다이아몬드 ◇ 또는 비소유 포인터):**
- Machine ◇ ProductionTask (`shared_ptr` — 큐와 기계가 같은 작업을 공유)
- ProductionLine ◇ ProductionTask (`deque<shared_ptr>` 큐)
- ProductionTask ◇ Product (`shared_ptr` 또는 비소유 `const Product*`)
- Factory 의 `machines_: vector<Machine*>` 는 라인 소속 기계를 가리키는 비소유 캐시

핵심: 소유는 `unique_ptr`/값 멤버로, 공유는 `shared_ptr` 로, 단순 참조는 raw
포인터(`Machine*`, `EventBus*`)로 의도를 명확히 표현했습니다."

### (D) dependencies (의존) — 1.5분
(UML: 점선 화살표들)

"의존(dependency, 점선 화살표)은 '소유하진 않지만 호출/생성/사용하는' 관계입니다.
- ProductCatalog ⇢ Product : 팩토리 함수 `createProduct()` 가 Product 를 생성.
- CarbonationFactory ⇢ ProductCatalog : ID 로 Product 를 만들 때 위임.
- Factory ⇢ FactoryMemento : `createMemento()/restoreFromMemento()`.
- FactoryController ⇢ DTO(FactorySnapshot 등) : Model 을 읽어 스냅샷을 '생성'.
- Machine / ProductionLine → EventBus* : 이벤트 발행 호출(비소유 association).
- (View) Panel ⇢ Module.Controller(명령) / FactoryView(렌더 데이터) / DOM(innerHTML).

의존 방향이 **한 방향**이라는 점이 중요합니다: View → Controller → {DTO, Model},
Model → Model/events. Model 은 Controller·DTO·View 헤더를 절대 include 하지 않습니다.
실제로 백엔드 라이브러리는 ImGui·GLFW·브라우저 그 어떤 UI 코드와도 링크하지 않습니다."

### (E) 코드 확장·수정 용이성 — 2분

"새 기능을 추가할 때 기존 루프를 건드리지 않도록 설계했습니다.

- **새 기계 타입 추가**: `Machine` 을 상속해 4개 가상 함수만 구현하고, 라인 구성
  코드에 등록만 하면 됩니다. `Factory::update()` 와 `ProductionLine::assignAvailableTask()`
  의 루프는 그대로 — 역할(role)로만 매칭하기 때문입니다.
- **새 제품 추가**: `ProductCatalog` 의 정의표(productDefinitions)에 항목 하나만
  추가하면 됩니다. 곳곳의 if/switch 수정이 필요 없습니다.
- **새 상태 추가**: `MachineState` 를 상속한 클래스를 만들고 전이 지점만 연결합니다.
  Machine 의 update 가 거대한 switch 문이 아니므로 안전합니다.
- **새 통계/알림 추가**: `Observer` 를 구현해 EventBus 에 구독시키면 끝. 발행자
  (Machine/Line)는 전혀 바꾸지 않습니다.
- **새 화면(View 패널) 추가**: `UIComponent` 를 상속하고 `app.addPanel()` 로 등록.
  AppUI 나 기존 패널 코드는 불변입니다.

이처럼 '추가는 새 클래스로, 수정은 최소로'가 우리 설계의 일관된 원칙입니다."

### (F) SOLID 원칙 충족 — 1.5분

"- **S (단일 책임)**: Machine=공정 진행, ProductionLine=배정, Inventory=재고,
  EventBus=발행, EventLog/Statistics=기록·집계, SimClock=시간, SimulationHistory=
  스냅샷 보관. 각자 책임이 하나로 분리돼 있습니다.
- **O (개방-폐쇄)**: 위 (E)에서 보았듯, 기계·제품·상태·Observer·패널 모두 '상속+등록'
  으로 추가되고 핵심 루프는 닫혀 있습니다.
- **L (리스코프 치환)**: Factory 루프는 `Machine*` 로, Machine 은 `MachineState` 로,
  EventBus 는 `Observer*` 로만 다룹니다. 어떤 구체 타입을 넣어도 동작이 깨지지 않습니다.
- **I (인터페이스 분리)**: `Observer`(onEvent 1개), `MachineState`(enter/update/exit/
  name), `UIComponent`(bind/render) — 모두 작고 목적이 분명한 인터페이스입니다.
- **D (의존성 역전)**: View 는 구체 Model 이 아니라 `Module.Controller` 경계에
  의존합니다. 백엔드 내부에서도 ProductionLine 은 구체 기계가 아니라 `Machine`
  추상에 의존합니다."

(소결) "정리하면, 캡슐화(모든 시뮬레이션 클래스는 private 멤버 + 메서드 접근, public
data member 없음), 다형성(타입 분기 없는 update 루프), 상속·추상화(5개 계층), 그리고
3대 디자인 패턴(State/Observer/Memento)이 SOLID 원칙과 함께 맞물려 있습니다."

---

## 4단계 — Q&A 대비 (5분)

예상 질문과 답변 요지:

- **Q. ImGui 가 아니라 JS 로 UI 를 만들어도 되나요?**
  A. 과제의 핵심인 'UI–백엔드 분리'와 'MVC, 단일 경계 통신'은 그대로 지켰습니다.
  C++ Model/Controller 를 wasm 으로 빌드하고, 단 하나의 경계(`Module.Controller`)로만
  통신합니다. UI 기술만 DOM 으로 대체했고 사전에 허락도 받았습니다.

- **Q. 시뮬레이션 루프에 정말 타입 분기가 없나요?**
  A. `Factory::update()` 와 `ProductionLine::assignAvailableTask()` 에 `dynamic_cast`
  나 `typeName()=="..."` 분기가 전혀 없습니다. 작업 배정은 `MachineRole` 비교로만
  합니다. (코드를 직접 보여드릴 수 있습니다.)

- **Q. public data member 가 정말 없나요?**
  A. 시뮬레이션 entity(Machine/Product/Inventory/Factory/Memento ...)는 모두 private
  필드 + 접근자입니다. 단, `ProductDefinition` 과 `ctrl::*View` DTO 는 시뮬레이션
  객체가 아니라 '값 운반용 카탈로그/화면 데이터'라 public 필드를 허용했습니다.

- **Q. Memento 가 스냅샷(DTO)과 뭐가 다른가요?**
  A. DTO 는 화면 표시용 읽기 데이터이고, Memento 는 복원 전용 저장 데이터입니다.
  Undo 에는 FactoryMemento 만 쓰며, 진행 중 작업·이벤트 로그·통계는 일부러 복원하지
  않습니다(역사는 되돌리지 않음).

- **Q. C++ 와 JS 사이에서 메모리 누수는 없나요?**
  A. embind vector 핸들은 GC 대상이 아니라, `util.js` 의 `vecToArray` 가 변환 후
  `delete()` 로 C++ 할당을 즉시 해제합니다. snapshot 은 값-복사라 핸들이 남지 않습니다.

- **Q. 팀 기여는?**
  A. 고원규: FE/BE/PM, 조용빈: BE. 커밋 히스토리로 확인 가능합니다.

"이상으로 발표를 마치겠습니다. 감사합니다."

---

## 부록 — UML 8종과 코드 대응 요약 (그래더 매칭용)

| UML 파일 | 보여줄 때 | 코드 대응 |
| --- | --- | --- |
| BE_Overall_Class_Diagram | (A) 큰 그림 | include/src 전체 Model/Controller/DTO |
| Core_Simulation_Class_Diagram | (B)(C) | Factory/ProductionLine/Machine/Task/Product/State |
| Product_Domain_Class_Diagram | (B) | Product/Item/Recipe/Inventory/ProductCatalog |
| Machine_State_Diagram | 2단계·(B) | MachineState 4종 + Machine 전이 함수 |
| Observer_Event_Pattern_Diagram | (B)(D) | EventBus/Observer/EventLog/Statistics/Event |
| Memento_Pattern_Diagram | (C)(D) | FactoryMemento/Line/Machine + SimulationHistory |
| Tick_Update_Sequence_Diagram | 2단계 | Factory::update() 호출 흐름 |
| FE_JS_View_Class_Diagram | (A) | docs/js/* + Module.Controller 경계 |
