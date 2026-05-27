#include "model/Inventory.hpp"

namespace gactorio {

void Inventory::addItem(ItemType itemType, int amount) {
    if (amount <= 0) {
        return;
    }

    items_[itemType] += amount;
}

void Inventory::addProduct(ProductId productId, int quantity) {
    if (quantity <= 0) {
        return;
    }

    products_[productId] += quantity;
}

bool Inventory::canConsume(const std::map<ItemType, int>& inputs) const {
    for (const auto& input : inputs) {
        if (input.second < 0 || getQuantity(input.first) < input.second) {
            return false;
        }
    }
    return true;
}

bool Inventory::consume(const std::map<ItemType, int>& inputs) {
    if (!canConsume(inputs)) {
        return false;
    }

    for (const auto& input : inputs) {
        items_[input.first] -= input.second;
    }
    return true;
}

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

InventoryMemento Inventory::exportState() const {
    InventoryMemento state;
    state.items = items_;
    state.products = products_;
    return state;
}

void Inventory::restoreState(const InventoryMemento& state) {
    items_ = state.items;
    products_ = state.products;
}

} // namespace gactorio
