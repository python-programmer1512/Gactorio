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
    std::vector<RecipeMemento> exportRecipeStates() const override;
    void restoreRecipeStates(const std::vector<RecipeMemento>& recipes) override;

private:
    std::vector<Recipe> recipes_;
};

} // namespace gactorio
