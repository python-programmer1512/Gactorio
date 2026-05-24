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

    void addInput(ItemType itemType, int quantity);
    void addOutput(ProductId productId, int quantity);

    const std::map<ItemType, int>& inputs() const;
    const std::map<ProductId, int>& outputs() const;

private:
    RecipeId id_;
    std::string name_;
    double durationSeconds_;
    std::map<ItemType, int> inputs_;
    std::map<ProductId, int> outputs_;
};

} // namespace gactorio
