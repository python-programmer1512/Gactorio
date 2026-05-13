#pragma once

#include <map>
#include <string>
#include "Ingredient.hpp"

class Inventory {
private:
    std::map<std::string, Ingredient> stock;

public:
    Inventory();

    void addIngredient(const Ingredient& ingredient);
    bool hasIngredient(const std::string& name, double requiredAmount) const;
    bool useIngredient(const std::string& name, double amount);
    double getAmount(const std::string& name) const;

    std::string getStockInfo() const;
};
