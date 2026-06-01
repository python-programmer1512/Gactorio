#pragma once

#include "model/Factory.hpp"
#include "model/Recipe.hpp"

#include <vector>

namespace gactorio {

class CarbonationFactory final : public Factory {
public:
    CarbonationFactory();

    const std::vector<Recipe>& recipes() const;

protected:
    // Memento helper — maps a ProductId back to a concrete Product instance
    // (VoltzClassic = 101, HyperBolt = 102, AuroraZero = 103).
    std::shared_ptr<Product> createProductById(ProductId id) const override;

private:
    std::vector<Recipe> recipes_;
};

} // namespace gactorio

