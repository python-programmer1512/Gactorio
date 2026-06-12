#include "controller/FactoryController.hpp"

#include "model/Machine.hpp"
#include "model/ProductCatalog.hpp"

#include <algorithm>
#include <memory>
#include <string>

namespace gactorio {

namespace {

InventorySnapshot makeInventorySnapshot(const Inventory& inventory) {
    InventorySnapshot snapshot;
    for (const auto& item : inventory.items()) {
        snapshot.addItem(std::to_string(static_cast<std::uint64_t>(item.first)), item.second);
    }
    for (const auto& product : inventory.products()) {
        snapshot.addItem(std::to_string(product.first), product.second);
    }
    return snapshot;
}

StatisticsSnapshot makeStatisticsSnapshot(const Statistics& statistics) {
    return StatisticsSnapshot(
        statistics.completedProductEvents(),
        statistics.startedTaskEvents(),
        statistics.completedStepEvents(),
        statistics.brokenMachineEvents(),
        statistics.repairedMachineEvents(),
        statistics.stateChangedEvents());
}

MachineSnapshot makeMachineSnapshot(const Machine& machine) {
    return MachineSnapshot(
        machine.getId(),
        machine.getName(),
        machine.typeName(),
        machine.getStatus(),
        machine.stateName(),
        machine.getProgress(),
        machine.getHealth());
}

ProductionLineSnapshot makeProductionLineSnapshot(const ProductionLine& line) {
    std::string currentName;
    double currentProgress = 0.0;
    if (const auto task = line.currentTask()) {
        currentName = task->getProductName();
        currentProgress = task->getProgressInRoute();
    }

    ProductionLineSnapshot snapshot(line.id(), line.name(), line.queueLength(), currentName, currentProgress);
    for (const auto& machine : line.machines()) {
        if (machine->hasTask()) {
            currentProgress = std::max(currentProgress, machine->getProgress());
        }
        snapshot.addMachine(makeMachineSnapshot(*machine));
    }
    snapshot.setCurrentTaskProgress(currentProgress);
    return snapshot;
}

} // namespace

FactoryController::FactoryController() {
    createDefaultCarbonationFactory();
}

void FactoryController::createDefaultCarbonationFactory() {
    factory_ = std::make_unique<CarbonationFactory>();
    history_.clear();
}

void FactoryController::reset() {
    createDefaultCarbonationFactory();
}

void FactoryController::tick(double deltaTime) {
    if (factory_) {
        factory_->update(deltaTime);
    }
}

void FactoryController::startSimulation() {
    resumeSimulation();
}

void FactoryController::pauseSimulation() {
    if (factory_) {
        factory_->pauseClock();
    }
}

void FactoryController::resetSimulation() {
    reset();
}

void FactoryController::resumeSimulation() {
    if (factory_) {
        factory_->resumeClock();
    }
}

void FactoryController::resetSimulationClock() {
    if (factory_) {
        factory_->resetClock();
    }
}

void FactoryController::stopSimulation() {
    if (factory_) {
        factory_->stopClock();
    }
}

void FactoryController::setSpeed(double speedMultiplier) {
    setSimulationSpeed(speedMultiplier);
}

void FactoryController::setSimulationSpeed(double speedMultiplier) {
    if (factory_) {
        factory_->setClockSpeed(speedMultiplier);
    }
}

FactoryCommandResult FactoryController::enqueueProduct(LineId lineId, ProductType productType) {
    return enqueueProductById(lineId, static_cast<ProductId>(productType));
}

FactoryCommandResult FactoryController::enqueueProductById(LineId lineId, ProductId productId) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* line = factory_->findProductionLine(lineId);
    if (line == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    auto product = createProduct(productId);
    if (product == nullptr) {
        return FactoryCommandResult::InvalidRequest;
    }
    return factory_->enqueueProduct(line->id(), std::move(product))
        ? FactoryCommandResult::Success
        : FactoryCommandResult::InvalidRequest;
}

LineId FactoryController::enqueueAuto(ProductType productType) {
    return enqueueAutoById(static_cast<ProductId>(productType));
}

LineId FactoryController::enqueueAutoById(ProductId productId) {
    if (!factory_) return 0;
    const auto& lines = factory_->productionLines();
    if (lines.empty()) return 0;

    // Pick the line with the smallest queue. Ties broken by order (first wins).
    const ProductionLine* best = nullptr;
    std::size_t bestQ = 0;
    for (const auto& l : lines) {
        if (best == nullptr || l.queueLength() < bestQ) {
            best  = &l;
            bestQ = l.queueLength();
        }
    }
    if (best == nullptr) return 0;
    if (enqueueProductById(best->id(), productId) == FactoryCommandResult::Success) {
        return best->id();
    }
    return 0;
}

LineId FactoryController::addLine() {
    if (!factory_) return 0;
    return factory_->addDynamicLine();
}

FactoryCommandResult FactoryController::removeLine(LineId id) {
    if (!factory_) return FactoryCommandResult::InvalidRequest;
    return factory_->removeProductionLine(id)
        ? FactoryCommandResult::Success
        : FactoryCommandResult::InvalidRequest;
}

FactoryCommandResult FactoryController::forceBreak(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->forceBreak();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::repairMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->repair();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::incrementalRepairMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->incrementalRepair();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::restockItem(ItemType itemType, int amount) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    return factory_->restockItem(itemType, amount)
        ? FactoryCommandResult::Success
        : FactoryCommandResult::InvalidRequest;
}

FactoryCommandResult FactoryController::pauseMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->pause();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::resumeMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    machine->resume();
    return FactoryCommandResult::Success;
}

FactorySnapshot FactoryController::getFactorySnapshot() const {
    return snapshot();
}

std::vector<EventSnapshot> FactoryController::getEventLogs() const {
    std::vector<EventSnapshot> logs;
    if (!factory_) {
        return logs;
    }

    for (const auto& event : factory_->eventLog().events()) {
        logs.emplace_back(event.simulationTime(), event.type(), event.message());
    }
    return logs;
}

StatisticsSnapshot FactoryController::getStatistics() const {
    if (!factory_) {
        return StatisticsSnapshot(0);
    }
    return makeStatisticsSnapshot(factory_->statistics());
}

FactorySnapshot FactoryController::snapshot() const {
    if (!factory_) {
        return FactorySnapshot(0.0, InventorySnapshot(), StatisticsSnapshot(0));
    }

    FactorySnapshot snapshot(
        factory_->simulationTime(),
        makeInventorySnapshot(factory_->inventory()),
        makeStatisticsSnapshot(factory_->statistics()));

    for (const auto& line : factory_->productionLines()) {
        auto lineSnapshot = makeProductionLineSnapshot(line);
        snapshot.addProductionLine(std::move(lineSnapshot));
    }

    for (const auto& event : factory_->eventLog().events()) {
        snapshot.addEvent(EventSnapshot(event.simulationTime(), event.type(), event.message()));
    }

    return snapshot;
}

// =============================================================================
// Memento façade — Caretaker access from the public Controller surface.
// =============================================================================
void FactoryController::saveCheckpoint() {
    if (!factory_) return;
    history_.push(factory_->createMemento());
}

bool FactoryController::undo() {
    if (!factory_) return false;
    auto m = history_.pop();
    if (!m.has_value()) return false;
    factory_->restoreFromMemento(*m);
    return true;
}

bool FactoryController::canUndo() const {
    return history_.canUndo();
}

std::size_t FactoryController::historySize() const {
    return history_.size();
}

} // namespace gactorio
