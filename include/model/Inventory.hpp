#pragma once

#include "common/Types.hpp"
#include "model/memento/FactoryMemento.hpp"
#include "model/Product.hpp"

#include <map>
#include <vector>

namespace gactorio {

class Inventory {
public:
    void addItem(ItemType itemType, int amount);
    void addProduct(ProductId productId, int quantity);

    bool hasEnough(const std::vector<ItemRequirement>& requirements) const;
    bool consume(const std::vector<ItemRequirement>& requirements);
    int getQuantity(ItemType itemType) const;

    bool canConsume(const std::map<ItemType, int>& inputs) const;
    bool consume(const std::map<ItemType, int>& inputs);

    const std::map<ItemType, int>& items() const;
    const std::map<ProductId, int>& products() const;

    InventoryMemento exportState() const;
    void restoreState(const InventoryMemento& state);

private:
    std::map<ItemType, int> items_;
    std::map<ProductId, int> products_;
};

} // namespace gactorio
