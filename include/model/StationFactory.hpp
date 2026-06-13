#pragma once

#include "common/Types.hpp"
#include "model/Machine.hpp"
#include "model/config/FactoryConfig.hpp"

#include <memory>

namespace gactorio {

class StationFactory {
public:
    static std::unique_ptr<Machine> create(
        MachineId runtimeId,
        const config_model::StationDefinition& definition);

    static std::unique_ptr<Machine> createConfiguredStation(
        MachineId runtimeId,
        const config_model::StationDefinition& definition);
};

} // namespace gactorio
