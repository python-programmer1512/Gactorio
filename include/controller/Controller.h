#pragma once

// =============================================================================
// ctrl::Controller — MVC 의 "Controller" 계층이자 View(JS)가 보는 유일한 경계
// =============================================================================
// ★ C++ ↔ JS(View) 연결의 핵심 파일 ★
//
// 이 헤더는 View(docs/js)가 controller/model 쪽에서 include 하도록 허용된 "유일한"
// 헤더다. 안에는 View 친화적 타입(문자열/숫자로만 된 plain struct = DTO)과,
// Model 을 완전히 숨기는 Controller 클래스만 있다(PImpl). 그래서 View 는 gactorio::*
// 심볼을 단 하나도 보지 못한다.
//
// 연결 방식(전체 그림):
//   JS(docs/js)  ──(embind 프록시)──▶  Module.Controller
//        │                                   │
//        │  controller.tick(dt)              │  ctrl::Controller 의 메서드
//        ▼                                   ▼
//   ctrl::Controller (이 파일)  ──(PImpl)──▶  gactorio::FactoryController ──▶ Model
//
//   * src/web/bindings.cpp 의 EMSCRIPTEN_BINDINGS 가 이 클래스와 아래 DTO 들을 JS에
//     노출한다. 빌드(build_web.ps1)하면 gactorio.js + gactorio.wasm 이 만들어지고,
//     JS 에서는 `new Module.Controller()` 로 이 객체를 만들어 쓴다.
//   * 명령(tick/pause/enqueue...)은 JS → Controller → FactoryController → Model 로 내려가고,
//   * 조회(snapshot/products)는 Model → FactorySnapshot → (여기서) FactoryView DTO 로
//     복사되어 JS 로 값-복사되어 올라온다. 포인터/가변 참조는 절대 넘어가지 않는다.
//
// Model 코드는 이 헤더를 절대 include 하지 않는다 — 의존은 한 방향(View→Controller→Model).
// =============================================================================

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ctrl {

// View 쪽 ID 는 uint32 로 둔다. embind 가 -sWASM_BIGINT 없이도 잘 다루며, 우리 ID 범위엔 충분.
using LineId    = std::uint32_t;
using MachineId = std::uint32_t;
using ProductId = std::uint32_t;
using ItemId    = std::uint32_t;

// View 가 쓰는 제품 종류 enum(gactorio::ProductType 의 거울). 값은 동일(101~103).
enum class ProductKind : std::uint32_t {
    Unknown      = 0,
    VoltzClassic = 101,
    HyperBolt    = 102,
    AuroraZero   = 103
};

// -----------------------------------------------------------------------------
// View 쪽 DTO (plain data) — 전부 문자열/숫자만. embind 의 value_object 로 JS 객체가 됨.
// 시뮬레이션 entity 가 아니라 "한 프레임의 화면 데이터"이므로 public 필드 허용.
// -----------------------------------------------------------------------------
struct MachineView {
    MachineId   id;
    std::string name;
    std::string type;       // "Mixing Station" 등
    std::string state;      // "Idle" / "Working" / "Broken" / "Maintenance"
    double      progress;   // 0.0 .. 1.0
    double      health;     // 0.0 .. 100.0
};

struct LineView {
    LineId      id;
    std::string name;
    std::size_t queueLength;            // 대기/진행 수
    std::string currentTaskName;        // 비어 있으면 유휴
    double      currentTaskProgress;    // 0.0 .. 1.0
    bool        isRemovable;            // true ⇒ 안전하게 삭제 가능
    std::vector<MachineView> machines;
};

struct EventView {
    double      time;       // 시뮬레이션 초
    std::string typeName;   // "TaskStarted" / "ProductCompleted" / ...
    std::string message;
};

struct InventoryEntry {
    ItemId      id;
    std::string name;       // "Ingredient", "Voltz Classic", ...
    int         quantity;
    bool        isProduct;  // false = 원자재, true = 완제품
};

struct ProductOption {
    ProductId   id;
    std::string key;        // 안정적 UI 키 (예: "VoltzClassic")
    std::string name;
    std::string tier;
    double      durationSeconds;
    std::string requirements; // "Ingredient x2, Water x1, ..."
};

struct Statistics {
    int tasksStarted     = 0;
    int stepsCompleted   = 0;
    int productsDone     = 0;
    int machinesBroken   = 0;
    int machinesRepaired = 0;
    int stateChanges     = 0;
};

// View 한 프레임에 필요한 모든 데이터를 담는 최상위 DTO.
struct FactoryView {
    double                       simulationTime = 0.0;
    Statistics                   stats;
    std::vector<LineView>        lines;
    std::vector<EventView>       events;
    std::vector<InventoryEntry>  inventory;
};

// -----------------------------------------------------------------------------
// Controller — View 와 Model 사이의 단일 접점.
// -----------------------------------------------------------------------------
class Controller {
public:
    Controller();
    ~Controller();
    Controller(const Controller&)            = delete;   // 단일 소유 객체(복사 금지)
    Controller& operator=(const Controller&) = delete;

    // ---- 명령(Model 상태 변경) ---------------------------------------------
    void tick(double deltaTime);                 // 시뮬레이션 한 틱 진행
    void pause();
    void resume();
    void reset();
    void setSpeed(double multiplier);            // 배속 설정
    bool enqueue      (LineId line,    ProductKind product);       // 특정 라인에 enqueue
    bool enqueueProduct(LineId line, ProductId product);
    // 현재 큐가 가장 짧은 라인에 enqueue.
    LineId enqueueAuto(ProductKind product);
    LineId enqueueAutoProduct(ProductId product);
    // 새 음료 라인 생성. 성공 시 LineId(실패 0).
    LineId addLine();
    // 라인 제거. 라인이 바쁘거나 없으면 false.
    bool   removeLine (LineId id);
    bool breakMachine (MachineId id);            // 강제 고장
    // 항상 가능한 빠른 수리: +config::kIncrementalRepairHp HP.
    bool repair       (MachineId id);
    bool instantRepair(MachineId id);
    // 원자재 1종을 5개 보충. 제품 ID는 거부.
    bool restockItem  (ItemId id);
    void setScenario(const std::string& scenario);
    std::string scenario() const;
    void clearEventLog();
    // 고장(HP=0)일 때만 의미 있음. 긴 정비 루틴을 돌려 HP 전량 회복 후 작업 재개.
    bool repairAll    (MachineId id);

    // ---- Memento (스냅샷 히스토리) -----------------------------------------
    void   saveCheckpoint();          // 현재 상태를 캡처해 스택에 push
    bool   undo();                    // 최근 스냅샷을 pop 해 복원
    bool   canUndo()     const;
    std::size_t historySize() const;

    // ---- 조회(Model 읽기, 캐시된 View 반환) --------------------------------
    const FactoryView& snapshot() const;                  // 현재 화면 데이터
    const std::vector<ProductOption>& products() const;   // 제품 카탈로그(불변)

private:
    struct Impl;                       // ★ PImpl: 모든 gactorio:: 타입을 이 안에 숨김
    std::unique_ptr<Impl> m_impl;      // 구현 세부는 Controller.cpp 에만 존재
};

} // namespace ctrl
