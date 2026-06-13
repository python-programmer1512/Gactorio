#include "controller/FactoryController.hpp"

#include "model/Machine.hpp"
#include "model/ProductCatalog.hpp"

#include <algorithm>
#include <memory>
#include <string>

// =============================================================================
// FactoryController.cpp — 백엔드 유스케이스 컨트롤러 구현
// (1) 명령: 인자 검증 후 Factory/Machine 으로 위임, 결과를 FactoryCommandResult 로.
// (2) 조회: Model 상태를 읽기 전용 DTO 스냅샷으로 "복사"해 반환(가변 객체 노출 금지).
// (3) Memento 파사드: createMemento→push / pop→restore.
// =============================================================================

namespace gactorio {

namespace {

// ---- Model → DTO 스냅샷 변환 헬퍼들 (값 복사) -------------------------------

// 재고를 스냅샷으로: 원자재/완제품을 모두 (숫자 문자열 ID, 수량)으로 담는다.
InventorySnapshot makeInventorySnapshot(const Inventory& inventory) {
    InventorySnapshot snapshot;
    for (const auto& item : inventory.items()) {
        snapshot.addItem(std::to_string(static_cast<std::uint64_t>(item.first)), item.second);
    }
    for (const auto& product : inventory.products()) {
        snapshot.addItem(std::to_string(product.first), product.second);
    }
    return snapshot;
}

StatisticsSnapshot makeStatisticsSnapshot(const Statistics& statistics) {
    return StatisticsSnapshot(
        statistics.completedProductEvents(),
        statistics.startedTaskEvents(),
        statistics.completedStepEvents(),
        statistics.brokenMachineEvents(),
        statistics.repairedMachineEvents(),
        statistics.stateChangedEvents());
}

MachineSnapshot makeMachineSnapshot(const Machine& machine) {
    return MachineSnapshot(
        machine.getId(),
        machine.getName(),
        machine.typeName(),
        machine.getStatus(),
        machine.stateName(),
        machine.getProgress(),
        machine.getHealth());
}

// 라인 스냅샷: 현재 작업명/진행률 + 소속 기계 스냅샷들. 진행률은 라인 작업과 기계
// 진행 중 더 큰 값을 채택해 UI 막대가 자연스럽게 차오르게 한다.
ProductionLineSnapshot makeProductionLineSnapshot(const ProductionLine& line) {
    std::string currentName;
    double currentProgress = 0.0;
    if (const auto task = line.currentTask()) {
        currentName = task->getProductName();
        currentProgress = task->getProgressInRoute();
    }

    ProductionLineSnapshot snapshot(line.id(), line.name(), line.queueLength(), currentName, currentProgress);
    for (const auto& machine : line.machines()) {
        if (machine->hasTask()) {
            currentProgress = std::max(currentProgress, machine->getProgress());
        }
        snapshot.addMachine(makeMachineSnapshot(*machine));
    }
    snapshot.setCurrentTaskProgress(currentProgress);
    return snapshot;
}

} // namespace

// 생성 시 기본 음료 공장을 구성(GUI가 켜지면 바로 시뮬레이션 가능).
FactoryController::FactoryController() {
    createDefaultCarbonationFactory();
}

void FactoryController::createDefaultCarbonationFactory() {
    factory_ = std::make_unique<CarbonationFactory>();
    history_.clear();
}

void FactoryController::reset() {
    createDefaultCarbonationFactory();
}

void FactoryController::tick(double deltaTime) {
    if (factory_) {
        factory_->update(deltaTime);
    }
}

void FactoryController::startSimulation() {
    resumeSimulation();
}

void FactoryController::pauseSimulation() {
    if (factory_) {
        factory_->pauseClock();
    }
}

void FactoryController::resetSimulation() {
    reset();
}

void FactoryController::resumeSimulation() {
    if (factory_) {
        factory_->resumeClock();
    }
}

void FactoryController::resetSimulationClock() {
    if (factory_) {
        factory_->resetClock();
    }
}

void FactoryController::stopSimulation() {
    if (factory_) {
        factory_->stopClock();
    }
}

void FactoryController::setSpeed(double speedMultiplier) {
    setSimulationSpeed(speedMultiplier);
}

void FactoryController::setSimulationSpeed(double speedMultiplier) {
    if (factory_) {
        factory_->setClockSpeed(speedMultiplier);
    }
}

FactoryCommandResult FactoryController::enqueueProduct(LineId lineId, ProductType productType) {
    return enqueueProductById(lineId, static_cast<ProductId>(productType));
}

// 특정 라인에 제품 enqueue: 라인 존재 확인 → 카탈로그로 Product 생성 → 재료 소비+큐 등록.
// 각 실패 지점을 NotFound/InvalidRequest 로 구분해 반환(전형적 명령 처리 패턴).
FactoryCommandResult FactoryController::enqueueProductById(LineId lineId, ProductId productId) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* line = factory_->findProductionLine(lineId);
    if (line == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    auto product = createProduct(productId);
    if (product == nullptr) {
        return FactoryCommandResult::InvalidRequest;
    }
    return factory_->enqueueProduct(line->id(), std::move(product))
        ? FactoryCommandResult::Success    // 재료 충분 → 성공
        : FactoryCommandResult::InvalidRequest;  // 재료 부족 등
}

LineId FactoryController::enqueueAuto(ProductType productType) {
    return enqueueAutoById(static_cast<ProductId>(productType));
}

// 자동 enqueue: 큐가 가장 짧은 라인을 골라 그 라인에 enqueue. 부하 분산용.
LineId FactoryController::enqueueAutoById(ProductId productId) {
    if (!factory_) return 0;
    const auto& lines = factory_->productionLines();
    if (lines.empty()) return 0;

    // 큐가 가장 짧은 라인을 선택. 동률이면 먼저 나온 라인이 이김.
    const ProductionLine* best = nullptr;
    std::size_t bestQ = 0;
    for (const auto& l : lines) {
        if (best == nullptr || l.queueLength() < bestQ) {
            best  = &l;
            bestQ = l.queueLength();
        }
    }
    if (best == nullptr) return 0;
    if (enqueueProductById(best->id(), productId) == FactoryCommandResult::Success) {
        return best->id();
    }
    return 0;
}

LineId FactoryController::addLine() {
    if (!factory_) return 0;
    return factory_->addDynamicLine();
}

FactoryCommandResult FactoryController::removeLine(LineId id) {
    if (!factory_) return FactoryCommandResult::InvalidRequest;
    return factory_->removeProductionLine(id)
        ? FactoryCommandResult::Success
        : FactoryCommandResult::InvalidRequest;
}

// 기계 대상 명령들의 공통 패턴: 공장 확인 → 기계 찾기(없으면 NotFound) → 기계에 위임.
// (forceBreak/repairMachine/incrementalRepairMachine/pause/resumeMachine 모두 동일 형태.)
FactoryCommandResult FactoryController::forceBreak(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->forceBreak();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::repairMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->repair();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::incrementalRepairMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->incrementalRepair();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::restockItem(ItemType itemType, int amount) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    return factory_->restockItem(itemType, amount)
        ? FactoryCommandResult::Success
        : FactoryCommandResult::InvalidRequest;
}

FactoryCommandResult FactoryController::pauseMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->pause();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::resumeMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->resume();
    return FactoryCommandResult::Success;
}

FactorySnapshot FactoryController::getFactorySnapshot() const {
    return snapshot();
}

std::vector<EventSnapshot> FactoryController::getEventLogs() const {
    std::vector<EventSnapshot> logs;
    if (!factory_) {
        return logs;
    }

    for (const auto& event : factory_->eventLog().events()) {
        logs.emplace_back(event.simulationTime(), event.type(), event.message());
    }
    return logs;
}

StatisticsSnapshot FactoryController::getStatistics() const {
    if (!factory_) {
        return StatisticsSnapshot(0);
    }
    return makeStatisticsSnapshot(factory_->statistics());
}

// 전체 공장 스냅샷: 시간/재고/통계 + 라인별 스냅샷 + 이벤트 로그를 값 복사로 구성.
// 공장이 없으면 빈 스냅샷. (가변 Model 객체는 절대 반환하지 않음 = 캡슐화 경계.)
FactorySnapshot FactoryController::snapshot() const {
    if (!factory_) {
        return FactorySnapshot(0.0, InventorySnapshot(), StatisticsSnapshot(0));
    }

    FactorySnapshot snapshot(
        factory_->simulationTime(),
        makeInventorySnapshot(factory_->inventory()),
        makeStatisticsSnapshot(factory_->statistics()));

    for (const auto& line : factory_->productionLines()) {
        auto lineSnapshot = makeProductionLineSnapshot(line);
        snapshot.addProductionLine(std::move(lineSnapshot));
    }

    for (const auto& event : factory_->eventLog().events()) {
        snapshot.addEvent(EventSnapshot(event.simulationTime(), event.type(), event.message()));
    }

    return snapshot;
}

// =============================================================================
// Memento 파사드 — Originator(factory_)와 Caretaker(history_)를 잇는 얇은 층.
// =============================================================================
// 체크포인트 저장: 공장에게 메멘토를 만들게 해 히스토리 스택에 push.
void FactoryController::saveCheckpoint() {
    if (!factory_) return;
    history_.push(factory_->createMemento());
}

// 되돌리기: 히스토리에서 최근 메멘토를 pop 해 공장에 복원 요청.
bool FactoryController::undo() {
    if (!factory_) return false;
    auto m = history_.pop();
    if (!m.has_value()) return false;
    factory_->restoreFromMemento(*m);
    return true;
}

bool FactoryController::canUndo() const {
    return history_.canUndo();
}

std::size_t FactoryController::historySize() const {
    return history_.size();
}

} // namespace gactorio
