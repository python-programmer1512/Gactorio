#include "controller/FactoryController.hpp"
#include "model/CarbonationFactory.hpp"

#include <cassert>
#include <cmath>

namespace {

bool near(double lhs, double rhs) {
    return std::fabs(lhs - rhs) < 0.000001;
}

const gactorio::ProductionLineSnapshot* lineById(
    const gactorio::FactorySnapshot& snapshot,
    gactorio::LineId id) {
    for (const auto& line : snapshot.productionLines()) {
        if (line.id() == id) {
            return &line;
        }
    }
    return nullptr;
}

} // namespace

int main() {
    gactorio::FactoryController controller;

    // Drive a little so something happens, then take a checkpoint.
    controller.tick(0.5);
    const auto snapBefore = controller.snapshot();
    const auto invBefore  = snapBefore.inventory().items();

    assert(!controller.canUndo());
    controller.saveCheckpoint();
    assert(controller.canUndo());
    assert(controller.historySize() == 1);

    // Enqueue a couple of products. These mutate the inventory deductions
    // (or at least the queue length).
    controller.enqueueProduct(1, gactorio::ProductType::HyperBolt);
    controller.enqueueProduct(1, gactorio::ProductType::AuroraZero);

    // Run sim forward — possibly completes some steps.
    for (int i = 0; i < 5; ++i) controller.tick(1.0);
    const auto simAfter = controller.snapshot().simulationTime();

    // Undo restores the prior state.
    assert(controller.undo());
    assert(!controller.canUndo());          // stack drained
    assert(controller.historySize() == 0);

    const auto snapRestored = controller.snapshot();
    assert(snapRestored.simulationTime() < simAfter); // jumped back in time

    // Inventory should match (we restored the captured items map).
    const auto invRestored = snapRestored.inventory().items();
    assert(invRestored.size() == invBefore.size());

    // Empty undo on a drained history returns false.
    assert(!controller.undo());

    // Topology also belongs to the memento: a line created after the
    // checkpoint disappears after undo, and the next generated id is restored.
    gactorio::FactoryController topologyController;
    topologyController.saveCheckpoint();
    assert(topologyController.addLine() == 2);
    assert(topologyController.snapshot().productionLines().size() == 2);
    assert(topologyController.undo());
    assert(topologyController.snapshot().productionLines().size() == 1);
    assert(topologyController.addLine() == 2);

    gactorio::FactoryController removedLineController;
    assert(removedLineController.addLine() == 2);
    removedLineController.saveCheckpoint();
    assert(removedLineController.removeLine(2) == gactorio::FactoryCommandResult::Success);
    assert(removedLineController.snapshot().productionLines().size() == 1);
    assert(removedLineController.undo());
    assert(removedLineController.snapshot().productionLines().size() == 2);
    assert(removedLineController.addLine() == 3);

    gactorio::FactoryController scenarioController;
    assert(scenarioController.addLine() == 2);
    assert(scenarioController.setLineScenario(1, gactorio::ScenarioType::Bottleneck)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.setLineScenario(2, gactorio::ScenarioType::RandomBreakdowns)
           == gactorio::FactoryCommandResult::Success);
    scenarioController.saveCheckpoint();

    assert(scenarioController.setLineScenario(1, gactorio::ScenarioType::NormalFlow)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.setLineScenario(2, gactorio::ScenarioType::Overflow)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.restockItem(gactorio::ItemType::Ingredient, 10)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.restockItem(gactorio::ItemType::Water, 10)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.restockItem(gactorio::ItemType::EmptyBottle, 10)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.restockItem(gactorio::ItemType::Label, 10)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.restockItem(gactorio::ItemType::Package, 10)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.enqueueProduct(2, gactorio::ProductType::VoltzClassic)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.enqueueProduct(2, gactorio::ProductType::HyperBolt)
           == gactorio::FactoryCommandResult::Success);
    assert(scenarioController.enqueueProduct(2, gactorio::ProductType::AuroraZero)
           == gactorio::FactoryCommandResult::OverflowDropped);

    auto changed = scenarioController.snapshot();
    assert(lineById(changed, 2)->droppedTaskCount() == 1);
    assert(scenarioController.undo());

    auto restoredScenarioSnapshot = scenarioController.snapshot();
    const auto* restoredLine1 = lineById(restoredScenarioSnapshot, 1);
    const auto* restoredLine2 = lineById(restoredScenarioSnapshot, 2);
    assert(restoredLine1 != nullptr);
    assert(restoredLine2 != nullptr);
    assert(restoredLine1->scenarioId() == "bottleneck");
    assert(restoredLine1->scenarioName() == "Bottleneck");
    assert(restoredLine2->scenarioId() == "random-breakdowns");
    assert(restoredLine2->scenarioName() == "Random Breakdowns");
    assert(restoredLine1->queueCapacity() == 0);
    assert(restoredLine2->queueCapacity() == 0);
    assert(restoredLine2->droppedTaskCount() == 0);

    const auto& restoredLineModels = scenarioController.snapshot().productionLines();
    (void)restoredLineModels;

    // Controller snapshots prove the public view state; use a concrete factory
    // to also verify the restored scenarios re-apply machine modifiers.
    gactorio::CarbonationFactory factory;
    assert(factory.addDynamicLine() == 2);
    assert(factory.setLineScenario(1, gactorio::ScenarioType::Bottleneck));
    assert(factory.setLineScenario(2, gactorio::ScenarioType::RandomBreakdowns));
    auto memento = factory.createMemento();
    assert(factory.setLineScenario(1, gactorio::ScenarioType::NormalFlow));
    assert(factory.setLineScenario(2, gactorio::ScenarioType::Overflow));
    factory.restoreFromMemento(memento);

    const auto* modelLine1 = factory.findProductionLine(1);
    const auto* modelLine2 = factory.findProductionLine(2);
    assert(modelLine1 != nullptr);
    assert(modelLine2 != nullptr);
    assert(modelLine1->scenario() == gactorio::ScenarioType::Bottleneck);
    assert(modelLine2->scenario() == gactorio::ScenarioType::RandomBreakdowns);
    for (const auto& machine : modelLine1->machines()) {
        if (machine->role() == gactorio::MachineRole::Bottling) {
            assert(near(machine->effectiveProcessingSpeed(), machine->getProcessingSpeed() * 0.5));
        }
    }
    for (const auto& machine : modelLine2->machines()) {
        assert(near(machine->effectiveBreakdownProbability(), 0.06));
    }
    factory.update(0.1);

    // Reset creates a fresh factory, so stale checkpoints must not remain.
    topologyController.saveCheckpoint();
    assert(topologyController.canUndo());
    topologyController.resetSimulation();
    assert(!topologyController.canUndo());

    return 0;
}
