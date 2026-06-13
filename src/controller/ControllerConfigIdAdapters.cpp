#include "controller/ControllerConfigIdAdapters.hpp"

#include <stdexcept>

namespace ctrl {

gactorio::config_model::ProductId toProductId(ProductKind kind) {
    switch (kind) {
    case ProductKind::VoltzClassic: return "voltz_classic";
    case ProductKind::HyperBolt:    return "hyper_bolt";
    case ProductKind::AuroraZero:   return "aurora_zero";
    case ProductKind::Unknown:
    default:
        throw std::invalid_argument("Unknown ProductKind value");
    }
}

std::optional<ProductKind> productKindFromId(std::string_view id) {
    if (id == "voltz_classic") return ProductKind::VoltzClassic;
    if (id == "hyper_bolt") return ProductKind::HyperBolt;
    if (id == "aurora_zero") return ProductKind::AuroraZero;
    return std::nullopt;
}

} // namespace ctrl
