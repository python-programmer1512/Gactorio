#include "../../include/model/Ingredient.hpp"

namespace factory {

Ingredient::Ingredient()
    : name(""), amount(0.0), unit("") {
}

Ingredient::Ingredient(
    const std::string& name,
    double amount,
    const std::string& unit
)
    : name(name), amount(amount), unit(unit) {
}

const std::string& Ingredient::getName() const {
    return name;
}

double Ingredient::getAmount() const {
    return amount;
}

const std::string& Ingredient::getUnit() const {
    return unit;
}

void Ingredient::setAmount(double amount) {
    if (amount < 0.0) {
        this->amount = 0.0;
        return;
    }

    this->amount = amount;
}

void Ingredient::addAmount(double amount) {
    if (amount <= 0.0) {
        return;
    }

    this->amount += amount;
}

bool Ingredient::consumeAmount(double amount) {
    if (amount <= 0.0) {
        return false;
    }

    if (this->amount < amount) {
        return false;
    }

    this->amount -= amount;
    return true;
}

}