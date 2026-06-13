#include "model/CarbonationFactory.hpp"

#include <cassert>
#include <string>

namespace {

bool hasLogContaining(const gactorio::Factory& factory, const std::string& text) {
    for (const auto& event : factory.eventLog().events()) {
        if (event.message().find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

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

    // Product id "voltz_classic" = Voltz Classic
    const auto& products = factory.inventory().products();
    const auto found = products.find("voltz_classic");
    assert(found != products.end());
    assert(found->second >= 1);
    assert(factory.productionLines().front().queueLength() == 0);

    const auto secondLineId = factory.addDynamicLine();
    assert(factory.setLineScenario(1, gactorio::ScenarioType::Bottleneck));
    assert(!factory.setLineScenario(999, gactorio::ScenarioType::Overflow));

    const auto lineOneScenario = factory.getLineScenario(1);
    assert(lineOneScenario.has_value());
    assert(*lineOneScenario == gactorio::ScenarioType::Bottleneck);
    assert(!factory.getLineScenario(999).has_value());
    assert(hasLogContaining(factory, "Line 1 scenario changed to Bottleneck"));

    const auto& firstLine = factory.productionLines().front();
    const auto& secondLine = factory.productionLines().back();
    assert(firstLine.id() == 1);
    assert(secondLine.id() == secondLineId);

    for (const auto& machine : firstLine.machines()) {
        if (machine->role() == gactorio::MachineRole::Bottling) {
            assert(machine->effectiveProcessingSpeed() == machine->getProcessingSpeed() * 0.5);
        }
    }
    for (const auto& machine : secondLine.machines()) {
        assert(machine->effectiveProcessingSpeed() == machine->getProcessingSpeed());
    }

    assert(factory.setLineScenario(secondLineId, gactorio::ScenarioType::Overflow));
    assert(secondLine.queueCapacity().has_value());
    assert(*secondLine.queueCapacity() == 2);
    assert(!firstLine.queueCapacity().has_value());

    factory.restockItem(gactorio::ItemType::Ingredient, 10);
    factory.restockItem(gactorio::ItemType::Water, 10);
    factory.restockItem(gactorio::ItemType::EmptyBottle, 10);
    factory.restockItem(gactorio::ItemType::Label, 10);
    factory.restockItem(gactorio::ItemType::Package, 10);

    assert(factory.enqueueProduct(secondLineId, std::make_shared<gactorio::VoltzClassic>())
           == gactorio::EnqueueResult::Accepted);
    assert(factory.enqueueProduct(secondLineId, std::make_shared<gactorio::HyperBolt>())
           == gactorio::EnqueueResult::Accepted);
    assert(factory.enqueueProduct(secondLineId, std::make_shared<gactorio::AuroraZero>())
           == gactorio::EnqueueResult::LostOverflow);
    assert(secondLine.queueLength() == 2);
    assert(secondLine.droppedTaskCount() == 1);
    assert(hasLogContaining(factory, "overflow: product task dropped"));

    return 0;
}
