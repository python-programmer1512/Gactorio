#include "model/config/ConfigIdAdapters.hpp"

#include <stdexcept>

namespace gactorio::config_model {
namespace {

std::invalid_argument unknownEnum(const char* enumName) {
    return std::invalid_argument(std::string("Unknown ") + enumName + " value");
}

std::optional<MachineRole> roleLikeFromKind(std::string_view kind) {
    if (kind == "mixing") return MachineRole::Mixing;
    if (kind == "quality") return MachineRole::Quality;
    if (kind == "bottling") return MachineRole::Bottling;
    if (kind == "packaging") return MachineRole::Packaging;
    return std::nullopt;
}

std::optional<ProcessType> processLikeFromKind(std::string_view kind) {
    if (kind == "mixing") return ProcessType::Mixing;
    if (kind == "quality") return ProcessType::Quality;
    if (kind == "bottling") return ProcessType::Bottling;
    if (kind == "packaging") return ProcessType::Packaging;
    return std::nullopt;
}

} // namespace

ItemId toItemId(ItemType type) {
    switch (type) {
    case ItemType::Ingredient:  return "ingredient";
    case ItemType::Water:       return "water";
    case ItemType::EmptyBottle: return "empty_bottle";
    case ItemType::Label:       return "label";
    case ItemType::Package:     return "package";
    case ItemType::Unknown:
    default:
        throw unknownEnum("ItemType");
    }
}

std::optional<ItemType> itemTypeFromId(std::string_view id) {
    if (id == "ingredient") return ItemType::Ingredient;
    if (id == "water") return ItemType::Water;
    if (id == "empty_bottle") return ItemType::EmptyBottle;
    if (id == "label") return ItemType::Label;
    if (id == "package") return ItemType::Package;
    return std::nullopt;
}

ProductId toProductId(ProductType type) {
    switch (type) {
    case ProductType::VoltzClassic: return "voltz_classic";
    case ProductType::HyperBolt:    return "hyper_bolt";
    case ProductType::AuroraZero:   return "aurora_zero";
    case ProductType::Unknown:
    default:
        throw unknownEnum("ProductType");
    }
}

std::optional<ProductType> productTypeFromId(std::string_view id) {
    if (id == "voltz_classic") return ProductType::VoltzClassic;
    if (id == "hyper_bolt") return ProductType::HyperBolt;
    if (id == "aurora_zero") return ProductType::AuroraZero;
    return std::nullopt;
}

StationKind toStationKind(MachineRole role) {
    return toStepKind(role);
}

StepKind toStepKind(MachineRole role) {
    switch (role) {
    case MachineRole::Mixing:    return "mixing";
    case MachineRole::Quality:   return "quality";
    case MachineRole::Bottling:  return "bottling";
    case MachineRole::Packaging: return "packaging";
    case MachineRole::Unknown:
    default:
        throw unknownEnum("MachineRole");
    }
}

std::optional<MachineRole> machineRoleFromKind(std::string_view kind) {
    return roleLikeFromKind(kind);
}

StationKind toStationKind(ProcessType process) {
    return toStepKind(process);
}

StepKind toStepKind(ProcessType process) {
    switch (process) {
    case ProcessType::Mixing:    return "mixing";
    case ProcessType::Quality:   return "quality";
    case ProcessType::Bottling:  return "bottling";
    case ProcessType::Packaging: return "packaging";
    case ProcessType::Unknown:
    default:
        throw unknownEnum("ProcessType");
    }
}

std::optional<ProcessType> processTypeFromKind(std::string_view kind) {
    return processLikeFromKind(kind);
}

} // namespace gactorio::config_model
