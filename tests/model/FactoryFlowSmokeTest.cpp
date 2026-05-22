#include "model/CarbonationFactory.hpp"

#include <cassert>

int main() {
    gactorio::CarbonationFactory factory;

    assert(factory.machines().size() == 5);
    assert(factory.productionLines().size() == 1);
    assert(factory.productionLines().front().queueLength() == 1);

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
