#include "model/Recipe.hpp"

#include <utility>

namespace gactorio {

Recipe::Recipe(RecipeId id, std::string name, double durationSeconds)
    : id_(id), name_(std::move(name)), durationSeconds_(durationSeconds) {}

RecipeId Recipe::id() const {
    return id_;
}

const std::string& Recipe::name() const {
    return name_;
}

double Recipe::durationSeconds() const {
    return durationSeconds_;
}

void Recipe::addInput(ItemType itemType, int quantity) {
    inputs_[itemType] += quantity;
}

void Recipe::addOutput(ProductId productId, int quantity) {
    outputs_[productId] += quantity;
}

const std::map<ItemType, int>& Recipe::inputs() const {
    return inputs_;
}

const std::map<ProductId, int>& Recipe::outputs() const {
    return outputs_;
}

} // namespace gactorio
