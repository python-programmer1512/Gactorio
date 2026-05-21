#include "../../include/model/Product.hpp"

#include <sstream>

namespace factory {

Product::Product()
    : name(""),
      caffeineMg(0),
      sugarGram(0),
      volumeMl(0),
      carbonated(false) {
}

Product::Product(
    const std::string& name,
    int caffeineMg,
    int sugarGram,
    int volumeMl,
    bool carbonated
)
    : name(name),
      caffeineMg(caffeineMg),
      sugarGram(sugarGram),
      volumeMl(volumeMl),
      carbonated(carbonated) {
}

void Product::addIngredient(const std::string& ingredientName) {
    ingredients.push_back(ingredientName);
}

std::string Product::getName() const {
    return name;
}

int Product::getCaffeineMg() const {
    return caffeineMg;
}

int Product::getSugarGram() const {
    return sugarGram;
}

int Product::getVolumeMl() const {
    return volumeMl;
}

bool Product::isCarbonated() const {
    return carbonated;
}

std::vector<std::string> Product::getIngredients() const {
    return ingredients;
}

void Product::setName(const std::string& name) {
    this->name = name;
}

void Product::setCaffeineMg(int caffeineMg) {
    if (caffeineMg < 0) {
        this->caffeineMg = 0;
        return;
    }

    this->caffeineMg = caffeineMg;
}

void Product::setSugarGram(int sugarGram) {
    if (sugarGram < 0) {
        this->sugarGram = 0;
        return;
    }

    this->sugarGram = sugarGram;
}

void Product::setVolumeMl(int volumeMl) {
    if (volumeMl < 0) {
        this->volumeMl = 0;
        return;
    }

    this->volumeMl = volumeMl;
}

void Product::setCarbonated(bool carbonated) {
    this->carbonated = carbonated;
}

std::string Product::getInfo() const {
    std::ostringstream oss;

    oss << "===== Product Info =====\n";
    oss << "Name: " << name << "\n";
    oss << "Caffeine: " << caffeineMg << " mg\n";
    oss << "Sugar: " << sugarGram << " g\n";
    oss << "Volume: " << volumeMl << " ml\n";
    oss << "Carbonated: " << (carbonated ? "Yes" : "No") << "\n";

    oss << "Ingredients:\n";

    if (ingredients.empty()) {
        oss << "- No ingredients registered.\n";
    } else {
        for (const std::string& ingredient : ingredients) {
            oss << "- " << ingredient << "\n";
        }
    }

    return oss.str();
}

}