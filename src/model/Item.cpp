#include "model/Item.hpp"

#include <utility>

namespace gactorio {

Item::Item(std::string name) : name_(std::move(name)) {}
Item::~Item() = default;

const std::string& Item::storedName() const { return name_; }

// -----------------------------------------------------------------------------
// Concrete items for the energy-drink factory
// -----------------------------------------------------------------------------
Ingredient::Ingredient() : Item("Ingredient") {}
ItemType Ingredient::getTypeId() const           { return ItemType::Ingredient; }
const std::string& Ingredient::getName() const   { return storedName(); }

Water::Water() : Item("Water") {}
ItemType Water::getTypeId() const                { return ItemType::Water; }
const std::string& Water::getName() const        { return storedName(); }

EmptyBottle::EmptyBottle() : Item("Empty Bottle") {}
ItemType EmptyBottle::getTypeId() const          { return ItemType::EmptyBottle; }
const std::string& EmptyBottle::getName() const  { return storedName(); }

Label::Label() : Item("Label") {}
ItemType Label::getTypeId() const                { return ItemType::Label; }
const std::string& Label::getName() const        { return storedName(); }

Package::Package() : Item("Package") {}
ItemType Package::getTypeId() const              { return ItemType::Package; }
const std::string& Package::getName() const      { return storedName(); }

// -----------------------------------------------------------------------------
const char* ItemTypeName::get(ItemType type) {
    switch (type) {
    case ItemType::Ingredient:  return "Ingredient";
    case ItemType::Water:       return "Water";
    case ItemType::EmptyBottle: return "Empty Bottle";
    case ItemType::Label:       return "Label";
    case ItemType::Package:     return "Package";
    case ItemType::Unknown:
    default:                    return "Unknown";
    }
}

} // namespace gactorio
