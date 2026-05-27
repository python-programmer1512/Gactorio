#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"

#include <cassert>
#include <memory>
#include <string>
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

    return 0;
}
