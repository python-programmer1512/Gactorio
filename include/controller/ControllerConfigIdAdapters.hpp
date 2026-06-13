#pragma once

#include "controller/Controller.h"
#include "model/config/FactoryConfig.hpp"

#include <optional>
#include <string_view>

namespace ctrl {

gactorio::config_model::ProductId toProductId(ProductKind kind);
std::optional<ProductKind> productKindFromId(std::string_view id);

} // namespace ctrl
