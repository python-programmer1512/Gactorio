# Gactorio 구조 및 제출 점검 보고서

검토 기준:

- `factory_project_v3.pdf`
- `MVC-UI-Backend.pdf`
- `SOLID.pdf`
- `ImGui_OOP_CPP.pdf`
- 교수님 공지: today/tomorrow 수업은 프로젝트 Q&A 전용
- 현재 코드, `presentation/Script`, `presentation/UML_raw`, `presentation/UML`

## 1. 결론

현재 C++ Model/Controller 구조는 수업 자료의 MVC, SOLID, State/Observer/Memento 의도에 전반적으로 잘 맞습니다. 특히 Model이 View를 모르고, View는 `Module.Controller`만 통해 backend와 통신하는 구조는 좋은 설계입니다.

다만 엄격한 채점 기준으로 보면 다음 항목은 리스크였습니다.

1. `FactoryMemento`, `LineMemento`, `MachineMemento`가 `struct` 공개 필드로 되어 있어 public data member 금지 원칙에 걸릴 수 있었습니다. 이 항목은 코드에서 private field + accessor 구조로 수정했습니다.
2. 기존 Memento UML과 BE overall UML은 Memento를 공개 필드로 표시하고 있었습니다. 원본은 보존하고 `*_REVISED.drawio`와 bkp 수정본을 새로 만들었습니다.
3. 과제 PDF는 Dear ImGui UI를 명시하지만 현재 View는 JavaScript/DOM입니다. MVC 원칙은 만족하지만, UI 기술 자체가 ImGui 요구사항의 대체로 인정되는지는 교수님 Q&A 시간에 확인해야 합니다.
4. README는 `web/` 폴더와 `.github/workflows/deploy.yml`을 언급하지만 현재 실제 산출물은 `docs/`이고 root `.github/workflows/deploy.yml`은 없습니다. 제출 전 README를 `README_SUBMISSION_REVISED.md` 기준으로 정리하는 것이 좋습니다.
5. 과제 PDF의 scenario selector, Inspector, Force Break, Clear Log 요구사항은 현재 UI에서 완전한 ImGui checklist 형태로 구현되어 있지는 않습니다. 현재 기능은 pause/reset/speed, factory floor, event log, statistics, inventory/product controls에 집중되어 있습니다.

## 2. MVC 평가

### Model

Model은 `include/model`, `src/model`에 있습니다. 주요 class는 `Factory`, `ProductionLine`, `Machine`, `Product`, `Inventory`, `EventBus`, `MachineState`, `FactoryMemento`입니다.

좋은 점:

- Model이 JS, DOM, ImGui, Emscripten binding을 include하지 않습니다.
- `Factory::update()`는 `machines_` cache를 순회하며 `Machine::update()`를 호출합니다.
- `MachineState` hierarchy가 상태별 behavior를 분리합니다.
- Event logging/statistics는 Observer로 분리되어 있습니다.
- Memento 수정 후 내부 상태가 public field로 노출되지 않습니다.

주의점:

- `ProductDefinition`과 `ctrl::*View`는 public field struct입니다. 이는 domain simulation object가 아니라 catalog/view DTO이므로 허용 가능합니다. 발표 때 “DTO와 catalog definition은 value carrier이고, simulation entity는 private field로 캡슐화했다”고 설명하면 됩니다.

### Controller

Controller는 두 단계입니다.

- `gactorio::FactoryController`: backend use-case controller
- `ctrl::Controller`: View-facing facade, PImpl로 `gactorio::*`를 숨김

좋은 점:

- View-facing header인 `include/controller/Controller.h`가 `gactorio::*` Model header를 노출하지 않습니다.
- JS는 `Module.Controller`만 봅니다.
- snapshot은 `FactoryView` plain DTO로 복사되어 전달됩니다.

### View

View는 `docs/js`입니다.

좋은 점:

- `UIComponent` abstract base와 `AppUI` compositor가 수업 자료의 composable UI 구조와 대응됩니다.
- Panel class는 각각 하나의 화면 책임을 가집니다.
- 모든 command는 controller method 호출로 제한됩니다.
- snapshot rendering은 `toPlainSnapshot()` 이후 plain JS object를 사용합니다.

주의점:

- View가 DOM `innerHTML`을 사용하므로 `esc()` 사용이 필수입니다. 현재 제품명, 메시지, 타입 등 주요 문자열은 escape 처리되어 있습니다.
- `ProductsPanel`은 `products()` query를 `bind()` 시 한 번 읽습니다. 제품 catalog가 런타임에 바뀌지 않는다는 전제에서는 적절합니다.

## 3. SOLID 평가

- SRP: `Machine`, `ProductionLine`, `Inventory`, `EventBus`, `SimulationHistory`, 각 Panel의 책임이 대체로 분리되어 있습니다.
- OCP: 새 Panel은 `UIComponent` 상속 후 `app.addPanel()`로 추가 가능합니다. 새 machine type은 `Machine` 상속으로 추가 가능하지만 line construction code에는 등록이 필요합니다.
- LSP: concrete machine과 concrete state는 base interface를 통해 사용됩니다.
- ISP: `Observer`, `MachineState`, `UIComponent`가 작은 interface입니다.
- DIP: View는 concrete Model에 의존하지 않고 `Module.Controller` boundary에 의존합니다. Backend 내부에서는 `ProductionLine`이 `Machine` abstraction을 소유합니다.

## 4. UML 평가

기존 UML은 대부분 현재 C++ 구조와 일치합니다.

수정한 문제:

- Memento 관련 UML이 `+ simulationTime`, `+ id`처럼 공개 field로 표기되어 있었습니다.
- 코드도 같은 문제를 가지고 있었으므로 `FactoryMemento.hpp`를 private field/accessor 구조로 수정했습니다.
- 수정본 UML:
  - `presentation/UML_raw/Memento_Pattern_Diagram_REVISED.drawio`
  - `presentation/UML_raw/.$Memento_Pattern_Diagram_REVISED.drawio.bkp`
  - `presentation/UML_raw/BE_Overall_Class_Diagram_REVISED.drawio`
  - `presentation/UML_raw/.$BE_Overall_Class_Diagram_REVISED.drawio.bkp`

추가한 FE UML:

- `presentation/UML_raw/FE_JS_View_Class_Diagram.drawio`
- `presentation/UML_raw/.$FE_JS_View_Class_Diagram.drawio.bkp`
- `presentation/UML/FE_JS_View_UML_Info_KR.md`

UML 표기 규칙:

- 실선 + 빈 삼각형: inheritance/generalization
- 채워진 다이아몬드: composition/ownership
- 빈 화살표: association/call
- 점선 + 빈 화살표: dependency
- DOM으로 향하는 점선: rendering/event dependency

## 5. 발표 대본 평가

`OOPS_Scripts_KR.md`와 `OOPS_Scripts_EN.md`는 Backend UML 설명으로는 논리 흐름이 좋습니다.

문제점:

- 현재 최종 프로젝트는 JS View가 있으므로 Backend-only 발표로 끝나면 MVC View 구조 설명이 부족합니다.
- Memento가 opaque하다고 설명하지만 기존 UML/code에서는 공개 field였기 때문에 설명과 구현 사이에 긴장이 있었습니다. 이 부분은 코드와 수정 UML로 해결했습니다.

보완 파일:

- `presentation/Script/OOPS_Scripts_FE_JS_MVC_Addendum_KR.md`

이 파일을 발표 후반에 추가하면 JS View가 수업 MVC 구조를 어떻게 만족하는지 설명할 수 있습니다.

## 6. 제출/GitHub 점검

과제 PDF의 제출 항목:

- GitHub repository URL 제출
- `src/`, header/source 포함
- CMakeLists 또는 build file 포함
- UML/ER diagram 포함
- GitHub Pages live
- 팀원별 meaningful commit 필요

현재 상태:

- remote: `https://github.com/python-programmer1512/Gactorio.git`
- local `main`과 `origin/main`은 기존 커밋 기준으로 일치했지만, 이번 수정 사항은 아직 commit/push되지 않았습니다.
- `docs/`에 Pages 산출물 `index.html`, `gactorio.js`, `gactorio.wasm`가 있습니다.
- root `.github/workflows/deploy.yml`은 없습니다. Pages를 `/docs`로 직접 설정하는 방식이면 필수는 아니지만 README의 문장은 정리해야 합니다.
- untracked `factory_progress_report_JP.docx`가 있습니다. 제출물에 포함할지 결정해야 합니다.

## 7. 교수님 공지 반영

공지 내용은 새로운 기술 요구사항이 아니라 프로젝트 질문 시간 안내입니다. 하지만 현재 프로젝트에는 “JS View가 ImGui 요구사항을 대체할 수 있는가”라는 중요한 확인 사항이 있으므로, Q&A 시간에 반드시 확인하는 것이 좋습니다.

질문 추천:

1. GitHub Pages에서 실행되는 JS/DOM View가 Dear ImGui UI 요구사항을 대체해도 되는지
2. 현재 구현된 Normal flow + random breakdown 중심 구조가 scenario requirement를 충족하는지
3. UML에 FE JS View UML을 함께 제출해도 되는지

## 8. 검증 결과

- Native build: 성공
- `ctest --test-dir build --output-on-failure`: 12/12 passed
- JS syntax check: 주요 JS files 통과
- 새 drawio/bkp XML parse: 통과
