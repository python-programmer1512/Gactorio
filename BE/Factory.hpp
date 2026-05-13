#pragma once

#include <string>
#include "Product.hpp"
#include "Recipe.hpp"
#include "Inventory.hpp"

class Factory {
protected:
    std::string factoryName;

public:
    Factory();
    Factory(const std::string& factoryName);
    virtual ~Factory() = default;

    virtual Product process(const Product& input,
                            const Recipe& recipe,
                            Inventory& inventory) = 0;

    std::string getFactoryName() const;
};
