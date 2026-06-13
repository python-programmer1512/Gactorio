#pragma once

// =============================================================================
// CarbonationFactory — 에너지 드링크 테마에 특화된 구체 Factory
// -----------------------------------------------------------------------------
// is-a: CarbonationFactory : Factory (Factory 레벨에서의 상속 데모)
//
// 역할:
//   * 생성자에서 기본 구성(원자재 재고, 4스테이션 라인 1개, 시작 Voltz Classic 1개,
//     카탈로그 기반 레시피 목록)을 세팅한다.
//   * addDynamicLine()으로 새 4스테이션 라인을 동적으로 추가한다(ID 자동 발급).
//   * Factory 의 가상 훅(createProductById/createLineForMemento)을 ProductCatalog 와
//     스테이션 구성으로 구현해, base Factory 의 복원 로직이 구체 타입을 몰라도 되게 한다.
//
// base 의 시뮬레이션 루프는 전혀 건드리지 않는다 → 상속으로 "특화"만 더한 형태.
// =============================================================================

#include "model/Factory.hpp"
#include "model/Recipe.hpp"

#include <vector>

namespace gactorio {

class CarbonationFactory final : public Factory {
public:
    CarbonationFactory();

    const std::vector<Recipe>& recipes() const;   // 보관 중인 레시피 목록(조회용)

    // 새 4스테이션 음료 라인을 만들어 공장에 추가하고, 그 LineId 를 반환.
    LineId addDynamicLine();

    // Memento 오버라이드: 다음 ID 상태를 스냅샷에 함께 저장/복원.
    FactoryMemento createMemento() const override;
    void restoreFromMemento(const FactoryMemento& memento) override;

protected:
    // Memento 복원 훅 구현 — ProductId 를 카탈로그로 되돌려 Product 생성.
    std::shared_ptr<Product> createProductById(ProductId id) const override;
    // Memento 복원 훅 구현 — 저장된 기계 ID로 4스테이션 라인을 재구성.
    std::optional<ProductionLine> createLineForMemento(const LineMemento& memento) const override;

private:
    std::vector<Recipe> recipes_;          // 카탈로그에서 생성한 레시피들
    LineId    nextLineId_    = 2;          // 다음 동적 라인 ID(1번은 기본 라인)
    MachineId nextMachineId_ = 5;          // 다음 기계 ID(1~4는 기본 라인 기계)
};

} // namespace gactorio
