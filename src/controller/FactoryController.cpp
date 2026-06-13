#include "controller/FactoryController.hpp"

#include "common/ScenarioType.hpp"
#include "model/CarbonationFactory.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/Machine.hpp"
#include "model/config/ConfigIdAdapters.hpp"
#include "model/config/FactoryConfigLoader.hpp"

#include <algorithm>
#include <memory>
#include <string>

namespace gactorio {

namespace {

std::string fallbackDisplayName(const std::string& id) {
    return id.empty() ? std::string("Unknown") : id;
}

InventorySnapshot makeInventorySnapshot(
    const Inventory& inventory,
    const config_model::DefinitionRegistry* registry) {
    InventorySnapshot snapshot;
    for (const auto& item : inventory.items()) {
        if (registry != nullptr) {
            if (const auto* definition = registry->findItem(item.first)) {
                snapshot.addItem(
                    item.first,
                    item.second,
                    definition->displayName.empty() ? item.first : definition->displayName,
                    definition->category,
                    definition->restockable,
                    definition->restockAmount);
                continue;
            }
        }
        snapshot.addItem(item.first, item.second, fallbackDisplayName(item.first), "item", false, 0);
    }
    for (const auto& product : inventory.products()) {
        if (registry != nullptr) {
            if (const auto* definition = registry->findProduct(product.first)) {
                snapshot.addItem(
                    product.first,
                    product.second,
                    definition->displayName.empty() ? product.first : definition->displayName,
                    "product",
                    false,
                    0);
                continue;
            }
        }
        if (const auto* definition = findProductDefinition(product.first)) {
            snapshot.addItem(product.first, product.second, definition->name, definition->tier, false, 0);
            continue;
        }
        snapshot.addItem(product.first, product.second, fallbackDisplayName(product.first), "product", false, 0);
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

    ProductionLineSnapshot snapshot(
        line.id(),
        line.name(),
        line.queueLength(),
        currentName,
        currentProgress,
        scenarioTypeToString(line.scenario()),
        scenarioTypeToDisplayName(line.scenario()),
        line.queueCapacityValueOrZero(),
        line.droppedTaskCount());
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

FactoryController::FactoryController(std::unique_ptr<Factory> factory) {
    replaceFactory(std::move(factory));
}

FactoryController::FactoryController(
    std::unique_ptr<Factory> factory,
    std::unique_ptr<config_model::FactoryRuntimeContext> runtimeContext) {
    replaceFactory(std::move(factory), std::move(runtimeContext));
}

FactoryController FactoryController::createFromConfigFile(const std::filesystem::path& path) {
    auto runtimeContext = std::make_unique<config_model::FactoryRuntimeContext>(
        config_model::FactoryConfigLoader::loadFromFile(path));
    auto factory = FactoryBuilder::createFactory(*runtimeContext);
    return FactoryController(std::move(factory), std::move(runtimeContext));
}

FactoryController FactoryController::createFromConfigString(std::string_view jsonText) {
    auto runtimeContext = std::make_unique<config_model::FactoryRuntimeContext>(
        config_model::FactoryConfigLoader::loadFromString(jsonText));
    auto factory = FactoryBuilder::createFactory(*runtimeContext);
    return FactoryController(std::move(factory), std::move(runtimeContext));
}

void FactoryController::createDefaultCarbonationFactory() {
    replaceFactory(std::make_unique<CarbonationFactory>(), nullptr);
}

void FactoryController::replaceFactory(std::unique_ptr<Factory> factory) {
    replaceFactory(std::move(factory), nullptr);
}

void FactoryController::replaceFactory(
    std::unique_ptr<Factory> factory,
    std::unique_ptr<config_model::FactoryRuntimeContext> runtimeContext) {
    factory_ = std::move(factory);
    runtimeContext_ = std::move(runtimeContext);
    history_.clear();
}

void FactoryController::reset() {
    if (runtimeContext_) {
        factory_ = FactoryBuilder::createFactory(*runtimeContext_);
        history_.clear();
        return;
    }
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
    if (productType == ProductType::Unknown) {
        return FactoryCommandResult::InvalidRequest;
    }
    return enqueueProductById(lineId, config_model::toProductId(productType));
}

FactoryCommandResult FactoryController::enqueueProductById(LineId lineId, ProductId productId) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    auto* line = factory_->findProductionLine(lineId);
    if (line == nullptr) {
        return FactoryCommandResult::NotFound;
    }

    auto product = factory_->createProductForQueue(productId);
    if (product == nullptr) {
        return FactoryCommandResult::InvalidRequest;
    }

    switch (factory_->enqueueProduct(line->id(), std::move(product))) {
    case EnqueueResult::Accepted:
        return FactoryCommandResult::Success;
    case EnqueueResult::LostOverflow:
        return FactoryCommandResult::OverflowDropped;
    case EnqueueResult::RejectedFull:
    default:
        return FactoryCommandResult::InvalidRequest;
    }
}

LineId FactoryController::enqueueAuto(ProductType productType) {
    if (productType == ProductType::Unknown) {
        return 0;
    }
    return enqueueAutoById(config_model::toProductId(productType));
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
    auto* carbonationFactory = dynamic_cast<CarbonationFactory*>(factory_.get());
    if (carbonationFactory == nullptr) {
        return 0;
    }
    return carbonationFactory->addDynamicLine();
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

FactoryCommandResult FactoryController::restockItemById(const std::string& itemId) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }

    if (const auto* currentRegistry = registry()) {
        const auto* item = currentRegistry->findItem(itemId);
        if (item == nullptr || !item->restockable || item->restockAmount <= 0) {
            return FactoryCommandResult::InvalidRequest;
        }
        factory_->inventory().addItem(itemId, item->restockAmount);
        return FactoryCommandResult::Success;
    }

    const auto itemType = config_model::itemTypeFromId(itemId);
    if (!itemType.has_value()) {
        return FactoryCommandResult::InvalidRequest;
    }
    return restockItem(*itemType, 5);
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

FactoryCommandResult FactoryController::setLineScenario(LineId lineId, ScenarioType scenario) {
    if (!factory_) {
        return FactoryCommandResult::InvalidRequest;
    }
    return factory_->setLineScenario(lineId, scenario)
        ? FactoryCommandResult::Success
        : FactoryCommandResult::NotFound;
}

FactoryCommandResult FactoryController::setLineScenarioById(LineId lineId, const std::string& scenarioId) {
    const auto scenario = scenarioTypeFromString(scenarioId);
    if (!scenario.has_value()) {
        return FactoryCommandResult::InvalidRequest;
    }
    return setLineScenario(lineId, *scenario);
}

std::optional<ScenarioType> FactoryController::getLineScenario(LineId lineId) const {
    if (!factory_) {
        return std::nullopt;
    }
    return factory_->getLineScenario(lineId);
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
        makeInventorySnapshot(factory_->inventory(), registry()),
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

std::vector<ProductDefinition> FactoryController::availableProductDefinitions() const {
    if (const auto* currentRegistry = registry()) {
        return productDefinitionsFromRegistry(*currentRegistry);
    }
    return productDefinitions();
}

bool FactoryController::hasRuntimeContext() const noexcept {
    return runtimeContext_ != nullptr;
}

const config_model::FactoryRuntimeContext* FactoryController::runtimeContext() const noexcept {
    return runtimeContext_.get();
}

const config_model::FactoryConfig* FactoryController::config() const noexcept {
    return runtimeContext_ == nullptr ? nullptr : &runtimeContext_->config();
}

const config_model::DefinitionRegistry* FactoryController::registry() const noexcept {
    return runtimeContext_ == nullptr ? nullptr : &runtimeContext_->registry();
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
