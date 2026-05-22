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
    addProductionLine(std::move(line));
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

} // namespace gactorio
