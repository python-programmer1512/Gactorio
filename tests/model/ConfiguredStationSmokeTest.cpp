#include "model/ConfiguredStation.hpp"
#include "model/StationFactory.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <filesystem>
#include <memory>
#include <stdexcept>

int main() {
    const auto context = gactorio::config_model::FactoryRuntimeContext::loadFromFile(
        std::filesystem::path("..") / "data" / "factory_config.runtime.json");

    const auto& mixingDefinition = context.registry().requireStation("mixing_station");
    auto machine = gactorio::StationFactory::create(42, mixingDefinition);
    assert(machine != nullptr);
    assert(machine->getId() == 42);
    assert(machine->getName() == "Mixer");
    assert(machine->typeName() == "Mixing Station");
    assert(machine->role() == gactorio::MachineRole::Mixing);
    assert(machine->processType() == gactorio::ProcessType::Mixing);
    assert(machine->canProcess(gactorio::MachineRole::Mixing));
    assert(!machine->canProcess(gactorio::MachineRole::Packaging));
    assert(machine->getProcessingSpeed() == 1.0);
    assert(machine->getHealth() == 100.0);
    assert(machine->getBreakdownProbability() == 0.0);
    assert(machine->canAcceptTask());

    const auto* configured = dynamic_cast<const gactorio::ConfiguredStation*>(machine.get());
    assert(configured != nullptr);
    assert(configured->stationDefinitionId() == "mixing_station");
    assert(configured->stationKind() == "mixing");
    assert(configured->acceptsStep("mixing"));
    assert(!configured->acceptsStep("packaging"));

    gactorio::config_model::StationDefinition fallbackDefinition;
    fallbackDefinition.id = "quality_without_explicit_steps";
    fallbackDefinition.kind = "quality";
    fallbackDefinition.displayName = "Quality Fallback";
    fallbackDefinition.typeName = "Quality Fallback Station";
    fallbackDefinition.processingSpeed = 1.25;
    fallbackDefinition.maxHealth = 72.0;
    fallbackDefinition.breakdownProbability = 0.04;

    auto fallbackMachine = gactorio::StationFactory::createConfiguredStation(43, fallbackDefinition);
    assert(fallbackMachine != nullptr);
    assert(fallbackMachine->getName() == "Quality Fallback");
    assert(fallbackMachine->typeName() == "Quality Fallback Station");
    assert(fallbackMachine->role() == gactorio::MachineRole::Quality);
    assert(fallbackMachine->processType() == gactorio::ProcessType::Quality);
    assert(fallbackMachine->getProcessingSpeed() == 1.25);
    assert(fallbackMachine->getHealth() == 72.0);
    assert(fallbackMachine->getBreakdownProbability() == 0.04);

    const auto* fallbackConfigured =
        dynamic_cast<const gactorio::ConfiguredStation*>(fallbackMachine.get());
    assert(fallbackConfigured != nullptr);
    assert(fallbackConfigured->acceptsStep("quality"));
    assert(!fallbackConfigured->acceptsStep("mixing"));

    gactorio::config_model::StationDefinition unsupportedDefinition;
    unsupportedDefinition.id = "flavor_station";
    unsupportedDefinition.kind = "flavoring";
    unsupportedDefinition.displayName = "Flavor Station";
    unsupportedDefinition.typeName = "Flavor Station";
    unsupportedDefinition.acceptedStepKinds = {"flavoring"};

    bool sawUnsupportedKind = false;
    try {
        (void)gactorio::StationFactory::create(44, unsupportedDefinition);
    } catch (const std::invalid_argument&) {
        sawUnsupportedKind = true;
    }
    assert(sawUnsupportedKind);

    return 0;
}
