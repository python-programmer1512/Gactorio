#include "model/ConfiguredStation.hpp"
#include "model/Machine.hpp"
#include "model/config/FactoryConfig.hpp"

#include <cassert>

int main() {
    gactorio::MixingStation mixer(1, "Mixer");
    assert(mixer.stationKind() == "mixing");
    assert(mixer.acceptsStep("mixing"));
    assert(!mixer.acceptsStep("packaging"));
    assert(!mixer.acceptsStep(""));
    assert(mixer.canProcess(gactorio::MachineRole::Mixing));
    assert(!mixer.canProcess(gactorio::MachineRole::Packaging));
    assert(!mixer.canProcess(gactorio::MachineRole::Unknown));

    gactorio::PackagingStation packager(2, "Packager");
    assert(packager.stationKind() == "packaging");
    assert(packager.acceptsStep("packaging"));
    assert(!packager.acceptsStep("mixing"));
    assert(packager.canProcess(gactorio::MachineRole::Packaging));
    assert(!packager.canProcess(gactorio::MachineRole::Mixing));

    gactorio::config_model::StationDefinition configuredDefinition;
    configuredDefinition.id = "custom_mixer";
    configuredDefinition.kind = "mixing";
    configuredDefinition.displayName = "Custom Mixer";
    configuredDefinition.typeName = "Custom Mixing Station";
    configuredDefinition.acceptedStepKinds = {"mixing", "pre_mix"};

    gactorio::ConfiguredStation configured(3, configuredDefinition);
    assert(configured.stationKind() == "mixing");
    assert(configured.acceptsStep("mixing"));
    assert(configured.acceptsStep("pre_mix"));
    assert(!configured.acceptsStep("packaging"));
    assert(configured.canProcess(gactorio::MachineRole::Mixing));
    assert(!configured.canProcess(gactorio::MachineRole::Packaging));

    gactorio::config_model::StationDefinition fallbackDefinition;
    fallbackDefinition.id = "quality_fallback";
    fallbackDefinition.kind = "quality";
    fallbackDefinition.displayName = "Quality Fallback";
    fallbackDefinition.typeName = "Quality Fallback Station";

    gactorio::ConfiguredStation fallback(4, fallbackDefinition);
    assert(fallback.stationKind() == "quality");
    assert(fallback.acceptsStep("quality"));
    assert(!fallback.acceptsStep("mixing"));
    assert(fallback.canProcess(gactorio::MachineRole::Quality));
    assert(!fallback.canProcess(gactorio::MachineRole::Mixing));

    return 0;
}
