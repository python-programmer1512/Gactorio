#include "model/Product.hpp"

#include <stdexcept>
#include <utility>

namespace gactorio {

ItemRequirement::ItemRequirement(ItemType itemType, int quantity)
    : itemType_(itemType), quantity_(quantity) {}

ItemType ItemRequirement::itemType() const {
    return itemType_;
}

int ItemRequirement::quantity() const {
    return quantity_;
}

ProcessStep::ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds)
    : requiredRole_(requiredRole), baseDurationSeconds_(baseDurationSeconds) {}

MachineRole ProcessStep::requiredRole() const {
    return requiredRole_;
}

SimulationTime ProcessStep::baseDurationSeconds() const {
    return baseDurationSeconds_;
}

SimulationTime ProcessStep::durationSeconds() const {
    return baseDurationSeconds_;
}

Product::Product(std::shared_ptr<const ProductDefinition> definition)
    : definition_(std::move(definition)) {
    if (definition_ == nullptr) {
        throw std::invalid_argument("product definition is required");
    }
}

Product::~Product() = default;

ProductId Product::getProductId() const {
    return definition_->id;
}

const std::string& Product::getName() const {
    return definition_->displayName;
}

const std::vector<ItemRequirement>& Product::getRequirements() const {
    return definition_->requirements;
}

const std::vector<ProcessStep>& Product::getRoute() const {
    return definition_->route;
}

} // namespace gactorio
