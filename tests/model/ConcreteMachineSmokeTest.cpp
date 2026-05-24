#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>
#include <vector>

int main() {
    gactorio::MixingStation    mixer    (1, "Mixer 1",    1.5, 88.0, 0.07);
    gactorio::QualityStation   quality  (2, "Quality 1");
    gactorio::BottlingStation  bottling (3, "Bottling 1");
    gactorio::PackagingStation packaging(4, "Packaging 1");

    // Each station only accepts the role it's designed for.
    assert(mixer.canProcess    (gactorio::MachineRole::Mixing));
    assert(quality.canProcess  (gactorio::MachineRole::Quality));
    assert(bottling.canProcess (gactorio::MachineRole::Bottling));
    assert(packaging.canProcess(gactorio::MachineRole::Packaging));

    // Custom-constructed mixer respects the explicit values.
    assert(mixer.getProcessingSpeed() == 1.5);
    assert(mixer.getHealth()          == 88.0);
    assert(mixer.getBreakdownProbability() == 0.07);

    // Polymorphic update: heterogeneous machines through the base pointer.
    std::vector<std::unique_ptr<gactorio::Machine>> machines;
    machines.push_back(std::make_unique<gactorio::MixingStation>   (10, "Poly Mixer"));
    machines.push_back(std::make_unique<gactorio::QualityStation>  (11, "Poly Quality"));
    machines.push_back(std::make_unique<gactorio::BottlingStation> (12, "Poly Bottling"));
    machines.push_back(std::make_unique<gactorio::PackagingStation>(13, "Poly Packaging"));

    for (auto& machine : machines) {
        machine->update(0.25);
        assert(machine->getHealth() > 0.0);
    }

    // VoltzClassic's first step is Mixing — only mixer can accept the task.
    const gactorio::VoltzClassic voltz;
    auto task = std::make_shared<gactorio::ProductionTask>(voltz);

    assert(!packaging.assignTask(task));
    assert( mixer.assignTask(task));

    return 0;
}
