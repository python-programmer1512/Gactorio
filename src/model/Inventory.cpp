#include "model/Inventory.hpp"

// =============================================================================
// Inventory.cpp — 재고 연산 구현. consume 류는 "전부 있을 때만 전부 차감"(원자성).
// =============================================================================

namespace gactorio {

// 원자재 추가(양수만). 없던 키는 0에서 시작해 누적.
void Inventory::addItem(ItemType itemType, int amount) {
    if (amount <= 0) {
        return;
    }

    items_[itemType] += amount;
}

// 완제품 추가.
void Inventory::addProduct(ProductId productId, int quantity) {
    products_[productId] += quantity;
}

// 맵 기준 소비 가능 여부: 모든 항목이 음수가 아니고, 보유량이 요구량 이상이어야 true.
bool Inventory::canConsume(const std::map<ItemType, int>& inputs) const {
    for (const auto& input : inputs) {
        if (input.second < 0 || getQuantity(input.first) < input.second) {
            return false;
        }
    }
    return true;
}

// 맵 기준 소비: 먼저 전부 가능한지 확인한 뒤에만 차감(부분 소비 없음 = 원자적).
bool Inventory::consume(const std::map<ItemType, int>& inputs) {
    if (!canConsume(inputs)) {
        return false;
    }

    for (const auto& input : inputs) {
        items_[input.first] -= input.second;
    }
    return true;
}

// ItemRequirement 목록을 (ItemType→총수량) 맵으로 합산한 뒤 canConsume 위임.
bool Inventory::hasEnough(const std::vector<ItemRequirement>& requirements) const {
    std::map<ItemType, int> totals;
    for (const auto& requirement : requirements) {
        if (requirement.quantity() < 0) {
            return false;
        }
        totals[requirement.itemType()] += requirement.quantity();
    }

    return canConsume(totals);
}

// ItemRequirement 목록 기준 소비(합산 후 consume 위임).
bool Inventory::consume(const std::vector<ItemRequirement>& requirements) {
    std::map<ItemType, int> totals;
    for (const auto& requirement : requirements) {
        if (requirement.quantity() < 0) {
            return false;
        }
        totals[requirement.itemType()] += requirement.quantity();
    }

    return consume(totals);
}

// 특정 원자재 보유 수량(없으면 0).
int Inventory::getQuantity(ItemType itemType) const {
    const auto found = items_.find(itemType);
    if (found == items_.end()) {
        return 0;
    }
    return found->second;
}

const std::map<ItemType, int>& Inventory::items() const {
    return items_;
}

const std::map<ProductId, int>& Inventory::products() const {
    return products_;
}

// Memento 복원용: 두 맵을 통째로 교체.
void Inventory::replaceContents(const std::map<ItemType, int>& newItems,
                                const std::map<ProductId, int>& newProducts) {
    items_    = newItems;
    products_ = newProducts;
}

} // namespace gactorio
