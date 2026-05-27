#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/Product.hpp"

#include <memory>

namespace gactorio {

CarbonationFactory::CarbonationFactory() {
    Recipe sodaCan(1, "Soda Can", 4.0);
    sodaCan.addInput(ItemType::Water, 1);
    sodaCan.addInput(ItemType::Syrup, 1);
    sodaCan.addInput(ItemType::CarbonDioxide, 1);
    sodaCan.addInput(ItemType::Can, 1);
    sodaCan.addInput(ItemType::Label, 1);
    sodaCan.addOutput(static_cast<ProductId>(ProductType::SodaCan), 1);
    recipes_.push_back(sodaCan);

    Recipe sparklingWater(2, "Sparkling Water", 3.0);
    sparklingWater.addInput(ItemType::Water, 1);
    sparklingWater.addInput(ItemType::CarbonDioxide, 1);
    sparklingWater.addInput(ItemType::Can, 1);
    sparklingWater.addInput(ItemType::Label, 1);
    sparklingWater.addOutput(static_cast<ProductId>(ProductType::SparklingWater), 1);
    recipes_.push_back(sparklingWater);

    inventory().addItem(ItemType::Water, 100);
    inventory().addItem(ItemType::Syrup, 100);
    inventory().addItem(ItemType::CarbonDioxide, 100);
    inventory().addItem(ItemType::Can, 100);
    inventory().addItem(ItemType::Caffeine, 100);
    inventory().addItem(ItemType::Label, 100);

    ProductionLine line(1, "Beverage Line");
    line.addMachine(std::make_unique<Carbonator>(1, "Carbonator"));
    line.addMachine(std::make_unique<Filler>(2, "Filler"));
    line.addMachine(std::make_unique<Sealer>(3, "Sealer"));
    line.addMachine(std::make_unique<Labeler>(4, "Labeler"));
    addProductionLine(std::move(line));

    (void)enqueueProduct(1, std::make_unique<SparklingWater>());
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

} // namespace gactorio
