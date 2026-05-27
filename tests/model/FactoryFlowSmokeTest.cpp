#include "model/CarbonationFactory.hpp"

#include <cassert>

int main() {
    gactorio::CarbonationFactory factory;

    assert(factory.machines().size() == 4);
    assert(factory.productionLines().size() == 1);
    assert(factory.productionLines().front().queueLength() == 1);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Water) == 99);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Syrup) == 100);
    assert(factory.inventory().getQuantity(gactorio::ItemType::CarbonDioxide) == 99);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Can) == 99);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Caffeine) == 100);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Label) == 99);

    const auto savedState = factory.createMemento();
    assert(savedState.recipes.size() == 2);
    assert(savedState.productionLines.size() == 1);
    assert(savedState.productionLines.front().taskQueue.size() == 1);
    assert(savedState.productionLines.front().machines.size() == 4);
    const auto savedEventCount = factory.eventLog().events().size();
    const auto savedStartedTasks = factory.statistics().startedTaskEvents();

    for (int i = 0; i < 10; ++i) {
        factory.update(10.0);
    }

    const auto& products = factory.inventory().products();
    const auto found = products.find(static_cast<gactorio::ProductId>(gactorio::ProductType::SparklingWater));
    assert(found != products.end());
    assert(found->second == 1);
    assert(factory.productionLines().front().queueLength() == 0);

    factory.restoreFromMemento(savedState);
    assert(factory.machines().size() == 4);
    assert(factory.productionLines().size() == 1);
    assert(factory.productionLines().front().queueLength() == 1);
    assert(factory.inventory().products().count(static_cast<gactorio::ProductId>(gactorio::ProductType::SparklingWater)) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Water) == 99);
    assert(factory.eventLog().events().size() == savedEventCount);
    assert(factory.statistics().startedTaskEvents() == savedStartedTasks);
    assert(factory.recipes().size() == 2);

    for (int i = 0; i < 10; ++i) {
        factory.update(10.0);
    }

    const auto& restoredProducts = factory.inventory().products();
    const auto restoredFound = restoredProducts.find(static_cast<gactorio::ProductId>(gactorio::ProductType::SparklingWater));
    assert(restoredFound != restoredProducts.end());
    assert(restoredFound->second == 1);
    assert(factory.productionLines().front().queueLength() == 0);

    return 0;
}
