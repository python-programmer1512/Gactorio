#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

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

class Water final : public Item {
public:
    Water();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Syrup final : public Item {
public:
    Syrup();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class CarbonDioxide final : public Item {
public:
    CarbonDioxide();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Can final : public Item {
public:
    Can();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Caffeine final : public Item {
public:
    Caffeine();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Label final : public Item {
public:
    Label();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class ItemTypeName {
public:
    static const char* get(ItemType type);
    
private:
    ItemTypeName() = delete;
};

} // namespace gactorio
