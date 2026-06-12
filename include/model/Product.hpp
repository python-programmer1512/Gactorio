#pragma once

#include "common/Types.hpp"

#include <string>
#include <vector>

namespace gactorio {

struct ProductDefinition;

// Item quantity required to start producing a Product unit.
class ItemRequirement {
public:
    ItemRequirement(ItemType itemType, int quantity);
    ItemType itemType() const;
    int quantity() const;

private:
    ItemType itemType_;
    int quantity_;
};

// One stage on a product's route: which kind of station handles it and how
// many seconds it takes at base speed.
class ProcessStep {
public:
    ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds);
    MachineRole requiredRole() const;
    SimulationTime baseDurationSeconds() const;
    SimulationTime durationSeconds() const;

private:
    MachineRole requiredRole_;
    SimulationTime baseDurationSeconds_;
};

// Product is the abstract base for every finished good. Each concrete product
// declares its ingredient list (ItemRequirement) and its route through the
// four stations (ProcessStep) — these are read from JSON-equivalent data
// baked into the constructor today; tomorrow the same shape can be loaded
// from data/factory_config.json.
class Product {
public:
    virtual ~Product();

    virtual ProductId getProductId() const = 0;
    virtual const std::string& getName() const = 0;
    virtual const std::vector<ItemRequirement>& getRequirements() const = 0;
    virtual const std::vector<ProcessStep>& getRoute() const = 0;

protected:
    explicit Product(const ProductDefinition& definition);

    Product(ProductId id,
            std::string name,
            std::vector<ItemRequirement> requirements,
            std::vector<ProcessStep> route);

    ProductId storedProductId() const;
    const std::string& storedName() const;
    const std::vector<ItemRequirement>& storedRequirements() const;
    const std::vector<ProcessStep>& storedRoute() const;

private:
    ProductId id_;
    std::string name_;
    std::vector<ItemRequirement> requirements_;
    std::vector<ProcessStep> route_;
};

// -----------------------------------------------------------------------------
// Three energy drinks. Times mirror data/factory_config.json:
//   Voltz Classic : MIXING 13s, QUALITY 9s, BOTTLING 9s, PACKAGING 9s
//   Hyper Bolt    : MIXING 18s, QUALITY 12s, BOTTLING 9s, PACKAGING 9s
//   Aurora Zero   : MIXING 17s, QUALITY 12s, BOTTLING 9s, PACKAGING 11s
// -----------------------------------------------------------------------------
class VoltzClassic final : public Product {
public:
    VoltzClassic();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class HyperBolt final : public Product {
public:
    HyperBolt();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class AuroraZero final : public Product {
public:
    AuroraZero();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

} // namespace gactorio
