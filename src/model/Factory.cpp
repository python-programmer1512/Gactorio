#include "model/Factory.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

Factory::Factory() {
    eventBus_.subscribe(&eventLog_);
    eventBus_.subscribe(&statistics_);
}

SimulationTime Factory::simulationTime() const {
    return clock_.now();
}

const Inventory& Factory::inventory() const {
    return inventory_;
}

Inventory& Factory::inventory() {
    return inventory_;
}

const std::vector<ProductionLine>& Factory::productionLines() const {
    return productionLines_;
}

const std::vector<Machine*>& Factory::machines() const {
    return machines_;
}

const EventLog& Factory::eventLog() const {
    return eventLog_;
}

const Statistics& Factory::statistics() const {
    return statistics_;
}

EventBus& Factory::eventBus() {
    return eventBus_;
}

const EventBus& Factory::eventBus() const {
    return eventBus_;
}

const SimClock& Factory::clock() const {
    return clock_;
}

void Factory::addProductionLine(ProductionLine line) {
    line.setEventBus(&eventBus_);
    for (const auto& machine : line.machines()) {
        machines_.push_back(machine.get());
    }
    productionLines_.push_back(std::move(line));
}

ProductionLine* Factory::findProductionLine(LineId id) {
    for (auto& line : productionLines_) {
        if (line.id() == id) {
            return &line;
        }
    }
    return nullptr;
}

const ProductionLine* Factory::findProductionLine(LineId id) const {
    for (const auto& line : productionLines_) {
        if (line.id() == id) {
            return &line;
        }
    }
    return nullptr;
}

Machine* Factory::findMachine(MachineId id) {
    for (auto& line : productionLines_) {
        auto* machine = line.findMachine(id);
        if (machine != nullptr) {
            return machine;
        }
    }
    return nullptr;
}

SimulationTime Factory::update(double realDeltaTime) {
    const auto deltaTime = clock_.update(realDeltaTime);

    for (auto& line : productionLines_) {
        line.assignAvailableTask();
    }

    for (auto* machine : machines_) {
        machine->update(deltaTime);
    }

    for (auto& line : productionLines_) {
        for (const auto productId : line.collectCompletedProducts()) {
            inventory_.addProduct(productId, 1);
        }
    }

    return deltaTime;
}

void Factory::pauseClock() {
    clock_.pause();
}

void Factory::resumeClock() {
    clock_.resume();
}

void Factory::resetClock() {
    clock_.reset();
}

void Factory::stopClock() {
    clock_.stop();
}

void Factory::setClockSpeed(double speedMultiplier) {
    clock_.setSpeed(speedMultiplier);
}

EventLog& Factory::mutableEventLog() {
    return eventLog_;
}

Statistics& Factory::mutableStatistics() {
    return statistics_;
}

std::shared_ptr<Product> Factory::createProductById(ProductId) const {
    // Base Factory has no catalog. Subclasses (CarbonationFactory) override
    // this to build the right concrete Product subclass from an ID.
    return nullptr;
}

// =============================================================================
// Memento — Originator implementation
// =============================================================================
FactoryMemento Factory::createMemento() const {
    FactoryMemento m;
    m.simulationTime = clock_.now();
    m.items          = inventory_.items();
    m.products       = inventory_.products();

    for (const auto& line : productionLines_) {
        LineMemento lm;
        lm.id              = line.id();
        lm.queueProductIds = line.pendingProductIds();
        for (const auto& machine : line.machines()) {
            lm.machines.push_back({
                machine->id(), machine->getHealth(), machine->getStatus()
            });
        }
        m.lines.push_back(std::move(lm));
    }
    return m;
}

void Factory::restoreFromMemento(const FactoryMemento& m) {
    // Clock — jump straight to the captured simulation time.
    clock_.setNow(m.simulationTime);

    // Inventory — overwrite both raw items and finished products.
    inventory_.replaceContents(m.items, m.products);

    // Each line: reset machines (drop in-flight tasks, restore HP/status),
    // clear the queue, re-enqueue saved pending products in order.
    for (const auto& lm : m.lines) {
        auto* line = findProductionLine(lm.id);
        if (line == nullptr) continue;

        line->clearQueue();
        line->clearCompleted();

        for (const auto& machineSnap : lm.machines) {
            auto* machine = line->findMachine(machineSnap.id);
            if (machine != nullptr) {
                machine->resetForRestore(machineSnap.health, machineSnap.status);
            }
        }

        for (const auto productId : lm.queueProductIds) {
            auto product = createProductById(productId);
            if (product != nullptr) {
                line->enqueueProduct(std::move(product));
            }
        }
    }
}

} // namespace gactorio
