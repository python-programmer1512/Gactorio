#pragma once

#include "common/Types.hpp"

#include <map>
#include <string>

namespace gactorio {

class Recipe {
public:
    Recipe(RecipeId id, std::string name, double durationSeconds);

    RecipeId id() const;
    const std::string& name() const;
    double durationSeconds() const;

    void addInput(const std::string& itemId, int quantity);
    void addInput(ItemType itemType, int quantity);
    void addOutput(ProductId productId, int quantity);

    bool requiresItem(const std::string& itemId) const;
    bool requiresItem(ItemType itemType) const;
    int requiredQuantity(const std::string& itemId) const;
    int requiredQuantity(ItemType itemType) const;

    const std::map<std::string, int>& inputs() const;
    const std::map<ProductId, int>& outputs() const;

private:
    RecipeId id_;
    std::string name_;
    double durationSeconds_;
    std::map<std::string, int> inputs_;
    std::map<ProductId, int> outputs_;
};

} // namespace gactorio
