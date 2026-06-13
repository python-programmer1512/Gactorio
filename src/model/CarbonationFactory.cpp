#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/ProductCatalog.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace gactorio {

namespace {

constexpr int kInitialRawItemQuantity = 5;

std::string beverageLineName(LineId id) {
    const std::string letter(1, static_cast<char>('A' + static_cast<int>(id) - 1));
    return std::string("Beverage Line ") + letter;
}

ProductionLine makeBeverageLine(LineId id, const std::array<MachineId, 4>& machineIds) {
    ProductionLine line(id, beverageLineName(id));
    line.addMachine(std::make_unique<MixingStation>   (machineIds[0], "Mixer"));
    line.addMachine(std::make_unique<QualityStation>  (machineIds[1], "Quality Check"));
    line.addMachine(std::make_unique<BottlingStation> (machineIds[2], "Filler"));
    line.addMachine(std::make_unique<PackagingStation>(machineIds[3], "Packager"));
    return line;
}

LineId nextLineIdAfter(const std::vector<ProductionLine>& lines) {
    LineId next = 1;
    for (const auto& line : lines) {
        next = std::max(next, line.id() + 1);
    }
    return next;
}

MachineId nextMachineIdAfter(const std::vector<Machine*>& machines) {
    MachineId next = 1;
    for (const auto* machine : machines) {
        if (machine != nullptr) {
            next = std::max(next, machine->id() + 1);
        }
    }
    return next;
}

} // namespace

// CarbonationFactory is the default factory subclass for the energy-drink
// theme. It seeds the inventory with raw materials, builds a single production
// line containing the four stations, and queues a starter Voltz Classic so
// the simulation has something to do as soon as the GUI launches.
CarbonationFactory::CarbonationFactory() {
    // ---- Recipes (currently unused by the runtime but kept for inspection) --
    for (const auto& definition : productDefinitions()) {
        Recipe recipe(definition.defaultRecipeId, definition.name + " Brew", definition.totalDurationSeconds);
        for (const auto& requirement : definition.requirements) {
            recipe.addInput(requirement.itemId(), requirement.quantity());
        }
        recipe.addOutput(definition.id, 1);
        recipes_.push_back(std::move(recipe));
    }

    // ---- Initial inventory ------------------------------------------------
    inventory().addItem(ItemType::Ingredient,  kInitialRawItemQuantity);
    inventory().addItem(ItemType::Water,       kInitialRawItemQuantity);
    inventory().addItem(ItemType::EmptyBottle, kInitialRawItemQuantity);
    inventory().addItem(ItemType::Label,       kInitialRawItemQuantity);
    inventory().addItem(ItemType::Package,     kInitialRawItemQuantity);

    // ---- Production line: 4 stations in factory order ---------------------
    ProductionLine line = makeBeverageLine(1, {1, 2, 3, 4});

    // Pre-queue one Voltz Classic so the simulation visibly starts.
    line.enqueueProduct(createProduct(ProductType::VoltzClassic));

    addProductionLine(std::move(line));
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

LineId CarbonationFactory::addDynamicLine() {
    const LineId    id      = nextLineId_++;
    const MachineId baseMid = nextMachineId_;
    nextMachineId_ += 4;

    ProductionLine line = makeBeverageLine(id, {baseMid + 0, baseMid + 1, baseMid + 2, baseMid + 3});
    addProductionLine(std::move(line));
    return id;
}

FactoryMemento CarbonationFactory::createMemento() const {
    auto memento = Factory::createMemento();
    memento.setNextIds(nextLineId_, nextMachineId_);
    return memento;
}

void CarbonationFactory::restoreFromMemento(const FactoryMemento& memento) {
    Factory::restoreFromMemento(memento);
    nextLineId_ = memento.nextLineId() != 0
        ? memento.nextLineId()
        : nextLineIdAfter(productionLines());
    nextMachineId_ = memento.nextMachineId() != 0
        ? memento.nextMachineId()
        : nextMachineIdAfter(machines());
}

std::shared_ptr<Product> CarbonationFactory::createProductById(ProductId id) const {
    return createProduct(id);
}

std::optional<ProductionLine> CarbonationFactory::createLineForMemento(const LineMemento& memento) const {
    const auto& machineMementos = memento.machines();
    if (machineMementos.size() < 4) {
        return std::nullopt;
    }

    return makeBeverageLine(
        memento.id(),
        {
            machineMementos[0].id(),
            machineMementos[1].id(),
            machineMementos[2].id(),
            machineMementos[3].id(),
        });
}

} // namespace gactorio
