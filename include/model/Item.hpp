#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

// Item is the abstract base for every raw input the energy-drink factory
// stocks in its inventory. Concrete items (Ingredient, Water, EmptyBottle,
// Label, Package) only need to identify themselves and return a name.
class Item {
public:
    virtual ~Item();

    virtual ItemType getTypeId() const = 0;
    virtual const std::string& getName() const = 0;

protected:
    explicit Item(std::string name);
    const std::string& storedName() const;

private:
    std::string name_;
};

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

// Helper for view code: look up a human-readable name for an ItemType enum.
class ItemTypeName {
public:
    static const char* get(ItemType type);

private:
    ItemTypeName() = delete;
};

} // namespace gactorio
