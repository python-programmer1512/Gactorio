#include "model/Item.hpp"

#include <utility>

// =============================================================================
// Item.cpp — Item 계층 구현. 각 구체 아이템은 생성자에서 이름을 base 에 넘기고,
// getTypeId()/getName() 만 override 한다. 데이터(이름)는 base 가 캡슐화.
// =============================================================================

namespace gactorio {

Item::Item(std::string name) : name_(std::move(name)) {}
Item::~Item() = default;

const std::string& Item::storedName() const { return name_; }   // 파생이 이름 읽는 통로

// -----------------------------------------------------------------------------
// 에너지 드링크 공장의 구체 원자재 5종 (이름과 ItemType 만 제공)
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
// ItemType enum → 표시 이름 변환(View/Controller 가 재고 라벨 만들 때 사용).
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
