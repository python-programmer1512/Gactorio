#include "model/Item.hpp"

#include <utility>

namespace gactorio {

Item::Item(std::string name)
    : name_(std::move(name)) {}

Item::~Item() = default;

const std::string& Item::storedName() const {
    return name_;
}

Water::Water()
    : Item("Water") {}

ItemType Water::getTypeId() const {
    return ItemType::Water;
}

const std::string& Water::getName() const {
    return storedName();
}

Syrup::Syrup()
    : Item("Syrup") {}

ItemType Syrup::getTypeId() const {
    return ItemType::Syrup;
}

const std::string& Syrup::getName() const {
    return storedName();
}

CarbonDioxide::CarbonDioxide()
    : Item("Carbon Dioxide") {}

ItemType CarbonDioxide::getTypeId() const {
    return ItemType::CarbonDioxide;
}

const std::string& CarbonDioxide::getName() const {
    return storedName();
}

Can::Can()
    : Item("Can") {}

ItemType Can::getTypeId() const {
    return ItemType::Can;
}

const std::string& Can::getName() const {
    return storedName();
}

Caffeine::Caffeine()
    : Item("Caffeine") {}

ItemType Caffeine::getTypeId() const {
    return ItemType::Caffeine;
}

const std::string& Caffeine::getName() const {
    return storedName();
}

Label::Label()
    : Item("Label") {}

ItemType Label::getTypeId() const {
    return ItemType::Label;
}

const std::string& Label::getName() const {
    return storedName();
}

const char* ItemTypeName::get(ItemType type) {
    switch (type) {
    case ItemType::Water:
        return "Water";
    case ItemType::Syrup:
        return "Syrup";
    case ItemType::CarbonDioxide:
        return "Carbon Dioxide";
    case ItemType::Can:
        return "Can";
    case ItemType::Caffeine:
        return "Caffeine";
    case ItemType::Label:
        return "Label";
    case ItemType::Unknown:
    default:
        return "Unknown";
    }
}

} // namespace gactorio
