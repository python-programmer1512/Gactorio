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

struct ProductDefinition {
    ProductId id = 0;
    std::string displayName;
    std::vector<ItemRequirement> requirements;
    std::vector<ProcessStep> route;
};

} // namespace gactorio
