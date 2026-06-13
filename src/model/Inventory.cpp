#include "model/Inventory.hpp"

#include "model/config/ConfigIdAdapters.hpp"

namespace gactorio {

void Inventory::addItem(const std::string& itemId, int amount) {
    if (amount <= 0) {
        return;
    }

    items_[itemId] += amount;
}

void Inventory::addItem(ItemType itemType, int amount) {
    addItem(config_model::toItemId(itemType), amount);
}

void Inventory::addProduct(ProductId productId, int quantity) {
    products_[productId] += quantity;
}

bool Inventory::hasItem(const std::string& itemId, int quantity) const {
    if (quantity < 0) {
        return false;
    }
    return getItemQuantity(itemId) >= quantity;
}

bool Inventory::consumeItem(const std::string& itemId, int quantity) {
    if (!hasItem(itemId, quantity)) {
        return false;
    }
    items_[itemId] -= quantity;
    return true;
}

int Inventory::getItemQuantity(const std::string& itemId) const {
    const auto found = items_.find(itemId);
    if (found == items_.end()) {
        return 0;
    }
    return found->second;
}

bool Inventory::canConsume(const std::map<std::string, int>& inputs) const {
    for (const auto& input : inputs) {
        if (input.second < 0 || getItemQuantity(input.first) < input.second) {
            return false;
        }
    }
    return true;
}

bool Inventory::consume(const std::map<std::string, int>& inputs) {
    if (!canConsume(inputs)) {
        return false;
    }

    for (const auto& input : inputs) {
        items_[input.first] -= input.second;
    }
    return true;
}

bool Inventory::canConsume(const std::map<ItemType, int>& inputs) const {
    std::map<std::string, int> converted;
    for (const auto& input : inputs) {
        converted[config_model::toItemId(input.first)] += input.second;
    }
    return canConsume(converted);
}

bool Inventory::consume(const std::map<ItemType, int>& inputs) {
    std::map<std::string, int> converted;
    for (const auto& input : inputs) {
        converted[config_model::toItemId(input.first)] += input.second;
    }
    return consume(converted);
}

bool Inventory::hasEnough(const std::vector<ItemRequirement>& requirements) const {
    std::map<std::string, int> totals;
    for (const auto& requirement : requirements) {
        if (requirement.quantity() < 0) {
            return false;
        }
        totals[requirement.itemId()] += requirement.quantity();
    }

    return canConsume(totals);
}

bool Inventory::consume(const std::vector<ItemRequirement>& requirements) {
    std::map<std::string, int> totals;
    for (const auto& requirement : requirements) {
        if (requirement.quantity() < 0) {
            return false;
        }
        totals[requirement.itemId()] += requirement.quantity();
    }

    return consume(totals);
}

int Inventory::getQuantity(ItemType itemType) const {
    return getItemQuantity(config_model::toItemId(itemType));
}

const std::map<std::string, int>& Inventory::items() const {
    return items_;
}

const std::map<ProductId, int>& Inventory::products() const {
    return products_;
}

void Inventory::replaceContents(const std::map<std::string, int>& newItems,
                                const std::map<ProductId, int>& newProducts) {
    items_    = newItems;
    products_ = newProducts;
}

} // namespace gactorio
