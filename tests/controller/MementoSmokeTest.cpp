#include "controller/FactoryController.hpp"

#include <cassert>

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
    return 0;
}
