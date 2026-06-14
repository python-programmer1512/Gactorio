#pragma once

#include "common/Types.hpp"
#include "model/config/FactoryConfig.hpp"

#include <optional>
#include <string_view>

namespace gactorio::config_model {

ItemId toItemId(ItemType type);
std::optional<ItemType> itemTypeFromId(std::string_view id);

ProductId toProductId(ProductType type);
std::optional<ProductType> productTypeFromId(std::string_view id);

StationKind toStationKind(MachineRole role);
StepKind toStepKind(MachineRole role);
std::optional<MachineRole> machineRoleFromKind(std::string_view kind);

StationKind toStationKind(ProcessType process);
StepKind toStepKind(ProcessType process);
std::optional<ProcessType> processTypeFromKind(std::string_view kind);

} // namespace gactorio::config_model
