#pragma once

#include <string>

namespace factory {

class Ingredient {
private:
    std::string name;
    double amount;
    std::string unit;

public:
    Ingredient();
    Ingredient(const std::string& name, double amount, const std::string& unit);

    const std::string& getName() const;
    double getAmount() const;
    const std::string& getUnit() const;

    void setAmount(double amount);
    void addAmount(double amount);
    bool consumeAmount(double amount);
};

}