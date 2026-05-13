#pragma once

#include <string>

class Ingredient {
private:
    std::string name;
    double amount;
    std::string unit;

public:
    Ingredient();
    Ingredient(const std::string& name, double amount, const std::string& unit);

    std::string getName() const;
    double getAmount() const;
    std::string getUnit() const;

    void setAmount(double amount);

    std::string getInfo() const;
};
