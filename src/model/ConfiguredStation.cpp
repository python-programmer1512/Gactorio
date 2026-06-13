#include "model/ConfiguredStation.hpp"

#include "model/config/ConfigIdAdapters.hpp"

#include <algorithm>
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
      processType_(config_model::processTypeFromKind(definition.kind).value_or(ProcessType::Unknown)),
      role_(config_model::machineRoleFromKind(definition.kind).value_or(MachineRole::Unknown)) {}

const std::string& ConfiguredStation::stationDefinitionId() const {
    return stationDefinitionId_;
}

std::string ConfiguredStation::stationKind() const {
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
