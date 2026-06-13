#include "model/ConfiguredStation.hpp"

#include "model/config/ConfigIdAdapters.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace gactorio {
namespace {

std::string displayNameFor(const config_model::StationDefinition& definition) {
    if (!definition.displayName.empty()) {
        return definition.displayName;
    }
    if (!definition.typeName.empty()) {
        return definition.typeName;
    }
    return definition.id;
}

std::string typeNameFor(const config_model::StationDefinition& definition) {
    if (!definition.typeName.empty()) {
        return definition.typeName;
    }
    if (!definition.displayName.empty()) {
        return definition.displayName;
    }
    return definition.kind;
}

MachineRole requireMachineRole(const config_model::StationDefinition& definition) {
    const auto role = config_model::machineRoleFromKind(definition.kind);
    if (!role.has_value()) {
        throw std::invalid_argument("Unsupported station kind for runtime MachineRole: " + definition.kind);
    }
    return *role;
}

ProcessType requireProcessType(const config_model::StationDefinition& definition) {
    const auto processType = config_model::processTypeFromKind(definition.kind);
    if (!processType.has_value()) {
        throw std::invalid_argument("Unsupported station kind for runtime ProcessType: " + definition.kind);
    }
    return *processType;
}

} // namespace

ConfiguredStation::ConfiguredStation(
    MachineId runtimeId,
    const config_model::StationDefinition& definition)
    : Machine(
          runtimeId,
          displayNameFor(definition),
          definition.processingSpeed,
          definition.maxHealth,
          definition.breakdownProbability),
      stationDefinitionId_(definition.id),
      stationKind_(definition.kind),
      typeName_(typeNameFor(definition)),
      acceptedStepKinds_(definition.acceptedStepKinds),
      processType_(requireProcessType(definition)),
      role_(requireMachineRole(definition)) {}

const std::string& ConfiguredStation::stationDefinitionId() const noexcept {
    return stationDefinitionId_;
}

const std::string& ConfiguredStation::stationKind() const noexcept {
    return stationKind_;
}

bool ConfiguredStation::acceptsStep(std::string_view stepKind) const {
    if (stepKind.empty()) {
        return false;
    }
    if (acceptedStepKinds_.empty()) {
        return stationKind_ == stepKind;
    }
    return std::any_of(
        acceptedStepKinds_.begin(),
        acceptedStepKinds_.end(),
        [stepKind](const std::string& accepted) {
            return accepted == stepKind;
        });
}

std::string ConfiguredStation::typeName() const {
    return typeName_;
}

ProcessType ConfiguredStation::processType() const {
    return processType_;
}

MachineRole ConfiguredStation::role() const {
    return role_;
}

bool ConfiguredStation::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

} // namespace gactorio
