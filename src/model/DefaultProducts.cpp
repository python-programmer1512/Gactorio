#include "model/DefaultProducts.hpp"

namespace gactorio {

namespace {

constexpr ProductId SodaCanProductId = 101;
constexpr ProductId SparklingWaterProductId = 102;
constexpr ProductId EnergyDrinkProductId = 103;

} // namespace

void registerDefaultProducts(ProductCatalog& catalog) {
    catalog.registerProduct({
        SodaCanProductId,
        "Soda Can",
        {
            ItemRequirement(ItemType::Water, 1),
            ItemRequirement(ItemType::Syrup, 1),
            ItemRequirement(ItemType::CarbonDioxide, 1),
            ItemRequirement(ItemType::Can, 1),
            ItemRequirement(ItemType::Label, 1),
        },
        {
            ProcessStep(MachineRole::Carbonator, 2.0),
            ProcessStep(MachineRole::Filler, 3.0),
            ProcessStep(MachineRole::Sealer, 1.0),
            ProcessStep(MachineRole::Labeler, 1.0),
        },
    });

    catalog.registerProduct({
        SparklingWaterProductId,
        "Sparkling Water",
        {
            ItemRequirement(ItemType::Water, 1),
            ItemRequirement(ItemType::CarbonDioxide, 1),
            ItemRequirement(ItemType::Can, 1),
            ItemRequirement(ItemType::Label, 1),
        },
        {
            ProcessStep(MachineRole::Carbonator, 2.5),
            ProcessStep(MachineRole::Filler, 2.0),
            ProcessStep(MachineRole::Sealer, 1.0),
            ProcessStep(MachineRole::Labeler, 1.0),
        },
    });

    catalog.registerProduct({
        EnergyDrinkProductId,
        "Energy Drink",
        {
            ItemRequirement(ItemType::Water, 1),
            ItemRequirement(ItemType::Syrup, 2),
            ItemRequirement(ItemType::CarbonDioxide, 1),
            ItemRequirement(ItemType::Can, 1),
            ItemRequirement(ItemType::Caffeine, 1),
            ItemRequirement(ItemType::Label, 1),
        },
        {
            ProcessStep(MachineRole::Carbonator, 3.0),
            ProcessStep(MachineRole::Filler, 4.0),
            ProcessStep(MachineRole::Sealer, 1.0),
            ProcessStep(MachineRole::Labeler, 1.5),
        },
    });
}

} // namespace gactorio
