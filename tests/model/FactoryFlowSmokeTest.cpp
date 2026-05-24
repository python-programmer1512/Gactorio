#include "model/CarbonationFactory.hpp"

#include <cassert>

int main() {
    gactorio::CarbonationFactory factory;

    // Four stations (Mixing/Quality/Bottling/Packaging) on one beverage line.
    assert(factory.machines().size() == 4);
    assert(factory.productionLines().size() == 1);
    assert(factory.productionLines().front().queueLength() == 1);

    // Drive the simulation far enough to finish the queued Voltz Classic
    // (route total is 40s; we give a generous budget for state transitions).
    for (int i = 0; i < 20; ++i) {
        factory.update(10.0);
    }

    // ProductId 101 = VoltzClassic
    const auto& products = factory.inventory().products();
    const auto found = products.find(101);
    assert(found != products.end());
    assert(found->second >= 1);
    assert(factory.productionLines().front().queueLength() == 0);

    return 0;
}
