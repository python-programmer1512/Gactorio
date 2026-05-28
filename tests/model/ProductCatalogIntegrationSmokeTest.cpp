#include "controller/FactoryController.hpp"
#include "model/CarbonationFactory.hpp"

#include <cassert>
#include <string>

namespace {

constexpr gactorio::ProductId GingerAleProductId = 104;

bool registerGingerAle(gactorio::ProductCatalog& catalog) {
    return catalog.registerProduct({
        GingerAleProductId,
        "Ginger Ale",
        {
            {gactorio::ItemType::Water, 1},
            {gactorio::ItemType::Syrup, 1},
            {gactorio::ItemType::CarbonDioxide, 1},
            {gactorio::ItemType::Can, 1},
            {gactorio::ItemType::Label, 1},
        },
        {
            {gactorio::MachineRole::Carbonator, 0.5},
            {gactorio::MachineRole::Filler, 0.5},
            {gactorio::MachineRole::Sealer, 0.5},
            {gactorio::MachineRole::Labeler, 0.5},
        },
    });
}

int inventoryQuantity(const gactorio::FactorySnapshot& snapshot, const std::string& name) {
    for (const auto& item : snapshot.inventory().items()) {
        if (item.name() == name) {
            return item.quantity();
        }
    }
    return 0;
}

int productQuantity(const gactorio::CarbonationFactory& factory, gactorio::ProductId productId) {
    const auto found = factory.inventory().products().find(productId);
    if (found == factory.inventory().products().end()) {
        return 0;
    }
    return found->second;
}

void updateUntilComplete(gactorio::CarbonationFactory& factory, gactorio::ProductId productId) {
    for (int i = 0; i < 20 && productQuantity(factory, productId) == 0; ++i) {
        factory.update(10.0);
    }
}

} // namespace

int main() {
    {
        gactorio::FactoryController controller;
        assert(registerGingerAle(controller.productCatalog()));
        assert(!registerGingerAle(controller.productCatalog()));

        assert(controller.enqueueProduct(1, GingerAleProductId) == gactorio::FactoryCommandResult::Success);

        for (int i = 0; i < 20 && inventoryQuantity(controller.getFactorySnapshot(), "Ginger Ale") == 0; ++i) {
            controller.tick(10.0);
        }

        const auto snapshot = controller.getFactorySnapshot();
        assert(inventoryQuantity(snapshot, "Ginger Ale") == 1);
    }

    {
        gactorio::CarbonationFactory factory;
        assert(registerGingerAle(factory.productCatalog()));

        auto product = factory.productCatalog().createProduct(GingerAleProductId);
        assert(product != nullptr);
        assert(product->getName() == std::string("Ginger Ale"));
        assert(factory.enqueueProduct(1, std::move(product)) == gactorio::ProductionRequestResult::Success);

        const auto savedState = factory.createMemento();
        bool sawGingerAleTask = false;
        for (const auto& task : savedState.productionLines.front().taskQueue) {
            if (task.productId == GingerAleProductId) {
                sawGingerAleTask = true;
            }
        }
        assert(sawGingerAleTask);

        updateUntilComplete(factory, GingerAleProductId);
        assert(productQuantity(factory, GingerAleProductId) == 1);

        factory.restoreFromMemento(savedState);
        assert(productQuantity(factory, GingerAleProductId) == 0);

        updateUntilComplete(factory, GingerAleProductId);
        assert(productQuantity(factory, GingerAleProductId) == 1);
    }

    return 0;
}
