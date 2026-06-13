#include "model/StationFactory.hpp"

#include "model/ConfiguredStation.hpp"

#include <memory>

namespace gactorio {

std::unique_ptr<Machine> StationFactory::create(
    MachineId runtimeId,
    const config_model::StationDefinition& definition) {
    return createConfiguredStation(runtimeId, definition);
}

std::unique_ptr<Machine> StationFactory::createConfiguredStation(
    MachineId runtimeId,
    const config_model::StationDefinition& definition) {
    return std::make_unique<ConfiguredStation>(runtimeId, definition);
}

} // namespace gactorio
