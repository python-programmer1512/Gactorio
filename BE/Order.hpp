#pragma once

#include <string>
#include "Recipe.hpp"

class Order {
private:
    std::string customerName;
    Recipe recipe;
    int quantity;

public:
    Order();
    Order(const std::string& customerName, const Recipe& recipe, int quantity);

    std::string getCustomerName() const;
    Recipe getRecipe() const;
    int getQuantity() const;

    std::string getInfo() const;
};
