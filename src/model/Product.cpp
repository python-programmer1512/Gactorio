#include "model/Product.hpp"

#include "model/ProductCatalog.hpp"

#include <stdexcept>
#include <utility>

namespace gactorio {

namespace {

const ProductDefinition& definitionFor(ProductType type) {
    const auto* definition = findProductDefinition(type);
    if (definition == nullptr) {
        throw std::logic_error("missing product definition");
    }
    return *definition;
}

} // namespace

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

Product::Product(const ProductDefinition& definition)
    : Product(
          definition.id,
          definition.name,
          definition.requirements,
          definition.route) {}

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
    : Product(definitionFor(ProductType::VoltzClassic)) {}

ProductId          VoltzClassic::getProductId() const         { return storedProductId(); }
const std::string& VoltzClassic::getName() const              { return storedName(); }
const std::vector<ItemRequirement>& VoltzClassic::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     VoltzClassic::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Hyper Bolt — premium high-caffeine, heavier MIXING & QUALITY work.
//   Recipe: 18 + 12 + 9 + 9 = 48s
// -----------------------------------------------------------------------------
HyperBolt::HyperBolt()
    : Product(definitionFor(ProductType::HyperBolt)) {}

ProductId          HyperBolt::getProductId() const            { return storedProductId(); }
const std::string& HyperBolt::getName() const                 { return storedName(); }
const std::vector<ItemRequirement>& HyperBolt::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     HyperBolt::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Aurora Zero — sugar-free specialty, very careful QUALITY check and PACKAGING.
//   Recipe: 17 + 12 + 9 + 11 = 49s
// -----------------------------------------------------------------------------
AuroraZero::AuroraZero()
    : Product(definitionFor(ProductType::AuroraZero)) {}

ProductId          AuroraZero::getProductId() const           { return storedProductId(); }
const std::string& AuroraZero::getName() const                { return storedName(); }
const std::vector<ItemRequirement>& AuroraZero::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     AuroraZero::getRoute()        const { return storedRoute(); }

} // namespace gactorio
