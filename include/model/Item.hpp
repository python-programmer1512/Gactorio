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

class RawMaterial final : public Item {
public:
    RawMaterial();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class MetalPlate final : public Item {
public:
    MetalPlate();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Screw final : public Item {
public:
    Screw();

    ItemType getTypeId() const override;
    const std::string& getName() const override;
};

class Paint final : public Item {
public:
    Paint();

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
