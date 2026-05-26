#include "model/CarbonationFactory.hpp"

#include <cassert>

int main() {
    gactorio::CarbonationFactory factory;

    assert(factory.machines().size() == 5);
    assert(factory.productionLines().size() == 1);
    assert(factory.productionLines().front().queueLength() == 1);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Ingredient) == 98);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Water) == 99);
    assert(factory.inventory().getQuantity(gactorio::ItemType::EmptyBottle) == 59);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Label) == 59);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Package) == 59);

    for (int i = 0; i < 10; ++i) {
        factory.update(10.0);
    }

    const auto& products = factory.inventory().products();
    const auto found = products.find(101);
    assert(found != products.end());
    assert(found->second == 1);
    assert(factory.productionLines().front().queueLength() == 0);

    return 0;
}
