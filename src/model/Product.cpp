#include "model/Product.hpp"

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

Product::Product(
    ProductId id,
    std::string name,
    std::vector<ItemRequirement> requirements,
    std::vector<ProcessStep> route)
    : id_(id),
      name_(std::move(name)),
      requirements_(std::move(requirements)),
      route_(std::move(route)) {}

Product::~Product() = default;

ProductId Product::storedProductId() const {
    return id_;
}

const std::string& Product::storedName() const {
    return name_;
}

const std::vector<ItemRequirement>& Product::storedRequirements() const {
    return requirements_;
}

const std::vector<ProcessStep>& Product::storedRoute() const {
    return route_;
}

SodaCan::SodaCan()
    : Product(
          static_cast<ProductId>(ProductType::SodaCan),
          "Soda Can",
          {
              ItemRequirement(ItemType::Water, 1),
              ItemRequirement(ItemType::Syrup, 1),
              ItemRequirement(ItemType::CarbonDioxide, 1),
              ItemRequirement(ItemType::Can, 1),
              ItemRequirement(ItemType::Label, 1),
          },
          {
              ProcessStep(MachineRole::Carbonator, 2.0),
              ProcessStep(MachineRole::Filler, 3.0),
              ProcessStep(MachineRole::Sealer, 1.0),
              ProcessStep(MachineRole::Labeler, 1.0),
          }) {}

ProductId SodaCan::getProductId() const {
    return storedProductId();
}

const std::string& SodaCan::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& SodaCan::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& SodaCan::getRoute() const {
    return storedRoute();
}

SparklingWater::SparklingWater()
    : Product(
          static_cast<ProductId>(ProductType::SparklingWater),
          "Sparkling Water",
          {
              ItemRequirement(ItemType::Water, 1),
              ItemRequirement(ItemType::CarbonDioxide, 1),
              ItemRequirement(ItemType::Can, 1),
              ItemRequirement(ItemType::Label, 1),
          },
          {
              ProcessStep(MachineRole::Carbonator, 2.5),
              ProcessStep(MachineRole::Filler, 2.0),
              ProcessStep(MachineRole::Sealer, 1.0),
              ProcessStep(MachineRole::Labeler, 1.0),
          }) {}

ProductId SparklingWater::getProductId() const {
    return storedProductId();
}

const std::string& SparklingWater::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& SparklingWater::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& SparklingWater::getRoute() const {
    return storedRoute();
}

EnergyDrink::EnergyDrink()
    : Product(
          static_cast<ProductId>(ProductType::EnergyDrink),
          "Energy Drink",
          {
              ItemRequirement(ItemType::Water, 1),
              ItemRequirement(ItemType::Syrup, 2),
              ItemRequirement(ItemType::CarbonDioxide, 1),
              ItemRequirement(ItemType::Can, 1),
              ItemRequirement(ItemType::Caffeine, 1),
              ItemRequirement(ItemType::Label, 1),
          },
          {
              ProcessStep(MachineRole::Carbonator, 3.0),
              ProcessStep(MachineRole::Filler, 4.0),
              ProcessStep(MachineRole::Sealer, 1.0),
              ProcessStep(MachineRole::Labeler, 1.5),
          }) {}

ProductId EnergyDrink::getProductId() const {
    return storedProductId();
}

const std::string& EnergyDrink::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& EnergyDrink::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& EnergyDrink::getRoute() const {
    return storedRoute();
}

} // namespace gactorio
