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

ToyCar::ToyCar()
    : Product(
          101,
          "Toy Car",
          {
              ItemRequirement(ItemType::MetalPlate, 2),
              ItemRequirement(ItemType::Screw, 4),
              ItemRequirement(ItemType::Paint, 1),
          },
          {
              ProcessStep(MachineRole::Processor, 2.0),
              ProcessStep(MachineRole::Buffer, 0.5),
              ProcessStep(MachineRole::Producer, 4.0),
              ProcessStep(MachineRole::Buffer, 0.5),
              ProcessStep(MachineRole::Output, 1.5),
          }) {}

ProductId ToyCar::getProductId() const {
    return storedProductId();
}

const std::string& ToyCar::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& ToyCar::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& ToyCar::getRoute() const {
    return storedRoute();
}

MetalBox::MetalBox()
    : Product(
          102,
          "Metal Box",
          {
              ItemRequirement(ItemType::MetalPlate, 3),
              ItemRequirement(ItemType::Screw, 2),
          },
          {
              ProcessStep(MachineRole::Processor, 3.0),
              ProcessStep(MachineRole::Buffer, 0.5),
          }) {}

ProductId MetalBox::getProductId() const {
    return storedProductId();
}

const std::string& MetalBox::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& MetalBox::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& MetalBox::getRoute() const {
    return storedRoute();
}

DroneFrame::DroneFrame()
    : Product(
          103,
          "Drone Frame",
          {
              ItemRequirement(ItemType::RawMaterial, 2),
              ItemRequirement(ItemType::MetalPlate, 6),
              ItemRequirement(ItemType::Screw, 8),
          },
          {
              ProcessStep(MachineRole::Processor, 2.0),
              ProcessStep(MachineRole::Producer, 6.0),
              ProcessStep(MachineRole::Output, 2.0),
          }) {}

ProductId DroneFrame::getProductId() const {
    return storedProductId();
}

const std::string& DroneFrame::getName() const {
    return storedName();
}

const std::vector<ItemRequirement>& DroneFrame::getRequirements() const {
    return storedRequirements();
}

const std::vector<ProcessStep>& DroneFrame::getRoute() const {
    return storedRoute();
}

} // namespace gactorio
