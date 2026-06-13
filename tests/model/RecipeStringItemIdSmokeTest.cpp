#include "model/Inventory.hpp"
#include "model/Product.hpp"
#include "model/Recipe.hpp"

#include <cassert>
#include <map>
#include <string>
#include <vector>

int main() {
    gactorio::Recipe recipe("string_recipe", "String Recipe", 12.0);
    recipe.addInput("ingredient", 2);
    recipe.addInput("water", 1);
    recipe.addInput(gactorio::ItemType::Label, 1);

    assert(recipe.inputs().at("ingredient") == 2);
    assert(recipe.inputs().at("water") == 1);
    assert(recipe.inputs().at("label") == 1);
    assert(recipe.requiresItem("ingredient"));
    assert(recipe.requiresItem(gactorio::ItemType::Label));
    assert(recipe.requiredQuantity("missing_item") == 0);
    assert(recipe.requiredQuantity(gactorio::ItemType::Label) == 1);

    gactorio::Inventory inventory;
    inventory.addItem("ingredient", 2);
    inventory.addItem("water", 1);
    inventory.addItem("label", 1);

    assert(inventory.canConsume(recipe.inputs()));
    assert(inventory.consume(recipe.inputs()));
    assert(inventory.getItemQuantity("ingredient") == 0);
    assert(inventory.getItemQuantity("water") == 0);
    assert(inventory.getItemQuantity("label") == 0);

    const gactorio::ItemRequirement stringRequirement("custom_flavor", 3);
    assert(stringRequirement.itemId() == "custom_flavor");
    assert(stringRequirement.itemType() == gactorio::ItemType::Unknown);
    assert(stringRequirement.quantity() == 3);

    const gactorio::ItemRequirement enumRequirement(gactorio::ItemType::Package, 4);
    assert(enumRequirement.itemId() == "package");
    assert(enumRequirement.itemType() == gactorio::ItemType::Package);

    const std::vector<gactorio::ItemRequirement> customRequirements = {
        gactorio::ItemRequirement("custom_flavor", 3),
    };
    inventory.addItem("custom_flavor", 3);
    assert(inventory.hasEnough(customRequirements));
    assert(inventory.consume(customRequirements));
    assert(inventory.getItemQuantity("custom_flavor") == 0);

    return 0;
}
