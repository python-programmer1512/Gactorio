#include "model/ProductionLine.hpp"

#include <cassert>
#include <cmath>
#include <memory>

namespace {

bool near(double lhs, double rhs) {
    return std::fabs(lhs - rhs) < 0.000001;
}

void addBeverageMachines(gactorio::ProductionLine& line, gactorio::MachineId baseId = 1) {
    line.addMachine(std::make_unique<gactorio::MixingStation>   (baseId + 0, "Mixer",    1.0, 150.0, 0.0));
    line.addMachine(std::make_unique<gactorio::QualityStation>  (baseId + 1, "Quality",  1.0, 100.0, 0.0));
    line.addMachine(std::make_unique<gactorio::BottlingStation> (baseId + 2, "Bottling", 1.0, 120.0, 0.0));
    line.addMachine(std::make_unique<gactorio::PackagingStation>(baseId + 3, "Packaging",1.0, 140.0, 0.0));
}

} // namespace

int main() {
    gactorio::ProductionLine line(1, "Beverage Line A");
    addBeverageMachines(line);

    line.enqueueProduct(std::make_shared<gactorio::VoltzClassic>());
    line.enqueueProduct(std::make_shared<gactorio::HyperBolt>());

    assert(line.queueLength() == 2);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->getProductName() == "Voltz Classic");

    assert(line.queueLength() == 2);
    assert(line.currentTask()->getProductName() == "Voltz Classic");
    assert(line.machines().size() == 4);

    // First update routes the task to the Mixing station.
    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines().back()->getStatus()  == gactorio::MachineStatus::Idle);

    // When the first drink leaves Mixing, Mixing immediately starts the next
    // queued drink while Quality starts the first one.
    line.update(13.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines()[1]->getStatus() == gactorio::MachineStatus::Working);

    // Hyper Bolt's Mixing stage comes from data/factory_config.json: 18 sec.
    line.update(17.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);
    line.update(1.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Idle ||
           line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);

    // Drive the remaining pipelined work to completion.
    for (int i = 0; i < 30 && line.queueLength() > 0; ++i) {
        line.update(5.0);
    }
    assert(line.queueLength() == 0);

    assert(line.queueLength() == 0);
    assert(line.currentTask() == nullptr);

    gactorio::ProductionLine scenarioLine(2, "Scenario Line");
    addBeverageMachines(scenarioLine, 10);
    gactorio::ProductionLine otherLine(3, "Other Line");
    addBeverageMachines(otherLine, 20);

    assert(scenarioLine.scenario() == gactorio::ScenarioType::NormalFlow);
    assert(scenarioLine.getScenario() == gactorio::ScenarioType::NormalFlow);
    assert(!scenarioLine.queueCapacity().has_value());
    assert(scenarioLine.queueCapacityValueOrZero() == 0);
    assert(scenarioLine.droppedTaskCount() == 0);

    scenarioLine.setScenario(gactorio::ScenarioType::RandomBreakdowns);
    assert(scenarioLine.scenario() == gactorio::ScenarioType::RandomBreakdowns);
    for (const auto& machine : scenarioLine.machines()) {
        assert(near(machine->effectiveBreakdownProbability(), 0.06));
        assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed()));
    }
    for (const auto& machine : otherLine.machines()) {
        assert(near(machine->effectiveBreakdownProbability(), machine->getBreakdownProbability()));
    }

    scenarioLine.setScenario(gactorio::ScenarioType::Bottleneck);
    for (const auto& machine : scenarioLine.machines()) {
        if (machine->role() == gactorio::MachineRole::Bottling) {
            assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed() * 0.5));
        } else {
            assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed()));
        }
        assert(near(machine->effectiveBreakdownProbability(), machine->getBreakdownProbability()));
    }

    scenarioLine.setScenario(gactorio::ScenarioType::NormalFlow);
    for (const auto& machine : scenarioLine.machines()) {
        assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed()));
        assert(near(machine->effectiveBreakdownProbability(), machine->getBreakdownProbability()));
    }

    scenarioLine.setScenario(gactorio::ScenarioType::Bottleneck);
    scenarioLine.setScenario(gactorio::ScenarioType::NormalFlow);
    scenarioLine.setScenario(gactorio::ScenarioType::Bottleneck);
    for (const auto& machine : scenarioLine.machines()) {
        if (machine->role() == gactorio::MachineRole::Bottling) {
            assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed() * 0.5));
        }
    }

    scenarioLine.setScenario(gactorio::ScenarioType::Overflow);
    assert(scenarioLine.queueCapacity().has_value());
    assert(*scenarioLine.queueCapacity() == 2);
    assert(scenarioLine.queueCapacityValueOrZero() == 2);
    scenarioLine.enqueueProduct(std::make_shared<gactorio::VoltzClassic>());
    scenarioLine.enqueueProduct(std::make_shared<gactorio::HyperBolt>());
    scenarioLine.enqueueProduct(std::make_shared<gactorio::AuroraZero>());
    assert(scenarioLine.queueLength() == 2);
    assert(scenarioLine.droppedTaskCount() == 1);

    return 0;
}
