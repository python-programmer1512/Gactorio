#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/ProductCatalog.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <utility>

// =============================================================================
// CarbonationFactory.cpp — 음료 공장 특화 구현
// base Factory 의 가상 훅(createProductById/createLineForMemento)을 채우고,
// 기본 구성(재고/라인/시작 제품/레시피)과 동적 라인 생성을 담당한다.
// =============================================================================

namespace gactorio {

namespace {

constexpr int kInitialRawItemQuantity = 5;   // 시작 시 원자재 각 5개

// 라인 ID(1,2,3...)를 "Beverage Line A/B/C..." 이름으로 변환.
std::string beverageLineName(LineId id) {
    const std::string letter(1, static_cast<char>('A' + static_cast<int>(id) - 1));
    return std::string("Beverage Line ") + letter;
}

// 4스테이션(혼합→품질→충전→포장) 라인을 만든다. 기계 ID 4개를 받아 순서대로 배치.
// 이 함수가 "라인 토폴로지"의 단일 정의처 — 생성과 Memento 복원이 모두 이걸 쓴다.
ProductionLine makeBeverageLine(LineId id, const std::array<MachineId, 4>& machineIds) {
    ProductionLine line(id, beverageLineName(id));
    line.addMachine(std::make_unique<MixingStation>   (machineIds[0], "Mixer"));
    line.addMachine(std::make_unique<QualityStation>  (machineIds[1], "Quality Check"));
    line.addMachine(std::make_unique<BottlingStation> (machineIds[2], "Filler"));
    line.addMachine(std::make_unique<PackagingStation>(machineIds[3], "Packager"));
    return line;
}

// 현재 라인들로부터 "다음에 발급할 라인 ID" 추정(복원 시 ID 미저장 대비 폴백).
LineId nextLineIdAfter(const std::vector<ProductionLine>& lines) {
    LineId next = 1;
    for (const auto& line : lines) {
        next = std::max(next, line.id() + 1);
    }
    return next;
}

// 현재 기계들로부터 "다음에 발급할 기계 ID" 추정(폴백).
MachineId nextMachineIdAfter(const std::vector<Machine*>& machines) {
    MachineId next = 1;
    for (const auto* machine : machines) {
        if (machine != nullptr) {
            next = std::max(next, machine->id() + 1);
        }
    }
    return next;
}

} // namespace

// 기본 구성: 카탈로그로부터 레시피 생성 → 원자재 재고 채우기 → 4스테이션 라인 1개 +
// 시작용 Voltz Classic 1개 enqueue. GUI 가 켜지자마자 무언가 돌아가도록.
CarbonationFactory::CarbonationFactory() {
    // ---- 레시피 (런타임 생산엔 미사용, 조회/표시용으로 보관) ----------------
    for (const auto& definition : productDefinitions()) {
        Recipe recipe(definition.id, definition.name + " Brew", definition.totalDurationSeconds);
        for (const auto& requirement : definition.requirements) {
            recipe.addInput(requirement.itemType(), requirement.quantity());
        }
        recipe.addOutput(definition.id, 1);
        recipes_.push_back(std::move(recipe));
    }

    // ---- 초기 재고 -------------------------------------------------------
    inventory().addItem(ItemType::Ingredient,  kInitialRawItemQuantity);
    inventory().addItem(ItemType::Water,       kInitialRawItemQuantity);
    inventory().addItem(ItemType::EmptyBottle, kInitialRawItemQuantity);
    inventory().addItem(ItemType::Label,       kInitialRawItemQuantity);
    inventory().addItem(ItemType::Package,     kInitialRawItemQuantity);

    // ---- 기본 라인: 4스테이션 (라인 1, 기계 1~4) -------------------------
    ProductionLine line = makeBeverageLine(1, {1, 2, 3, 4});

    // 시작이 보이도록 Voltz Classic 한 개를 미리 큐에 넣음.
    line.enqueueProduct(createProduct(ProductType::VoltzClassic));

    addProductionLine(std::move(line));
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

// 동적 라인 추가: 다음 라인 ID와 기계 ID 4개를 발급해 새 4스테이션 라인 생성.
LineId CarbonationFactory::addDynamicLine() {
    const LineId    id      = nextLineId_++;
    const MachineId baseMid = nextMachineId_;
    nextMachineId_ += 4;

    ProductionLine line = makeBeverageLine(id, {baseMid + 0, baseMid + 1, baseMid + 2, baseMid + 3});
    addProductionLine(std::move(line));
    return id;
}

// Memento 캡처에 "다음 ID 상태"를 덧붙인다(동적 라인/기계 ID 연속성 보존).
FactoryMemento CarbonationFactory::createMemento() const {
    auto memento = Factory::createMemento();
    memento.setNextIds(nextLineId_, nextMachineId_);
    return memento;
}

// 복원: base 복원 후 다음 ID 상태를 되돌린다(저장돼 있으면 그 값, 아니면 폴백 추정).
void CarbonationFactory::restoreFromMemento(const FactoryMemento& memento) {
    Factory::restoreFromMemento(memento);
    nextLineId_ = memento.nextLineId() != 0
        ? memento.nextLineId()
        : nextLineIdAfter(productionLines());
    nextMachineId_ = memento.nextMachineId() != 0
        ? memento.nextMachineId()
        : nextMachineIdAfter(machines());
}

// base 훅 구현: ID로 카탈로그 Product 생성.
std::shared_ptr<Product> CarbonationFactory::createProductById(ProductId id) const {
    return createProduct(id);
}

// base 훅 구현: 저장된 기계 ID들로 4스테이션 라인 재구성. 기계가 4개 미만이면 복원 불가.
std::optional<ProductionLine> CarbonationFactory::createLineForMemento(const LineMemento& memento) const {
    const auto& machineMementos = memento.machines();
    if (machineMementos.size() < 4) {
        return std::nullopt;
    }

    return makeBeverageLine(
        memento.id(),
        {
            machineMementos[0].id(),
            machineMementos[1].id(),
            machineMementos[2].id(),
            machineMementos[3].id(),
        });
}

} // namespace gactorio
