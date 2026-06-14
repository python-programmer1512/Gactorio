#include "model/Inventory.hpp"

#include <cassert>
#include <map>
#include <string>

int main() {
    gactorio::Inventory inventory;

    inventory.addItem("ingredient", 5);
    assert(inventory.getItemQuantity("ingredient") == 5);
    assert(inventory.hasItem("ingredient", 5));
    assert(!inventory.hasItem("ingredient", 6));

    assert(inventory.consumeItem("ingredient", 2));
    assert(inventory.getItemQuantity("ingredient") == 3);
    assert(!inventory.consumeItem("ingredient", 4));
    assert(inventory.getItemQuantity("ingredient") == 3);

    const std::map<std::string, int> affordable = {
        {"ingredient", 2},
    };
    assert(inventory.canConsume(affordable));
    assert(inventory.consume(affordable));
    assert(inventory.getItemQuantity("ingredient") == 1);

    const std::map<std::string, int> tooExpensive = {
        {"ingredient", 2},
    };
    assert(!inventory.canConsume(tooExpensive));
    assert(!inventory.consume(tooExpensive));

    inventory.addItem(gactorio::ItemType::Water, 5);
    assert(inventory.getItemQuantity("water") == 5);
    assert(inventory.getQuantity(gactorio::ItemType::Water) == 5);

    const auto& items = inventory.items();
    assert(items.find("ingredient") != items.end());
    assert(items.find("water") != items.end());

    return 0;
}
