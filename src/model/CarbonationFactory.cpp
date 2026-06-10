#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/Product.hpp"

#include <memory>

namespace gactorio {

// CarbonationFactory is the default factory subclass for the energy-drink
// theme. It seeds the inventory with raw materials, builds a single production
// line containing the four stations, and queues a starter Voltz Classic so
// the simulation has something to do as soon as the GUI launches.
CarbonationFactory::CarbonationFactory() {
    // ---- Recipes (currently unused by the runtime but kept for inspection) --
    Recipe voltz(1, "Voltz Classic Brew", 40.0);
    voltz.addInput(ItemType::Ingredient, 2);
    voltz.addInput(ItemType::Water, 1);
    voltz.addOutput(static_cast<ProductId>(ProductType::VoltzClassic), 1);
    recipes_.push_back(voltz);

    Recipe hyper(2, "Hyper Bolt Brew", 48.0);
    hyper.addInput(ItemType::Ingredient, 3);
    hyper.addInput(ItemType::Water, 1);
    hyper.addOutput(static_cast<ProductId>(ProductType::HyperBolt), 1);
    recipes_.push_back(hyper);

    Recipe aurora(3, "Aurora Zero Brew", 49.0);
    aurora.addInput(ItemType::Ingredient, 2);
    aurora.addInput(ItemType::Water, 1);
    aurora.addOutput(static_cast<ProductId>(ProductType::AuroraZero), 1);
    recipes_.push_back(aurora);

    // ---- Initial inventory ------------------------------------------------
    inventory().addItem(ItemType::Ingredient,  100);
    inventory().addItem(ItemType::Water,       100);
    inventory().addItem(ItemType::EmptyBottle,  60);
    inventory().addItem(ItemType::Label,        60);
    inventory().addItem(ItemType::Package,      60);

    // ---- Production line: 4 stations in factory order ---------------------
    ProductionLine line(1, "Beverage Line A");
    line.addMachine(std::make_unique<MixingStation>   (1, "Mixer"));
    line.addMachine(std::make_unique<QualityStation>  (2, "Quality Check"));
    line.addMachine(std::make_unique<BottlingStation> (3, "Filler"));
    line.addMachine(std::make_unique<PackagingStation>(4, "Packager"));

    // Pre-queue one Voltz Classic so the simulation visibly starts.
    line.enqueueProduct(std::make_shared<VoltzClassic>());

    addProductionLine(std::move(line));
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

LineId CarbonationFactory::addDynamicLine() {
    const LineId    id      = nextLineId_++;
    const MachineId baseMid = nextMachineId_;
    nextMachineId_ += 4;

    const std::string letter(1, static_cast<char>('A' + static_cast<int>(id) - 1));

    ProductionLine line(id, std::string("Beverage Line ") + letter);
    line.addMachine(std::make_unique<MixingStation>   (baseMid + 0, "Mixer"));
    line.addMachine(std::make_unique<QualityStation>  (baseMid + 1, "Quality Check"));
    line.addMachine(std::make_unique<BottlingStation> (baseMid + 2, "Filler"));
    line.addMachine(std::make_unique<PackagingStation>(baseMid + 3, "Packager"));
    addProductionLine(std::move(line));
    return id;
}

std::shared_ptr<Product> CarbonationFactory::createProductById(ProductId id) const {
    switch (id) {
    case static_cast<ProductId>(ProductType::VoltzClassic): return std::make_shared<VoltzClassic>();
    case static_cast<ProductId>(ProductType::HyperBolt):    return std::make_shared<HyperBolt>();
    case static_cast<ProductId>(ProductType::AuroraZero):   return std::make_shared<AuroraZero>();
    default: return nullptr;
    }
}

} // namespace gactorio
