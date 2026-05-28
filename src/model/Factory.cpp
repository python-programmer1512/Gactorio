#include "model/Factory.hpp"

#include <algorithm>
#include <unordered_map>
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

const ProductCatalog& Factory::productCatalog() const {
    return productCatalog_;
}

ProductCatalog& Factory::productCatalog() {
    return productCatalog_;
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

ProductionRequestResult Factory::enqueueProduct(LineId lineId, std::unique_ptr<Product> product) {
    return enqueueProduct(lineId, std::shared_ptr<Product>(std::move(product)));
}

ProductionRequestResult Factory::enqueueProduct(LineId lineId, std::shared_ptr<Product> product) {
    auto* line = findProductionLine(lineId);
    if (line == nullptr) {
        return ProductionRequestResult::LineNotFound;
    }

    if (product == nullptr) {
        return ProductionRequestResult::InvalidRequest;
    }

    const auto productName = product->getName();
    if (!inventory_.consume(product->getRequirements())) {
        eventBus_.publish(Event(
            simulationTime(),
            EventType::Info,
            lineId,
            "Insufficient inputs for " + productName));
        return ProductionRequestResult::InsufficientMaterials;
    }

    eventBus_.publish(Event(
        simulationTime(),
        EventType::InputsConsumed,
        lineId,
        "Consumed inputs for " + productName));

    line->enqueueProduct(std::move(product));
    return ProductionRequestResult::Success;
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

FactoryMemento Factory::createMemento() const {
    FactoryMemento state;
    state.clock = clock_.exportState();
    state.inventory = inventory_.exportState();
    state.recipes = exportRecipeStates();

    std::unordered_map<const ProductionTask*, TaskMementoId> taskIds;
    TaskMementoId nextTaskId = 1;
    state.productionLines.reserve(productionLines_.size());
    for (const auto& line : productionLines_) {
        state.productionLines.push_back(line.exportState(taskIds, nextTaskId));
    }

    state.eventLog = eventLog_.exportState();
    state.statistics = statistics_.exportState();
    return state;
}

void Factory::restoreFromMemento(const FactoryMemento& state) {
    productionLines_.clear();
    machines_.clear();

    clock_.restoreState(state.clock);
    inventory_.restoreState(state.inventory);
    restoreRecipeStates(state.recipes);

    std::unordered_map<TaskMementoId, std::shared_ptr<ProductionTask>> restoredTasks;
    productionLines_.reserve(state.productionLines.size());
    for (const auto& lineState : state.productionLines) {
        ProductionLine line(lineState.id, lineState.name);
        line.restoreState(lineState, restoredTasks, productCatalog_);
        productionLines_.push_back(std::move(line));
    }

    rebuildMachineRegistry();
    reconnectEventBus();
    eventLog_.restoreState(state.eventLog);
    statistics_.restoreState(state.statistics);
}

EventLog& Factory::mutableEventLog() {
    return eventLog_;
}

Statistics& Factory::mutableStatistics() {
    return statistics_;
}

std::vector<RecipeMemento> Factory::exportRecipeStates() const {
    return {};
}

void Factory::restoreRecipeStates(const std::vector<RecipeMemento>& recipes) {
    (void)recipes;
}

void Factory::rebuildMachineRegistry() {
    machines_.clear();
    for (auto& line : productionLines_) {
        for (const auto& machine : line.machines()) {
            machines_.push_back(machine.get());
        }
    }
}

void Factory::reconnectEventBus() {
    eventBus_ = EventBus{};
    eventBus_.subscribe(&eventLog_);
    eventBus_.subscribe(&statistics_);
    for (auto& line : productionLines_) {
        line.setEventBus(&eventBus_);
    }
}

} // namespace gactorio
