#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace {

class PilotCarbonator final : public gactorio::Machine {
public:
    PilotCarbonator(gactorio::MachineId id, std::string name)
        : Machine(id, std::move(name), 1.0, 100.0, 0.0) {}

    std::string typeName() const override {
        return "Pilot Carbonator";
    }

    gactorio::ProcessType processType() const override {
        return gactorio::ProcessType::Carbonation;
    }

    gactorio::MachineRole role() const override {
        return gactorio::MachineRole::Carbonator;
    }

    bool canAcceptRecipe(const gactorio::Recipe& recipe) const override {
        (void)recipe;
        return true;
    }
};

} // namespace

int main() {
    gactorio::EventBus bus;
    gactorio::EventLogObserver log;
    gactorio::StatisticsObserver stats;
    bus.subscribe(&log);
    bus.subscribe(&stats);

    gactorio::ProductionLine line(1, "Routing Line");
    line.setEventBus(&bus);
    line.addMachine(std::make_unique<PilotCarbonator>(1, "Pilot Carbonator"));
    line.addMachine(std::make_unique<gactorio::Filler>(2, "Filler"));
    line.addMachine(std::make_unique<gactorio::Sealer>(3, "Sealer"));
    line.addMachine(std::make_unique<gactorio::Labeler>(4, "Labeler"));

    line.machines().front()->pause();
    line.enqueueProduct(std::make_shared<gactorio::SodaCan>());

    assert(line.queueLength() == 1);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->getProductName() == "Soda Can");

    auto snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 1);
    assert(snapshot.currentTaskName() == "Soda Can");
    assert(snapshot.machines().size() == 4);

    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Paused);
    assert(line.currentTask()->currentStepIndex() == 0);

    line.machines().front()->resume();
    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines().back()->getStatus() == gactorio::MachineStatus::Idle);

    line.update(2.0);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->currentStepIndex() == 1);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Idle);

    line.update(0.0);
    assert(line.machines()[1]->getStatus() == gactorio::MachineStatus::Working);

    for (int i = 0; i < 10 && line.queueLength() > 0; ++i) {
        line.update(10.0);
    }
    assert(line.queueLength() == 0);
    assert(stats.startedTaskEvents() == 4);
    assert(stats.completedStepEvents() == 4);
    assert(stats.completedProductEvents() == 1);

    bool sawProductCompleted = false;
    for (const auto& event : log.events()) {
        if (event.type() == gactorio::EventType::ProductCompleted) {
            sawProductCompleted = true;
            assert(event.message().find("Soda Can") != std::string::npos);
        }
    }
    assert(sawProductCompleted);

    snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 0);
    assert(snapshot.currentTaskName().empty());

    gactorio::ProductionLine mementoLine(10, "Memento Line");
    mementoLine.setEventBus(&bus);
    mementoLine.addMachine(std::make_unique<gactorio::Carbonator>(10, "Memento Carbonator"));
    mementoLine.addMachine(std::make_unique<gactorio::Filler>(11, "Memento Filler"));
    mementoLine.enqueueProduct(std::make_shared<gactorio::SodaCan>());
    mementoLine.update(0.0);

    assert(mementoLine.queueLength() == 1);
    assert(mementoLine.currentTask() != nullptr);
    assert(mementoLine.machines().front()->hasTask());

    std::unordered_map<const gactorio::ProductionTask*, gactorio::TaskMementoId> taskIds;
    gactorio::TaskMementoId nextTaskId = 1;
    auto lineState = mementoLine.exportState(taskIds, nextTaskId);
    assert(lineState.id == 10);
    assert(lineState.name == "Memento Line");
    assert(lineState.taskQueue.size() == 1);
    assert(lineState.taskQueue.front().taskId == 1);
    assert(lineState.machines.size() == 2);
    assert(lineState.machines.front().assignedTaskId.has_value());
    assert(*lineState.machines.front().assignedTaskId == lineState.taskQueue.front().taskId);

    lineState.completedProducts.push_back(static_cast<gactorio::ProductId>(gactorio::ProductType::EnergyDrink));

    gactorio::EventBus restoredBus;
    gactorio::EventLogObserver restoredLog;
    restoredBus.subscribe(&restoredLog);

    gactorio::ProductionLine restoredLine(0, "Temporary Line");
    restoredLine.setEventBus(&restoredBus);
    std::unordered_map<gactorio::TaskMementoId, std::shared_ptr<gactorio::ProductionTask>> restoredTasks;
    restoredLine.restoreState(lineState, restoredTasks);

    assert(restoredLog.events().empty());
    assert(restoredLine.id() == 10);
    assert(restoredLine.name() == "Memento Line");
    assert(restoredLine.queueLength() == 1);
    assert(restoredTasks.count(1) == 1);
    assert(restoredLine.currentTask() == restoredTasks.at(1));
    assert(restoredLine.machines().front()->hasTask());
    assert(restoredLine.machines().front()->getStatus() == gactorio::MachineStatus::Working);

    const auto restoredCompleted = restoredLine.collectCompletedProducts();
    assert(restoredCompleted.size() == 1);
    assert(restoredCompleted.front() == static_cast<gactorio::ProductId>(gactorio::ProductType::EnergyDrink));
    assert(restoredLine.queueLength() == 1);

    restoredLine.update(2.0);
    assert(restoredLine.currentTask() == restoredTasks.at(1));
    assert(restoredLine.currentTask()->currentStepIndex() == 1);
    assert(restoredTasks.at(1)->currentStepIndex() == 1);
    assert(!restoredLog.events().empty());

    return 0;
}
