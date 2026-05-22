#pragma once

#include "common/Types.hpp"

#include <string>
#include <vector>

namespace gactorio {

class ItemRequirement {
public:
    ItemRequirement(ItemType itemType, int quantity);

    ItemType itemType() const;
    int quantity() const;

private:
    ItemType itemType_;
    int quantity_;
};

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

class Product {
public:
    virtual ~Product();

    virtual ProductId getProductId() const = 0;
    virtual const std::string& getName() const = 0;
    virtual const std::vector<ItemRequirement>& getRequirements() const = 0;
    virtual const std::vector<ProcessStep>& getRoute() const = 0;

protected:
    Product(ProductId id, std::string name, std::vector<ItemRequirement> requirements, std::vector<ProcessStep> route);

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

class ToyCar final : public Product {
public:
    ToyCar();

    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class MetalBox final : public Product {
public:
    MetalBox();

    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class DroneFrame final : public Product {
public:
    DroneFrame();

    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

} // namespace gactorio
