#pragma once

// =============================================================================
// Inventory — 원자재/완제품 재고 보관소
// -----------------------------------------------------------------------------
// 두 개의 맵을 캡슐화한다:
//   items_    : 원자재 재고 (ItemType  → 수량)
//   products_ : 완제품 재고 (ProductId → 수량)
//
// 캡슐화(과제 핵심 요구): 외부는 맵을 직접 건드리지 못하고 add/consume/canConsume
// 같은 메서드로만 재고를 변경한다. 이로써 "수량이 음수가 되거나 원자에 깨지는" 상태를
// 방지한다. consume()은 "전부 있을 때만 전부 소비"하는 원자적(all-or-nothing) 연산.
// =============================================================================

#include "common/Types.hpp"
#include "model/Product.hpp"

#include <map>
#include <vector>

namespace gactorio {

class Inventory {
public:
    void addItem(ItemType itemType, int amount);          // 원자재 추가(양수만)
    void addProduct(ProductId productId, int quantity);    // 완제품 추가

    // ItemRequirement 목록 기준 소비 가능 여부 / 소비(원자적).
    bool hasEnough(const std::vector<ItemRequirement>& requirements) const;
    bool consume(const std::vector<ItemRequirement>& requirements);
    int getQuantity(ItemType itemType) const;              // 특정 원자재 보유 수량

    // (ItemType→수량) 맵 기준 소비 가능 여부 / 소비(원자적).
    bool canConsume(const std::map<ItemType, int>& inputs) const;
    bool consume(const std::map<ItemType, int>& inputs);

    const std::map<ItemType, int>& items() const;          // 원자재 재고 읽기
    const std::map<ProductId, int>& products() const;      // 완제품 재고 읽기

    // Memento 복원용: 두 맵을 한 번에 통째로 덮어쓴다.
    void replaceContents(const std::map<ItemType, int>& newItems,
                         const std::map<ProductId, int>& newProducts);

private:
    std::map<ItemType, int> items_;      // 원자재 재고(캡슐화)
    std::map<ProductId, int> products_;  // 완제품 재고(캡슐화)
};

} // namespace gactorio
