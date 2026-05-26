#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/Product.hpp"

#include <memory>

namespace gactorio {

CarbonationFactory::CarbonationFactory() {
    Recipe sparklingWater(1, "Sparkling Water", 3.0);
    sparklingWater.addInput(ItemType::RawMaterial, 1);
    sparklingWater.addOutput(1, 1);
    recipes_.push_back(sparklingWater);

<<<<<<< Updated upstream
    inventory().addItem(ItemType::RawMaterial, 100);
    inventory().addItem(ItemType::MetalPlate, 100);
    inventory().addItem(ItemType::Screw, 100);
    inventory().addItem(ItemType::Paint, 100);

    ProductionLine line(1, "Default Factory Line");
    line.addMachine(std::make_unique<Cutter>(1, "Cutter"));
    line.addMachine(std::make_unique<Conveyor>(2, "Conveyor C1"));
    line.addMachine(std::make_unique<Assembler>(3, "Assembler"));
    line.addMachine(std::make_unique<Conveyor>(4, "Conveyor C2"));
    line.addMachine(std::make_unique<Painter>(5, "Painter"));
    line.enqueueProduct(std::make_shared<ToyCar>());
=======
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

>>>>>>> Stashed changes
    addProductionLine(std::move(line));

    // Pre-queue one Voltz Classic so the simulation visibly starts.
    (void)enqueueProduct(1, std::make_unique<VoltzClassic>());
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

} // namespace gactorio
