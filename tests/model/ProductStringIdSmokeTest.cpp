#include "model/ProductCatalog.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <string>

int main() {
    const auto* byId = gactorio::findProductDefinition("voltz_classic");
    const auto* byType = gactorio::findProductDefinition(gactorio::ProductType::VoltzClassic);

    assert(byId != nullptr);
    assert(byType != nullptr);
    assert(byId == byType);
    assert(byId->id == "voltz_classic");
    assert(byId->defaultRecipeId == "voltz_classic_recipe");
    assert(byId->type == gactorio::ProductType::VoltzClassic);

    const auto product = gactorio::createProduct("voltz_classic");
    assert(product != nullptr);
    assert(product->getProductId() == "voltz_classic");
    assert(product->productId() == "voltz_classic");
    assert(product->defaultRecipeId() == "voltz_classic_recipe");

    const auto productFromType = gactorio::createProduct(gactorio::ProductType::VoltzClassic);
    assert(productFromType != nullptr);
    assert(productFromType->getProductId() == product->getProductId());

    const gactorio::ProductionTask task(product);
    assert(task.getProductId() == "voltz_classic");
    assert(task.productId() == "voltz_classic");
    assert(task.recipeId() == "voltz_classic_recipe");

    assert(gactorio::findProductDefinition("missing_product") == nullptr);
    assert(gactorio::createProduct("missing_product") == nullptr);
    assert(gactorio::findProductDefinition(gactorio::ProductType::Unknown) == nullptr);
    assert(gactorio::createProduct(gactorio::ProductType::Unknown) == nullptr);

    return 0;
}
