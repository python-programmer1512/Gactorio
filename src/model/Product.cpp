#include "model/Product.hpp"

#include <utility>

namespace gactorio {

// -----------------------------------------------------------------------------
// ItemRequirement
// -----------------------------------------------------------------------------
ItemRequirement::ItemRequirement(ItemType itemType, int quantity)
    : itemType_(itemType), quantity_(quantity) {}

ItemType ItemRequirement::itemType() const { return itemType_; }
int      ItemRequirement::quantity() const { return quantity_; }

// -----------------------------------------------------------------------------
// ProcessStep
// -----------------------------------------------------------------------------
ProcessStep::ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds)
    : requiredRole_(requiredRole), baseDurationSeconds_(baseDurationSeconds) {}

MachineRole    ProcessStep::requiredRole() const        { return requiredRole_; }
SimulationTime ProcessStep::baseDurationSeconds() const { return baseDurationSeconds_; }
SimulationTime ProcessStep::durationSeconds() const     { return baseDurationSeconds_; }

// -----------------------------------------------------------------------------
// Product (abstract)
// -----------------------------------------------------------------------------
Product::Product(ProductId id,
                 std::string name,
                 std::vector<ItemRequirement> requirements,
                 std::vector<ProcessStep> route)
    : id_(id),
      name_(std::move(name)),
      requirements_(std::move(requirements)),
      route_(std::move(route)) {}

Product::~Product() = default;

ProductId          Product::storedProductId()    const { return id_; }
const std::string& Product::storedName()         const { return name_; }
const std::vector<ItemRequirement>& Product::storedRequirements() const { return requirements_; }
const std::vector<ProcessStep>&     Product::storedRoute()        const { return route_; }

// -----------------------------------------------------------------------------
// Voltz Classic — standard energy drink, fast and cheap.
//   Recipe per data/factory_config.json: 13 + 9 + 9 + 9 = 40s
// -----------------------------------------------------------------------------
VoltzClassic::VoltzClassic()
    : Product(
          101,
          "Voltz Classic",
          {
              ItemRequirement(ItemType::Ingredient,  2),
              ItemRequirement(ItemType::Water,       1),
              ItemRequirement(ItemType::EmptyBottle, 1),
              ItemRequirement(ItemType::Label,       1),
              ItemRequirement(ItemType::Package,     1),
          },
          {
              ProcessStep(MachineRole::Mixing,    13.0),
              ProcessStep(MachineRole::Quality,    9.0),
              ProcessStep(MachineRole::Bottling,   9.0),
              ProcessStep(MachineRole::Packaging,  9.0),
          }) {}

ProductId          VoltzClassic::getProductId() const         { return storedProductId(); }
const std::string& VoltzClassic::getName() const              { return storedName(); }
const std::vector<ItemRequirement>& VoltzClassic::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     VoltzClassic::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Hyper Bolt — premium high-caffeine, heavier MIXING & QUALITY work.
//   Recipe: 18 + 12 + 9 + 9 = 48s
// -----------------------------------------------------------------------------
HyperBolt::HyperBolt()
    : Product(
          102,
          "Hyper Bolt",
          {
              ItemRequirement(ItemType::Ingredient,  3),
              ItemRequirement(ItemType::Water,       1),
              ItemRequirement(ItemType::EmptyBottle, 1),
              ItemRequirement(ItemType::Label,       1),
              ItemRequirement(ItemType::Package,     1),
          },
          {
              ProcessStep(MachineRole::Mixing,    18.0),
              ProcessStep(MachineRole::Quality,   12.0),
              ProcessStep(MachineRole::Bottling,   9.0),
              ProcessStep(MachineRole::Packaging,  9.0),
          }) {}

ProductId          HyperBolt::getProductId() const            { return storedProductId(); }
const std::string& HyperBolt::getName() const                 { return storedName(); }
const std::vector<ItemRequirement>& HyperBolt::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     HyperBolt::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Aurora Zero — sugar-free specialty, very careful QUALITY check and PACKAGING.
//   Recipe: 17 + 12 + 9 + 11 = 49s
// -----------------------------------------------------------------------------
AuroraZero::AuroraZero()
    : Product(
          103,
          "Aurora Zero",
          {
              ItemRequirement(ItemType::Ingredient,  2),
              ItemRequirement(ItemType::Water,       1),
              ItemRequirement(ItemType::EmptyBottle, 1),
              ItemRequirement(ItemType::Label,       1),
              ItemRequirement(ItemType::Package,     1),
          },
          {
              ProcessStep(MachineRole::Mixing,    17.0),
              ProcessStep(MachineRole::Quality,   12.0),
              ProcessStep(MachineRole::Bottling,   9.0),
              ProcessStep(MachineRole::Packaging, 11.0),
          }) {}

ProductId          AuroraZero::getProductId() const           { return storedProductId(); }
const std::string& AuroraZero::getName() const                { return storedName(); }
const std::vector<ItemRequirement>& AuroraZero::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     AuroraZero::getRoute()        const { return storedRoute(); }

} // namespace gactorio
