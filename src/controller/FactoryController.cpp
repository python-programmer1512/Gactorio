#include "controller/FactoryController.hpp"

#include "model/Machine.hpp"
#include "model/Product.hpp"

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

std::unique_ptr<Product> makeProduct(ProductType productType) {
    switch (productType) {
    case ProductType::VoltzClassic:
        return std::make_unique<VoltzClassic>();
    case ProductType::HyperBolt:
        return std::make_unique<HyperBolt>();
    case ProductType::AuroraZero:
        return std::make_unique<AuroraZero>();
    case ProductType::Unknown:
    default:
        return nullptr;
    }
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

} // namespace

FactoryController::FactoryController() {
    createDefaultCarbonationFactory();
}

void FactoryController::createDefaultCarbonationFactory() {
    factory_ = std::make_unique<CarbonationFactory>();
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
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* line = factory_->findProductionLine(lineId);
    if (line == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    auto product = makeProduct(productType);
    if (product == nullptr) {
        return FactoryCommandResult::InvalidRequest;
    }

    line->enqueueProduct(std::shared_ptr<Product>(std::move(product)));
    return FactoryCommandResult::Success;
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
        auto lineSnapshot = line.getSnapshot();
        snapshot.addProductionLine(std::move(lineSnapshot));
    }

    for (const auto& event : factory_->eventLog().events()) {
        snapshot.addEvent(EventSnapshot(event.simulationTime(), event.type(), event.message()));
    }

    return snapshot;
}

} // namespace gactorio
