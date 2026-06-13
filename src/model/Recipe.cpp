#include "model/Recipe.hpp"

#include "model/config/ConfigIdAdapters.hpp"

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

void Recipe::addInput(const std::string& itemId, int quantity) {
    inputs_[itemId] += quantity;
}

void Recipe::addInput(ItemType itemType, int quantity) {
    addInput(config_model::toItemId(itemType), quantity);
}

void Recipe::addOutput(ProductId productId, int quantity) {
    outputs_[productId] += quantity;
}

bool Recipe::requiresItem(const std::string& itemId) const {
    return inputs_.find(itemId) != inputs_.end();
}

bool Recipe::requiresItem(ItemType itemType) const {
    return requiresItem(config_model::toItemId(itemType));
}

int Recipe::requiredQuantity(const std::string& itemId) const {
    const auto found = inputs_.find(itemId);
    return found == inputs_.end() ? 0 : found->second;
}

int Recipe::requiredQuantity(ItemType itemType) const {
    return requiredQuantity(config_model::toItemId(itemType));
}

const std::map<std::string, int>& Recipe::inputs() const {
    return inputs_;
}

const std::map<ProductId, int>& Recipe::outputs() const {
    return outputs_;
}

} // namespace gactorio
