#pragma once

#include <memory>
#include <vector>
#include "Factory.hpp"
#include "Product.hpp"
#include "Recipe.hpp"
#include "Inventory.hpp"

class ProductionLine {
private:
    std::vector<std::shared_ptr<Factory>> factories;

public:
    ProductionLine();

    void addFactory(std::shared_ptr<Factory> factory);

    Product run(const Recipe& recipe, Inventory& inventory);

    int getFactoryCount() const;
};
