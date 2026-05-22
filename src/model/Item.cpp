#include "model/Item.hpp"

#include <utility>

namespace gactorio {

Item::Item(std::string name)
    : name_(std::move(name)) {}

Item::~Item() = default;

const std::string& Item::storedName() const {
    return name_;
}

RawMaterial::RawMaterial()
    : Item("Raw Material") {}

ItemType RawMaterial::getTypeId() const {
    return ItemType::RawMaterial;
}

const std::string& RawMaterial::getName() const {
    return storedName();
}

MetalPlate::MetalPlate()
    : Item("Metal Plate") {}

ItemType MetalPlate::getTypeId() const {
    return ItemType::MetalPlate;
}

const std::string& MetalPlate::getName() const {
    return storedName();
}

Screw::Screw()
    : Item("Screw") {}

ItemType Screw::getTypeId() const {
    return ItemType::Screw;
}

const std::string& Screw::getName() const {
    return storedName();
}

Paint::Paint()
    : Item("Paint") {}

ItemType Paint::getTypeId() const {
    return ItemType::Paint;
}

const std::string& Paint::getName() const {
    return storedName();
}

const char* ItemTypeName::get(ItemType type) {
    switch (type) {
    case ItemType::RawMaterial:
        return "Raw Material";
    case ItemType::MetalPlate:
        return "Metal Plate";
    case ItemType::Screw:
        return "Screw";
    case ItemType::Paint:
        return "Paint";
    case ItemType::Unknown:
    default:
        return "Unknown";
    }
}

} // namespace gactorio
