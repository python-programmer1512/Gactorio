#include "controller/FactoryController.hpp"

#include "model/Item.hpp"
#include "model/Machine.hpp"
#include "model/Product.hpp"

#include <memory>
#include <string>

namespace gactorio {

namespace {

InventorySnapshot makeInventorySnapshot(const Inventory& inventory, const ProductCatalog& productCatalog) {
    InventorySnapshot snapshot;
    for (const auto& item : inventory.items()) {
        snapshot.addItem(
            std::to_string(static_cast<std::uint64_t>(item.first)),
            ItemTypeName::get(item.first),
            item.second);
    }
    for (const auto& product : inventory.products()) {
        snapshot.addItem(std::to_string(product.first), productCatalog.displayName(product.first), product.second);
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

FactoryCommandResult toCommandResult(ProductionRequestResult result) {
    switch (result) {
    case ProductionRequestResult::Success:
        return FactoryCommandResult::Success;
    case ProductionRequestResult::LineNotFound:
        return FactoryCommandResult::NotFound;
    case ProductionRequestResult::InsufficientMaterials:
        return FactoryCommandResult::InsufficientMaterials;
    case ProductionRequestResult::InvalidRequest:
    default:
        return FactoryCommandResult::InvalidRequest;
    }
}

} // namespace

FactoryController::FactoryController() {
    createDefaultCarbonationFactory();
}

void FactoryController::recordCurrentState() {
    if (factory_ && !suppressAutoHistory_) {
        history_.record(factory_->createMemento());
    }
}

void FactoryController::createDefaultCarbonationFactory() {
    factory_ = std::make_unique<CarbonationFactory>();
}

void FactoryController::reset() {
    recordCurrentState();
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
        recordCurrentState();
        factory_->pauseClock();
    }
}

void FactoryController::resetSimulation() {
    reset();
}

void FactoryController::resumeSimulation() {
    if (factory_) {
        recordCurrentState();
        factory_->resumeClock();
    }
}

void FactoryController::resetSimulationClock() {
    if (factory_) {
        recordCurrentState();
        factory_->resetClock();
    }
}

void FactoryController::stopSimulation() {
    if (factory_) {
        recordCurrentState();
        factory_->stopClock();
    }
}

void FactoryController::setSpeed(double speedMultiplier) {
    setSimulationSpeed(speedMultiplier);
}

void FactoryController::setSimulationSpeed(double speedMultiplier) {
    if (factory_) {
        recordCurrentState();
        factory_->setClockSpeed(speedMultiplier);
    }
}

FactoryCommandResult FactoryController::enqueueProduct(LineId lineId, ProductId productId) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto product = factory_->productCatalog().createProduct(productId);
    if (product == nullptr) {
        return FactoryCommandResult::UnknownProduct;
    }

    const auto previous = factory_->createMemento();
    const auto result = toCommandResult(factory_->enqueueProduct(lineId, std::move(product)));
    if (result == FactoryCommandResult::Success) {
        history_.record(previous);
    }
    return result;
}

FactoryCommandResult FactoryController::forceBreak(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    recordCurrentState();
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

    recordCurrentState();
    machine->repair();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::pauseMachine(MachineId id) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* machine = factory_->findMachine(id);
    if (machine == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    recordCurrentState();
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

    recordCurrentState();
    machine->resume();
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::saveState() {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    history_.record(factory_->createMemento());
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::undo() {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto previous = history_.undo(factory_->createMemento());
    if (!previous.has_value()) {
        return FactoryCommandResult::InvalidRequest;
    }

    suppressAutoHistory_ = true;
    factory_->restoreFromMemento(*previous);
    suppressAutoHistory_ = false;
    return FactoryCommandResult::Success;
}

FactoryCommandResult FactoryController::redo() {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto next = history_.redo(factory_->createMemento());
    if (!next.has_value()) {
        return FactoryCommandResult::InvalidRequest;
    }

    suppressAutoHistory_ = true;
    factory_->restoreFromMemento(*next);
    suppressAutoHistory_ = false;
    return FactoryCommandResult::Success;
}

bool FactoryController::canUndo() const {
    return history_.canUndo();
}

bool FactoryController::canRedo() const {
    return history_.canRedo();
}

void FactoryController::clearHistory() {
    history_.clear();
}

SimulationHistoryStatus FactoryController::getHistoryStatus() const {
    return SimulationHistoryStatus(canUndo(), canRedo());
}

const ProductCatalog& FactoryController::productCatalog() const {
    return factory_->productCatalog();
}

ProductCatalog& FactoryController::productCatalog() {
    return factory_->productCatalog();
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
        logs.emplace_back(event.simulationTime(), event.type(), event.sourceId(), event.message());
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
        makeInventorySnapshot(factory_->inventory(), factory_->productCatalog()),
        makeStatisticsSnapshot(factory_->statistics()));

    for (const auto& line : factory_->productionLines()) {
        auto lineSnapshot = line.getSnapshot();
        snapshot.addProductionLine(std::move(lineSnapshot));
    }

    for (const auto& event : factory_->eventLog().events()) {
        snapshot.addEvent(EventSnapshot(event.simulationTime(), event.type(), event.sourceId(), event.message()));
    }

    return snapshot;
}

} // namespace gactorio
