#include "model/DefaultProducts.hpp"
#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/EventBus.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::ProductCatalog catalog;
    gactorio::registerDefaultProducts(catalog);
    auto product = catalog.createProduct(101);
    assert(product != nullptr);
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::Carbonator machine(1, "Carbonator State Test");

    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.stateName() == "Idle");

    assert(machine.assignTask(task));
    assert(machine.getStatus() == gactorio::MachineStatus::Working);
    assert(machine.stateName() == "Working");

    machine.forceBreak();
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.stateName() == "Broken");

    machine.update(100.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);

    machine.repair();
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);
    assert(machine.stateName() == "Maintenance");

    machine.update(1.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);

    machine.update(1.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.stateName() == "Idle");

    gactorio::EventBus bus;
    gactorio::EventLogObserver log;
    bus.subscribe(&log);
    machine.setEventBus(&bus);
    const auto eventsBeforeRestore = log.events().size();

    machine.restoreStateObject(gactorio::MachineStatus::Working);
    assert(machine.getStatus() == gactorio::MachineStatus::Working);
    assert(machine.stateName() == "Working");
    assert(log.events().size() == eventsBeforeRestore);

    machine.restoreStateObject(gactorio::MachineStatus::Paused);
    assert(machine.getStatus() == gactorio::MachineStatus::Paused);
    assert(machine.stateName() == "Paused");
    assert(log.events().size() == eventsBeforeRestore);

    machine.restoreStateObject(gactorio::MachineStatus::Broken);
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.stateName() == "Broken");
    assert(log.events().size() == eventsBeforeRestore);

    machine.restoreStateObject(gactorio::MachineStatus::Maintenance);
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);
    assert(machine.stateName() == "Maintenance");
    assert(log.events().size() == eventsBeforeRestore);

    machine.restoreStateObject(gactorio::MachineStatus::Idle);
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.stateName() == "Idle");
    assert(log.events().size() == eventsBeforeRestore);

    return 0;
}
