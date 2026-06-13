#pragma once

#include "common/Types.hpp"
#include "model/Product.hpp"

#include <map>
#include <string>
#include <vector>

namespace gactorio {

class Inventory {
public:
    void addItem(const std::string& itemId, int amount);
    void addItem(ItemType itemType, int amount);
    void addProduct(ProductId productId, int quantity);

    bool hasItem(const std::string& itemId, int quantity) const;
    bool consumeItem(const std::string& itemId, int quantity);
    int getItemQuantity(const std::string& itemId) const;
    bool canConsume(const std::map<std::string, int>& inputs) const;
    bool consume(const std::map<std::string, int>& inputs);

    bool hasEnough(const std::vector<ItemRequirement>& requirements) const;
    bool consume(const std::vector<ItemRequirement>& requirements);
    int getQuantity(ItemType itemType) const;

    bool canConsume(const std::map<ItemType, int>& inputs) const;
    bool consume(const std::map<ItemType, int>& inputs);

    const std::map<std::string, int>& items() const;
    const std::map<ProductId, int>& products() const;

    // For Memento restore: overwrite both maps in one shot.
    void replaceContents(const std::map<std::string, int>& newItems,
                         const std::map<ProductId, int>& newProducts);

private:
    std::map<std::string, int> items_;
    std::map<ProductId, int> products_;
};

} // namespace gactorio
