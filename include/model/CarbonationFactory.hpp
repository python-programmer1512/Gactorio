#pragma once

#include "Factory.hpp"
#include "Machine.hpp"

class CarbonationFactory : public Factory {
private:
    Machine carbonator;
    int carbonationLevel;

public:
    CarbonationFactory();

    void setCarbonationLevel(int level);
    int getCarbonationLevel() const;

    Product process(const Product& input,
                    const Recipe& recipe,
                    Inventory& inventory) override;
};
