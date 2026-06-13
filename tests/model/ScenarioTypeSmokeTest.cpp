#include "common/ScenarioType.hpp"
#include "model/Machine.hpp"

#include <cassert>
#include <optional>

int main() {
    assert(gactorio::scenarioTypeToString(gactorio::ScenarioType::NormalFlow) == "normal-flow");
    assert(gactorio::scenarioTypeToString(gactorio::ScenarioType::RandomBreakdowns) == "random-breakdowns");
    assert(gactorio::scenarioTypeToDisplayName(gactorio::ScenarioType::Overflow) == "Overflow");

    const auto bottleneck = gactorio::scenarioTypeFromString("bottleneck");
    assert(bottleneck.has_value());
    assert(*bottleneck == gactorio::ScenarioType::Bottleneck);
    assert(!gactorio::scenarioTypeFromString("rush-order").has_value());

    gactorio::MixingStation machine(1, "Mixer", 2.0, 100.0, 0.02);
    assert(machine.getProcessingSpeed() == 2.0);
    assert(machine.effectiveProcessingSpeed() == 2.0);
    assert(machine.getBreakdownProbability() == 0.02);
    assert(machine.effectiveBreakdownProbability() == 0.02);

    machine.setScenarioSpeedMultiplier(0.5);
    assert(machine.getProcessingSpeed() == 2.0);
    assert(machine.effectiveProcessingSpeed() == 1.0);

    machine.setScenarioSpeedMultiplier(-4.0);
    assert(machine.getProcessingSpeed() == 2.0);
    assert(machine.effectiveProcessingSpeed() == 0.0);

    machine.setScenarioBreakdownProbabilityOverride(0.06);
    assert(machine.getBreakdownProbability() == 0.02);
    assert(machine.effectiveBreakdownProbability() == 0.06);

    machine.setScenarioBreakdownProbabilityOverride(2.0);
    assert(machine.effectiveBreakdownProbability() == 1.0);

    machine.resetScenarioModifiers();
    assert(machine.effectiveProcessingSpeed() == machine.getProcessingSpeed());
    assert(machine.effectiveBreakdownProbability() == machine.getBreakdownProbability());

    return 0;
}
