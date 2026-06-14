#include "controller/Controller.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file.is_open());
    std::ostringstream out;
    out << file.rdbuf();
    return out.str();
}

const ctrl::ProductOption* findProduct(
    const std::vector<ctrl::ProductOption>& products,
    const std::string& id) {
    for (const auto& product : products) {
        if (product.id == id) {
            return &product;
        }
    }
    return nullptr;
}

const ctrl::InventoryEntry* findInventory(
    const std::vector<ctrl::InventoryEntry>& inventory,
    const std::string& id) {
    for (const auto& entry : inventory) {
        if (entry.id == id) {
            return &entry;
        }
    }
    return nullptr;
}

} // namespace

int main() {
    ctrl::Controller controller;

    const auto& products = controller.products();
    const auto* voltz = findProduct(products, "voltz_classic");
    assert(voltz != nullptr);
    assert(voltz->legacyId == static_cast<ctrl::ProductId>(ctrl::ProductKind::VoltzClassic));
    assert(voltz->displayName == "Voltz Classic");
    assert(voltz->defaultRecipeId == "voltz_classic_recipe");

    assert(controller.enqueueProduct(1, static_cast<ctrl::ProductId>(ctrl::ProductKind::VoltzClassic)));
    assert(controller.enqueueProductById(1, "hyper_bolt"));
    assert(controller.enqueueAutoProductById("aurora_zero") == 0);
    assert(!controller.enqueueProductById(1, "not_a_product"));

    auto snapshot = controller.snapshot();
    const auto* ingredient = findInventory(snapshot.inventory, "ingredient");
    assert(ingredient != nullptr);
    assert(ingredient->legacyId != 0);
    assert(!ingredient->displayName.empty());
    assert(ingredient->quantity >= 0);
    assert(!ingredient->isProduct);

    const int beforeRestock = ingredient->quantity;
    assert(controller.restockItemById("ingredient"));
    snapshot = controller.snapshot();
    ingredient = findInventory(snapshot.inventory, "ingredient");
    assert(ingredient != nullptr);
    assert(ingredient->quantity == beforeRestock + 5);
    assert(!controller.restockItemById("not_an_item"));
    assert(!controller.restockItem(static_cast<ctrl::ItemId>(ctrl::ProductKind::VoltzClassic)));

    const auto configText = readTextFile(
        std::filesystem::path("..") / "data" / "factory_config.runtime.json");
    assert(controller.loadFactoryConfigFromString(configText));
    assert(!controller.loadFactoryConfigFromString("{ not valid json }"));

    const auto& configProducts = controller.products();
    voltz = findProduct(configProducts, "voltz_classic");
    assert(voltz != nullptr);
    assert(voltz->displayName == "Voltz Classic");
    assert(voltz->defaultRecipeId == "voltz_classic_recipe");

    snapshot = controller.snapshot();
    ingredient = findInventory(snapshot.inventory, "ingredient");
    assert(ingredient != nullptr);
    assert(ingredient->id == "ingredient");
    assert(ingredient->displayName == "Ingredient");
    assert(ingredient->kind == "raw");
    assert(ingredient->restockable);
    assert(ingredient->restockAmount == 5);

    assert(controller.restockItemById("ingredient"));
    snapshot = controller.snapshot();
    ingredient = findInventory(snapshot.inventory, "ingredient");
    assert(ingredient != nullptr);
    assert(ingredient->quantity == 10);

    return 0;
}
