#pragma once

#include <string>
#include "Factory.hpp"
#include "Machine.hpp"

class PackagingFactory : public Factory {
private:
    Machine packager;
    std::string packageType;

public:
    PackagingFactory();

    void setPackageType(const std::string& packageType);
    std::string getPackageType() const;

    Product process(const Product& input,
                    const Recipe& recipe,
                    Inventory& inventory) override;
};
