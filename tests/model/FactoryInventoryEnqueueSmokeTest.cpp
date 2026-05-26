#include "model/Factory.hpp"
#include "model/Product.hpp"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

int main() {
    gactorio::Factory factory;
    factory.inventory().addItem(gactorio::ItemType::Ingredient, 3);
    factory.inventory().addItem(gactorio::ItemType::Water, 1);
    factory.inventory().addItem(gactorio::ItemType::EmptyBottle, 1);
    factory.inventory().addItem(gactorio::ItemType::Label, 1);
    factory.inventory().addItem(gactorio::ItemType::Package, 1);

    gactorio::ProductionLine line(1, "Test Line");
    factory.addProductionLine(std::move(line));

    assert(factory.enqueueProduct(1, std::make_unique<gactorio::HyperBolt>())
           == gactorio::ProductionRequestResult::Success);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Ingredient) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Water) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::EmptyBottle) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Label) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Package) == 0);
    assert(factory.productionLines().front().queueLength() == 1);

    std::vector<gactorio::EventType> enqueueEvents;
    for (const auto& event : factory.eventLog().events()) {
        if (event.type() == gactorio::EventType::InputsConsumed ||
            event.type() == gactorio::EventType::TaskEnqueued) {
            enqueueEvents.push_back(event.type());
        }
    }
    assert(enqueueEvents.size() == 2);
    assert(enqueueEvents[0] == gactorio::EventType::InputsConsumed);
    assert(enqueueEvents[1] == gactorio::EventType::TaskEnqueued);

    assert(factory.enqueueProduct(1, std::make_unique<gactorio::VoltzClassic>())
           == gactorio::ProductionRequestResult::InsufficientMaterials);
    assert(factory.productionLines().front().queueLength() == 1);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Ingredient) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Water) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::EmptyBottle) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Label) == 0);
    assert(factory.inventory().getQuantity(gactorio::ItemType::Package) == 0);
    assert(!factory.eventLog().events().empty());
    assert(factory.eventLog().events().back().type() == gactorio::EventType::Info);

    return 0;
}
