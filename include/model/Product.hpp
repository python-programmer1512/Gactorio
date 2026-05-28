#pragma once

#include "common/Types.hpp"
#include "model/ProductDefinition.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gactorio {

class Product {
public:
    explicit Product(std::shared_ptr<const ProductDefinition> definition);
    ~Product();

    ProductId getProductId() const;
    const std::string& getName() const;
    const std::vector<ItemRequirement>& getRequirements() const;
    const std::vector<ProcessStep>& getRoute() const;

private:
    std::shared_ptr<const ProductDefinition> definition_;
};

} // namespace gactorio
