#pragma once

#include "model/Machine.hpp"
#include "model/config/FactoryConfig.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace gactorio {

class ConfiguredStation final : public Machine {
public:
    ConfiguredStation(
        MachineId runtimeId,
        const config_model::StationDefinition& definition);

    const std::string& stationDefinitionId() const override;
    std::string stationKind() const override;
    bool acceptsStep(std::string_view stepKind) const override;

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;

private:
    std::string stationDefinitionId_;
    std::string stationKind_;
    std::string typeName_;
    std::vector<std::string> acceptedStepKinds_;
    ProcessType processType_;
    MachineRole role_;
};

} // namespace gactorio
