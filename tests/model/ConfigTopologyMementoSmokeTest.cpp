#include "model/ConfiguredStation.hpp"
#include "model/Factory.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string configJson() {
    return R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" },
    { "id": "mixed_base", "displayName": "Mixed Base" },
    { "id": "flavor_syrup", "displayName": "Flavor Syrup" }
  ],
  "products": [
    {
      "id": "flavored_voltz",
      "displayName": "Flavored Voltz",
      "defaultRecipeId": "flavored_voltz_recipe"
    }
  ],
  "stations": [
    { "id": "mixing_station", "kind": "mixing", "displayName": "Mixer" },
    {
      "id": "flavor_station",
      "kind": "flavoring",
      "displayName": "Flavor Station",
      "typeName": "Custom Flavor Station",
      "acceptedStepKinds": ["flavoring"]
    },
    {
      "id": "spare_flavor_station",
      "kind": "flavoring",
      "displayName": "Spare Flavor Station",
      "typeName": "Spare Custom Flavor Station",
      "acceptedStepKinds": ["flavoring"]
    }
  ],
  "recipes": [
    {
      "id": "flavored_voltz_recipe",
      "productId": "flavored_voltz",
      "displayName": "Flavored Voltz Recipe",
      "steps": [
        {
          "id": "mixing_step",
          "stepKind": "mixing",
          "duration": 10.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 1 }
          ],
          "outputs": [
            { "itemId": "mixed_base", "quantity": 1 }
          ]
        },
        {
          "id": "flavoring_step",
          "stepKind": "flavoring",
          "duration": 1.0,
          "inputs": [
            { "itemId": "mixed_base", "quantity": 1 },
            { "itemId": "flavor_syrup", "quantity": 1 }
          ],
          "outputs": [
            { "productId": "flavored_voltz", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_custom",
      "displayName": "Line Custom",
      "stationIds": ["mixing_station", "flavor_station"],
      "recipeIds": ["flavored_voltz_recipe"]
    },
    {
      "id": "line_spare",
      "displayName": "Line Spare",
      "stationIds": ["mixing_station", "spare_flavor_station"],
      "recipeIds": ["flavored_voltz_recipe"]
    }
  ],
  "initialInventory": [
    { "itemId": "ingredient", "quantity": 1 },
    { "itemId": "flavor_syrup", "quantity": 1 }
  ],
  "startupTasks": [
    { "lineId": "line_custom", "recipeId": "flavored_voltz_recipe", "quantity": 1 }
  ]
}
)json";
}

std::unique_ptr<gactorio::Factory> makeFactory(
    std::unique_ptr<gactorio::config_model::FactoryRuntimeContext>& context) {
    context = std::make_unique<gactorio::config_model::FactoryRuntimeContext>(
        gactorio::config_model::FactoryRuntimeContext::loadFromString(configJson()).config());
    return gactorio::FactoryBuilder::createFactory(*context);
}

int productQuantity(const gactorio::Factory& factory, const std::string& productId) {
    const auto found = factory.inventory().products().find(productId);
    return found == factory.inventory().products().end() ? 0 : found->second;
}

void restoresConfigLineAndCustomStationTopology() {
    std::unique_ptr<gactorio::config_model::FactoryRuntimeContext> context;
    auto factory = makeFactory(context);
    require(factory->productionLines().size() == 2, "factory should start with two lines");

    const auto memento = factory->createMemento();
    require(factory->removeProductionLine(2), "line 2 should be removable");
    require(factory->productionLines().size() == 1, "line 2 should be removed");

    factory->restoreFromMemento(memento);
    require(factory->productionLines().size() == 2, "restore should recreate removed line");

    const auto* restoredLine = factory->findProductionLine(2);
    require(restoredLine != nullptr, "restored line should be found");
    require(restoredLine->definitionId() == "line_spare", "line definition id should be restored");
    require(restoredLine->machines().size() == 2, "restored line should have two stations");
    const auto* station = dynamic_cast<const gactorio::ConfiguredStation*>(
        restoredLine->machines()[1].get());
    require(station != nullptr, "restored station should be configured station");
    require(station->stationDefinitionId() == "spare_flavor_station", "station id should be restored");
    require(station->stationKind() == "flavoring", "station kind should be restored");
    require(station->typeName() == "Spare Custom Flavor Station", "station type name should be restored");
}

void restoresAssignedTaskAndMachineProgress() {
    std::unique_ptr<gactorio::config_model::FactoryRuntimeContext> context;
    auto factory = makeFactory(context);

    factory->update(0.0);
    auto* line = factory->findProductionLine(1);
    require(line != nullptr, "line 1 should exist");
    auto* machine = line->findMachine(1);
    require(machine != nullptr, "machine 1 should exist");
    require(machine->currentTask() != nullptr, "machine should have assigned task");
    require(machine->getStatus() == gactorio::MachineStatus::Working, "machine should be working");
    require(factory->inventory().getItemQuantity("ingredient") == 0, "input should be consumed once");

    factory->update(4.0);
    require(machine->currentTask() != nullptr, "machine should still have task before restore");
    require(machine->getProgress() > 0.0, "machine progress should advance before restore");
    const auto restoredProgressFloor = machine->getProgress();

    const auto memento = factory->createMemento();
    factory->restoreFromMemento(memento);

    line = factory->findProductionLine(1);
    require(line != nullptr, "line 1 should exist after restore");
    machine = line->findMachine(1);
    require(machine != nullptr, "machine 1 should exist after restore");
    require(machine->currentTask() != nullptr, "machine assigned task should be restored");
    require(machine->getStatus() == gactorio::MachineStatus::Working, "machine status should be restored");
    require(machine->getProgress() >= restoredProgressFloor, "machine progress should be restored");
    require(factory->inventory().getItemQuantity("ingredient") == 0, "input should not be consumed again");

    factory->update(6.0);
    require(factory->inventory().getItemQuantity("mixed_base") == 0, "mixed base should feed next step");
    require(factory->inventory().getItemQuantity("flavor_syrup") == 0, "flavor syrup should feed next step");
    factory->update(1.0);
    require(productQuantity(*factory, "flavored_voltz") == 1, "product should complete after restore");
}

} // namespace

int main() {
    restoresConfigLineAndCustomStationTopology();
    restoresAssignedTaskAndMachineProgress();
    return 0;
}
