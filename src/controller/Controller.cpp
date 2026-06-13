#include "controller/Controller.h"

// =============================================================================
// Controller.cpp — ctrl::Controller 구현 = "Model ↔ View(JS) 번역기"
// -----------------------------------------------------------------------------
// 이 파일만 gactorio:: Model 을 안다. View 친화 타입(ctrl::*)과 Model 타입(gactorio::*)
// 사이의 모든 변환이 여기서 일어난다. PImpl(struct Impl)이 모든 gactorio:: 멤버를 숨겨,
// Controller.h 만 include 하는 View(JS)는 Model 심볼을 전혀 못 본다.
//
// 데이터 흐름:
//   명령: JS → ctrl::Controller(이 파일) → gactorio::FactoryController → Model
//   조회: snapshot() 시 backend 스냅샷(gactorio DTO) → ctrl::FactoryView(plain) 로 복사
//         (dirty 플래그로 틱마다 1번만 재구성하여 캐시)
// =============================================================================

// 이 .cpp 만이 gactorio 모델을 안다. ctrl::* 와 gactorio::* 의 번역이 여기 모인다.
#include "common/Types.hpp"
#include "controller/FactoryController.hpp"
#include "model/Item.hpp"
#include "model/ProductCatalog.hpp"

#include <sstream>
#include <stdexcept>

namespace ctrl {
namespace {

// View 의 제품 종류(ProductKind) → Model 의 ProductId 로 변환(값이 동일).
gactorio::ProductId toModelId(ProductKind k) {
    return static_cast<gactorio::ProductId>(k);
}

// MachineStatus(enum) → 화면 표시용 문자열.
const char* stateName(gactorio::MachineStatus s) {
    using S = gactorio::MachineStatus;
    switch (s) {
    case S::Idle:        return "Idle";
    case S::Working:     return "Working";
    case S::Paused:      return "Paused";
    case S::Blocked:     return "Blocked";
    case S::Broken:      return "Broken";
    case S::Maintenance: return "Maintenance";
    }
    return "Unknown";
}

// EventType(enum) → 화면 표시용 문자열.
const char* eventName(gactorio::EventType t) {
    using E = gactorio::EventType;
    switch (t) {
    case E::Info:             return "Info";
    case E::TaskEnqueued:     return "TaskEnqueued";
    case E::TaskStarted:      return "TaskStarted";
    case E::StepCompleted:    return "StepCompleted";
    case E::ProductCompleted: return "ProductCompleted";
    case E::MachineBroken:    return "MachineBroken";
    case E::MachineRepaired:  return "MachineRepaired";
    case E::StateChanged:     return "StateChanged";
    case E::InputsConsumed:   return "InputsConsumed";
    case E::MachinePaused:    return "MachinePaused";
    }
    return "Unknown";
}

// 백엔드 재고 스냅샷은 ID를 숫자 문자열로 준다(원자재 1~5, 제품 101~103).
// 이를 다시 ItemId(정수)로 파싱. 실패 시 0.
ItemId parseInventoryId(const std::string& numeric) {
    try {
        return static_cast<ItemId>(std::stoul(numeric));
    } catch (const std::exception&) {
        return 0;
    }
}

// 해당 ID가 제품 정의에 있으면 "완제품"이다.
bool isProductInventoryId(ItemId id) {
    return gactorio::findProductDefinition(static_cast<gactorio::ProductId>(id)) != nullptr;
}

// ID를 사람이 읽는 이름으로: 제품이면 제품명, 아니면 원자재 이름.
std::string humanizeInventoryId(ItemId id) {
    if (const auto* product = gactorio::findProductDefinition(static_cast<gactorio::ProductId>(id))) {
        return product->name;
    }
    return gactorio::ItemTypeName::get(static_cast<gactorio::ItemType>(id));
}

// 제품의 재료 목록을 "Ingredient x2, Water x1, ..." 형식 문자열로.
std::string formatRequirements(const gactorio::ProductDefinition& definition) {
    std::ostringstream out;
    bool first = true;
    for (const auto& requirement : definition.requirements) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << gactorio::ItemTypeName::get(requirement.itemType())
            << " x" << requirement.quantity();
    }
    return out.str();
}

} // namespace

// =============================================================================
// PImpl — 모든 gactorio:: 상태를 이 구조체 안에 숨긴다(헤더에 노출 안 됨).
// =============================================================================
struct Controller::Impl {
    gactorio::FactoryController backend;   // 실제 백엔드 컨트롤러(Model 소유)
    mutable FactoryView         cached;    // 마지막으로 만든 View 스냅샷(캐시)
    std::vector<ProductOption>  products;  // 제품 카탈로그(불변, 1회 구성)
    mutable bool                dirty = true;  // 캐시 갱신 필요 여부

    // 생성자: 제품 카탈로그를 한 번 만들어 둔다(런타임에 안 바뀜).
    Impl() {
        for (const auto& definition : gactorio::productDefinitions()) {
            products.push_back({
                static_cast<ProductId>(definition.id),
                definition.key,
                definition.name,
                definition.tier,
                definition.totalDurationSeconds,
                formatRequirements(definition)
            });
        }
    }

    // 백엔드 스냅샷을 View DTO(FactoryView)로 변환해 cached 에 채운다.
    // dirty 일 때만 호출되어, 한 틱에 최대 한 번만 변환(성능).
    void rebuild() const {
        const auto snap = backend.snapshot();
        cached = {};
        cached.simulationTime = snap.timeSeconds();

        const auto& s = snap.statistics();
        cached.stats = {
            s.startedTaskEvents(),
            s.completedStepEvents(),
            s.completedProductEvents(),
            s.brokenMachineEvents(),
            s.repairedMachineEvents(),
            s.stateChangedEvents()
        };

        for (const auto& line : snap.productionLines()) {
            LineView lv;
            lv.id                  = static_cast<LineId>(line.id());
            lv.name                = line.name();
            lv.queueLength         = line.queueLength();
            lv.currentTaskName     = line.currentTaskName();
            lv.currentTaskProgress = line.currentTaskProgress();
            // 라인이 "바쁜지" 판단(큐 있음/현재작업 있음/기계가 가동·정비 중) → 삭제 가능 여부.
            bool busy = lv.queueLength > 0 || !lv.currentTaskName.empty();
            for (const auto& m : line.machines()) {
                lv.machines.push_back({
                    static_cast<MachineId>(m.id()),
                    m.name(), m.typeName(),
                    stateName(m.status()), m.progress(), m.health()
                });
                if (m.status() == gactorio::MachineStatus::Working ||
                    m.status() == gactorio::MachineStatus::Maintenance) {
                    busy = true;
                }
            }
            lv.isRemovable = !busy;
            cached.lines.push_back(std::move(lv));
        }

        for (const auto& e : snap.events()) {
            cached.events.push_back({e.timeSeconds(), eventName(e.type()), e.message()});
        }

        for (const auto& entry : snap.inventory().items()) {
            const auto id = parseInventoryId(entry.id());
            cached.inventory.push_back({
                id,
                humanizeInventoryId(id),
                entry.quantity(),
                isProductInventoryId(id)
            });
        }

        dirty = false;
    }
};

// =============================================================================
// 공개 API — 대부분 backend 로 위임하고, 상태를 바꾸면 dirty=true 로 캐시 무효화.
// =============================================================================
Controller::Controller()  : m_impl(std::make_unique<Impl>()) {}
Controller::~Controller() = default;

void Controller::tick(double dt)               { m_impl->backend.tick(dt);               m_impl->dirty = true; }
void Controller::pause()                       { m_impl->backend.pauseSimulation();      m_impl->dirty = true; }
void Controller::resume()                      { m_impl->backend.resumeSimulation();     m_impl->dirty = true; }
void Controller::reset()                       { m_impl->backend.resetSimulation();      m_impl->dirty = true; }
void Controller::setSpeed(double mult)         { m_impl->backend.setSimulationSpeed(mult); }

// enqueue(ProductKind) → ProductId 로 바꿔 enqueueProduct 로 위임.
bool Controller::enqueue(LineId line, ProductKind p) {
    return enqueueProduct(line, static_cast<ProductId>(toModelId(p)));
}

// 명령 메서드들: backend 의 결과(enum/정수)를 bool/LineId 로 변환해 JS 로 반환.
bool Controller::enqueueProduct(LineId line, ProductId p) {
    m_impl->dirty = true;
    return m_impl->backend.enqueueProductById(line, static_cast<gactorio::ProductId>(p))
        == gactorio::FactoryCommandResult::Success;
}
LineId Controller::enqueueAuto(ProductKind p) {
    return enqueueAutoProduct(static_cast<ProductId>(toModelId(p)));
}
LineId Controller::enqueueAutoProduct(ProductId p) {
    m_impl->dirty = true;
    return static_cast<LineId>(m_impl->backend.enqueueAutoById(static_cast<gactorio::ProductId>(p)));
}
LineId Controller::addLine() {
    m_impl->dirty = true;
    return static_cast<LineId>(m_impl->backend.addLine());
}
bool Controller::removeLine(LineId id) {
    m_impl->dirty = true;
    return m_impl->backend.removeLine(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::breakMachine(MachineId id) {
    m_impl->dirty = true;
    return m_impl->backend.forceBreak(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::repair(MachineId id) {            // Repair 버튼 = +5 HP 즉시
    m_impl->dirty = true;
    return m_impl->backend.incrementalRepairMachine(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::restockItem(ItemId id) {          // 원자재 +5
    m_impl->dirty = true;
    return m_impl->backend.restockItem(static_cast<gactorio::ItemType>(id), 5)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::repairAll(MachineId id) {         // Repair All = 정비(지연 후 전량 회복)
    m_impl->dirty = true;
    return m_impl->backend.repairMachine(id)
        == gactorio::FactoryCommandResult::Success;
}

// 조회: dirty 면 한 번 rebuild 한 뒤 캐시 반환. embind 가 이 값을 JS로 값-복사.
const FactoryView& Controller::snapshot() const {
    if (m_impl->dirty) m_impl->rebuild();
    return m_impl->cached;
}

const std::vector<ProductOption>& Controller::products() const {
    return m_impl->products;
}

// ---- Memento 파사드 — backend(Caretaker)로 위임 ----------------------------
void Controller::saveCheckpoint() {
    m_impl->backend.saveCheckpoint();
}

bool Controller::undo() {
    if (!m_impl->backend.undo()) return false;
    m_impl->dirty = true;   // 복원되었으니 캐시 무효화
    return true;
}

bool Controller::canUndo() const {
    return m_impl->backend.canUndo();
}

std::size_t Controller::historySize() const {
    return m_impl->backend.historySize();
}

} // namespace ctrl
