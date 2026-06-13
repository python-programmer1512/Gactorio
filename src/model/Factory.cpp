#include "model/Factory.hpp"

#include <algorithm>
#include <utility>

// =============================================================================
// Factory.cpp — 공장 집합체의 동작 구현
// 가장 중요한 두 부분: (1) update() 시뮬레이션 루프(다형 호출, 타입 분기 없음),
//                      (2) Memento 의 create/restore(Originator 구현).
// =============================================================================

namespace gactorio {

// 생성자: 두 Observer(이벤트 로그/통계)를 이벤트 버스에 구독시킨다(Observer 패턴 배선).
Factory::Factory() {
    eventBus_.subscribe(&eventLog_);
    eventBus_.subscribe(&statistics_);
}

SimulationTime Factory::simulationTime() const {
    return clock_.now();
}

const Inventory& Factory::inventory() const {
    return inventory_;
}

Inventory& Factory::inventory() {
    return inventory_;
}

const std::vector<ProductionLine>& Factory::productionLines() const {
    return productionLines_;
}

const std::vector<Machine*>& Factory::machines() const {
    return machines_;
}

const EventLog& Factory::eventLog() const {
    return eventLog_;
}

const Statistics& Factory::statistics() const {
    return statistics_;
}

EventBus& Factory::eventBus() {
    return eventBus_;
}

const EventBus& Factory::eventBus() const {
    return eventBus_;
}

const SimClock& Factory::clock() const {
    return clock_;
}

// 라인 추가: 라인(및 그 기계들)에 이벤트 버스를 연결하고, 기계 포인터를 캐시에 등록한 뒤
// 라인을 소유 벡터로 이동(move)한다.
void Factory::addProductionLine(ProductionLine line) {
    line.setEventBus(&eventBus_);
    for (const auto& machine : line.machines()) {
        machines_.push_back(machine.get());
    }
    productionLines_.push_back(std::move(line));
}

// machines_ 캐시 재구성: 라인 벡터가 재배치/변경된 뒤 포인터 캐시를 다시 만든다.
// (vector<ProductionLine> 가 재할당되면 내부 기계 주소가 바뀔 수 있으므로 필요.)
void Factory::rebuildMachineCache() {
    machines_.clear();
    for (auto& line : productionLines_) {
        line.setEventBus(&eventBus_);
        for (const auto& machine : line.machines()) {
            machines_.push_back(machine.get());
        }
    }
}

// 라인 제거: 최소 1개 라인은 남겨야 하고, 큐/진행 작업/가동·정비 중 기계가 없어야 한다.
bool Factory::removeProductionLine(LineId id) {
    if (productionLines_.size() <= 1) {
        return false;   // 마지막 한 개는 보존
    }

    auto it = std::find_if(productionLines_.begin(), productionLines_.end(),
        [id](const ProductionLine& l) { return l.id() == id; });
    if (it == productionLines_.end()) return false;

    // 라인에 일감이 남아 있으면 제거 불가(데이터 일관성 보호).
    if (it->queueLength() > 0) return false;
    for (const auto& m : it->machines()) {
        if (m->hasTask()) return false;
        if (m->getStatus() == MachineStatus::Working ||
            m->getStatus() == MachineStatus::Maintenance) return false;
    }

    // 이 라인 소속 기계 포인터를 캐시에서 제거.
    for (const auto& m : it->machines()) {
        auto mit = std::find(machines_.begin(), machines_.end(), m.get());
        if (mit != machines_.end()) machines_.erase(mit);
    }
    productionLines_.erase(it);
    return true;
}

// 제품 enqueue: 라인 찾기 → 재료 소비(원자적) → 작업 큐에 등록. 재료 부족이면 실패.
bool Factory::enqueueProduct(LineId lineId, std::shared_ptr<Product> product) {
    if (product == nullptr) {
        return false;
    }

    auto* line = findProductionLine(lineId);
    if (line == nullptr) {
        return false;
    }

    if (!inventory_.consume(product->getRequirements())) {   // 재료 부족 시 enqueue 안 함
        return false;
    }

    line->enqueueProduct(std::move(product));
    return true;
}

// 원자재 보충: 알려진 원자재 종류만 허용(Unknown/제품은 거부).
bool Factory::restockItem(ItemType itemType, int amount) {
    if (amount <= 0) {
        return false;
    }

    switch (itemType) {
    case ItemType::Ingredient:
    case ItemType::Water:
    case ItemType::EmptyBottle:
    case ItemType::Label:
    case ItemType::Package:
        break;
    case ItemType::Unknown:
    default:
        return false;
    }

    inventory_.addItem(itemType, amount);
    return true;
}

ProductionLine* Factory::findProductionLine(LineId id) {
    for (auto& line : productionLines_) {
        if (line.id() == id) {
            return &line;
        }
    }
    return nullptr;
}

const ProductionLine* Factory::findProductionLine(LineId id) const {
    for (const auto& line : productionLines_) {
        if (line.id() == id) {
            return &line;
        }
    }
    return nullptr;
}

Machine* Factory::findMachine(MachineId id) {
    for (auto& line : productionLines_) {
        auto* machine = line.findMachine(id);
        if (machine != nullptr) {
            return machine;
        }
    }
    return nullptr;
}

// ★ 매 틱 시뮬레이션 진행. 순서가 중요하다:
//   1) 시계 갱신 → 배속 적용된 delta 획득(일시정지면 0).
//   2) (선)작업배정: 유휴 기계에 큐의 적합 작업을 배정.
//   3) machines_ 캐시를 돌며 Machine::update(delta) 다형 호출 ── 여기에 타입 분기 전무.
//   4) 완료 제품 수거해 재고에 반영.
//   5) (후)작업배정: 한 단계 끝나 비워진 기계에 다음 단계 작업을 바로 배정.
SimulationTime Factory::update(double realDeltaTime) {
    const auto deltaTime = clock_.update(realDeltaTime);

    for (auto& line : productionLines_) {
        line.assignAvailableTask();
    }

    for (auto* machine : machines_) {
        machine->update(deltaTime);   // ★ 다형 호출(구체 타입 모름)
    }

    for (auto& line : productionLines_) {
        for (const auto productId : line.collectCompletedProducts()) {
            inventory_.addProduct(productId, 1);
        }
    }

    for (auto& line : productionLines_) {
        line.assignAvailableTask();
    }

    return deltaTime;
}

void Factory::pauseClock() {
    clock_.pause();
}

void Factory::resumeClock() {
    clock_.resume();
}

void Factory::resetClock() {
    clock_.reset();
}

void Factory::stopClock() {
    clock_.stop();
}

void Factory::setClockSpeed(double speedMultiplier) {
    clock_.setSpeed(speedMultiplier);
}

EventLog& Factory::mutableEventLog() {
    return eventLog_;
}

Statistics& Factory::mutableStatistics() {
    return statistics_;
}

// base Factory 는 카탈로그가 없어 ID로 Product 를 못 만든다. 파생(CarbonationFactory)이 override.
std::shared_ptr<Product> Factory::createProductById(ProductId) const {
    return nullptr;
}

// base Factory 는 라인의 구체 스테이션 구성을 모른다. 파생이 override 로 재구성 제공.
std::optional<ProductionLine> Factory::createLineForMemento(const LineMemento&) const {
    return std::nullopt;
}

// =============================================================================
// Memento — Originator 구현
// =============================================================================
// createMemento(): 현재 시간/재고/라인별(대기 제품 ID + 기계 HP/상태)을 스냅샷에 담는다.
FactoryMemento Factory::createMemento() const {
    FactoryMemento m(clock_.now(), inventory_.items(), inventory_.products());

    for (const auto& line : productionLines_) {
        std::vector<MachineMemento> machineMementos;
        for (const auto& machine : line.machines()) {
            machineMementos.emplace_back(
                machine->id(), machine->getHealth(), machine->getStatus()
            );
        }
        m.addLine(LineMemento(line.id(), line.pendingProductIds(), std::move(machineMementos)));
    }
    return m;
}

// restoreFromMemento(): 스냅샷으로 공장 상태를 되돌린다.
void Factory::restoreFromMemento(const FactoryMemento& m) {
    // 시계 — 저장된 시뮬레이션 시간으로 즉시 점프.
    clock_.setNow(m.simulationTime());

    // 재고 — 원자재/완제품 맵을 통째로 덮어쓰기.
    inventory_.replaceContents(m.items(), m.products());

    // 토폴로지 정렬: 체크포인트 이후 생긴 라인은 제거하고, 빠진 라인은 파생에 재생성 요청.
    const auto& savedLines = m.lines();
    productionLines_.erase(
        std::remove_if(
            productionLines_.begin(),
            productionLines_.end(),
            [&savedLines](const ProductionLine& line) {
                return std::none_of(
                    savedLines.begin(),
                    savedLines.end(),
                    [&line](const LineMemento& lm) {
                        return lm.id() == line.id();
                    });
            }),
        productionLines_.end());
    rebuildMachineCache();

    for (const auto& lm : savedLines) {
        if (findProductionLine(lm.id()) == nullptr) {
            auto restoredLine = createLineForMemento(lm);   // 파생 훅으로 라인 재구성
            if (restoredLine.has_value()) {
                addProductionLine(std::move(*restoredLine));
            }
        }
    }

    // 라인별: 기계 리셋(작업 폐기, HP/상태 복원) → 큐 비우고 → 저장된 대기 제품 재등록.
    for (const auto& lm : savedLines) {
        auto* line = findProductionLine(lm.id());
        if (line == nullptr) continue;

        line->clearQueue();
        line->clearCompleted();

        for (const auto& machineSnap : lm.machines()) {
            auto* machine = line->findMachine(machineSnap.id());
            if (machine != nullptr) {
                machine->resetForRestore(machineSnap.health(), machineSnap.status());
            }
        }

        for (const auto productId : lm.queueProductIds()) {
            auto product = createProductById(productId);   // 파생 훅으로 Product 재생성
            if (product != nullptr) {
                line->enqueueProduct(std::move(product));
            }
        }
    }
}

} // namespace gactorio
