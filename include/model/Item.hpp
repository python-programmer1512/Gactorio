#pragma once

// =============================================================================
// Item — 원자재(raw material) 타입 계층
// -----------------------------------------------------------------------------
// OOP 4대 요소 중 "추상화/상속/다형성" 데모용 계층.
//   Item(추상 기반)  ──▷  Ingredient / Water / EmptyBottle / Label / Package
// 각 구체 아이템은 자기 종류(ItemType)와 표시 이름만 책임진다. 동작(시뮬레이션)은
// 없다 — 재고(Inventory)에 담기는 "값"에 가깝다.
//
// 캡슐화: 이름(name_)은 private 이며 protected 접근자 storedName() 으로만 노출한다.
// (과제 요구: 시뮬레이션 클래스에 public 데이터 멤버가 없어야 함)
// =============================================================================

#include "common/Types.hpp"

#include <string>

namespace gactorio {

// 모든 원자재의 추상 기반 클래스. getTypeId()/getName() 은 순수 가상이므로
// Item 자체로는 인스턴스화할 수 없다(= 추상 클래스).
class Item {
public:
    virtual ~Item();   // 다형적 소멸을 위해 가상 소멸자

    virtual ItemType getTypeId() const = 0;            // 이 아이템의 종류
    virtual const std::string& getName() const = 0;    // 표시 이름

protected:
    explicit Item(std::string name);          // 파생 클래스만 이름을 정해 생성
    const std::string& storedName() const;    // 파생 클래스가 이름을 읽는 통로

private:
    std::string name_;   // 표시 이름(캡슐화)
};

// 이하 5종 구체 아이템: final(더 이상 상속 불가) + override 로 종류/이름만 제공.
class Ingredient final : public Item {
public:
    Ingredient();
    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Water final : public Item {
public:
    Water();
    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class EmptyBottle final : public Item {
public:
    EmptyBottle();
    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Label final : public Item {
public:
    Label();
    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Package final : public Item {
public:
    Package();
    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

// View/Controller 가 ItemType enum 을 사람이 읽는 이름으로 바꿀 때 쓰는 헬퍼.
// 생성자를 delete 하여 인스턴스화 금지 → static get()만 사용하는 유틸 클래스.
class ItemTypeName {
public:
    static const char* get(ItemType type);

private:
    ItemTypeName() = delete;
};

} // namespace gactorio
