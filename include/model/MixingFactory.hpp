#pragma once

#include "Factory.hpp"
#include "Machine.hpp"

class MixingFactory : public Factory {
private:
    Machine mixer;

public:
    MixingFactory();

    Product process(const Product& input,
                    const Recipe& recipe,
                    Inventory& inventory) override;
};
