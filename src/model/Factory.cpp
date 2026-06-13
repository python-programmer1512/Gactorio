#include "model/Factory.hpp"

#include "common/ScenarioType.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

namespace gactorio {
namespace {

void rollbackRequirements(Inventory& inventory, const std::vector<ItemRequirement>& requirements) {
    for (const auto& requirement : requirements) {
        inventory.addItem(requirement.itemId(), requirement.quantity());
    }
}

void applyStepOutputs(Inventory& inventory, const std::vector<StepOutput>& outputs) {
    for (const auto& output : outputs) {
        if (output.quantity <= 0) {
            continue;
        }
        if (output.isItem()) {
            inventory.addItem(*output.itemId, output.quantity);
        } else if (output.isProduct()) {
            inventory.addProduct(*output.productId, output.quantity);
        }
    }
}

} // namespace

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

void Factory::rebuildMachineCache() {
    machines_.clear();
    for (auto& line : productionLines_) {
        line.setEventBus(&eventBus_);
        for (const auto& machine : line.machines()) {
            machines_.push_back(machine.get());
        }
    }
}

bool Factory::removeProductionLine(LineId id) {
    if (productionLines_.size() <= 1) {
        return false;
    }

    auto it = std::find_if(productionLines_.begin(), productionLines_.end(),
        [id](const ProductionLine& l) { return l.id() == id; });
    if (it == productionLines_.end()) return false;

    // Removal is only allowed when nothing is in flight on the line.
    if (it->queueLength() > 0) return false;
    for (const auto& m : it->machines()) {
        if (m->hasTask()) return false;
        if (m->getStatus() == MachineStatus::Working ||
            m->getStatus() == MachineStatus::Maintenance) return false;
    }

    // Drop the cached Machine* pointers belonging to this line.
    for (const auto& m : it->machines()) {
        auto mit = std::find(machines_.begin(), machines_.end(), m.get());
        if (mit != machines_.end()) machines_.erase(mit);
    }
    productionLines_.erase(it);
    return true;
}

std::shared_ptr<Product> Factory::createProductForQueue(ProductId id) const {
    return createProductById(std::move(id));
}

EnqueueResult Factory::enqueueProduct(LineId lineId, std::shared_ptr<Product> product) {
    if (product == nullptr) {
        return EnqueueResult::RejectedFull;
    }

    auto* line = findProductionLine(lineId);
    if (line == nullptr) {
        return EnqueueResult::RejectedFull;
    }

    const bool consumeAtEnqueue = !product->usesStepLevelIO();
    if (consumeAtEnqueue && !inventory_.consume(product->getRequirements())) {
        return EnqueueResult::RejectedFull;
    }

    const auto requirements = consumeAtEnqueue
        ? product->getRequirements()
        : std::vector<ItemRequirement>{};
    const auto result = line->enqueueProduct(std::move(product));
    if (consumeAtEnqueue && result != EnqueueResult::Accepted) {
        rollbackRequirements(inventory_, requirements);
    }
    return result;
}

bool Factory::restockItem(ItemType itemType, int amount) {
    if (amount <= 0) {
        return false;
    }

    switch (itemType) {
    case ItemType::Ingredient:
    case ItemType::Water:
    case ItemType::EmptyBottle:
    case ItemType::Label:
    case ItemType::Package:
        break;
    case ItemType::Unknown:
    default:
        return false;
    }

    inventory_.addItem(itemType, amount);
    return true;
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

bool Factory::setLineScenario(LineId lineId, ScenarioType scenario) {
    auto* line = findProductionLine(lineId);
    if (line == nullptr) {
        return false;
    }

    line->setScenario(scenario);
    eventBus_.publish(Event(
        clock_.now(),
        EventType::Info,
        lineId,
        "Line " + std::to_string(lineId) + " scenario changed to " + scenarioTypeToDisplayName(scenario)));
    return true;
}

std::optional<ScenarioType> Factory::getLineScenario(LineId lineId) const {
    const auto* line = findProductionLine(lineId);
    if (line == nullptr) {
        return std::nullopt;
    }
    return line->scenario();
}

SimulationTime Factory::update(double realDeltaTime) {
    const auto deltaTime = clock_.update(realDeltaTime);

    for (auto& line : productionLines_) {
        line.assignAvailableTask(&inventory_);
    }

    for (auto* machine : machines_) {
        machine->update(deltaTime);
    }

    for (auto& line : productionLines_) {
        applyStepOutputs(inventory_, line.collectPendingStepOutputs());
        for (const auto productId : line.collectCompletedProducts()) {
            inventory_.addProduct(productId, 1);
        }
    }

    for (auto& line : productionLines_) {
        line.assignAvailableTask(&inventory_);
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

std::optional<ProductionLine> Factory::createLineForMemento(const LineMemento&) const {
    // Base Factory does not know which concrete stations belong to a line.
    // Concrete factories can override this hook to rebuild missing topology.
    return std::nullopt;
}

// =============================================================================
// Memento — Originator implementation
// =============================================================================
FactoryMemento Factory::createMemento() const {
    FactoryMemento m(clock_.now(), inventory_.items(), inventory_.products());

    for (const auto& line : productionLines_) {
        std::vector<MachineMemento> machineMementos;
        for (const auto& machine : line.machines()) {
            machineMementos.emplace_back(
                machine->id(),
                machine->getHealth(),
                machine->getStatus(),
                machine->rawProgressForMemento(),
                machine->stationDefinitionId(),
                machine->stationKind(),
                machine->getName(),
                machine->typeName(),
                machine->getProcessingSpeed(),
                machine->getBreakdownProbability(),
                line.taskIndexFor(machine->currentTask()));
        }
        m.addLine(LineMemento(
            line.id(),
            line.pendingProductIds(),
            std::move(machineMementos),
            line.scenario(),
            line.queueCapacity(),
            line.droppedTaskCount(),
            line.pendingTaskMementos(),
            line.definitionId(),
            line.name()));
    }
    return m;
}

void Factory::restoreFromMemento(const FactoryMemento& m) {
    // Clock — jump straight to the captured simulation time.
    clock_.setNow(m.simulationTime());

    // Inventory — overwrite both raw items and finished products.
    inventory_.replaceContents(m.items(), m.products());

    // Keep the current topology aligned with the checkpoint: remove lines
    // created after the checkpoint and ask subclasses to recreate missing ones.
    const auto& savedLines = m.lines();
    productionLines_.erase(
        std::remove_if(
            productionLines_.begin(),
            productionLines_.end(),
            [&savedLines](const ProductionLine& line) {
                return std::none_of(
                    savedLines.begin(),
                    savedLines.end(),
                    [&line](const LineMemento& lm) {
                        return lm.id() == line.id();
                    });
            }),
        productionLines_.end());
    rebuildMachineCache();

    for (const auto& lm : savedLines) {
        if (findProductionLine(lm.id()) == nullptr) {
            auto restoredLine = createLineForMemento(lm);
            if (restoredLine.has_value()) {
                addProductionLine(std::move(*restoredLine));
            }
        }
    }

    // Each line: reset machines (drop in-flight work), clear the queue, and
    // restore queued task snapshots in order. A task that had already consumed
    // step-level inputs keeps that flag so restore does not consume them twice.
    for (const auto& lm : savedLines) {
        auto* line = findProductionLine(lm.id());
        if (line == nullptr) continue;

        line->clearQueue();
        line->clearCompleted();
        line->setScenario(lm.scenario());
        if (lm.queueCapacity().has_value()) {
            line->setQueueCapacity(*lm.queueCapacity());
        } else {
            line->resetQueueCapacity();
        }

        std::vector<std::shared_ptr<ProductionTask>> restoredTasks;
        restoredTasks.reserve(lm.tasks().size());
        for (const auto& taskSnap : lm.tasks()) {
            auto product = createProductById(taskSnap.productId());
            if (product != nullptr) {
                auto task = std::make_shared<ProductionTask>(std::move(product));
                task->restoreFromMemento(taskSnap);
                restoredTasks.push_back(std::move(task));
                (void)line->enqueueTask(restoredTasks.back());
            }
        }

        for (const auto& machineSnap : lm.machines()) {
            auto* machine = line->findMachine(machineSnap.id());
            if (machine == nullptr) {
                continue;
            }
            std::shared_ptr<ProductionTask> assignedTask;
            if (machineSnap.assignedTaskIndex().has_value() &&
                *machineSnap.assignedTaskIndex() < restoredTasks.size()) {
                assignedTask = restoredTasks[*machineSnap.assignedTaskIndex()];
            }
            machine->restoreForMemento(
                machineSnap.health(),
                machineSnap.status(),
                machineSnap.progress(),
                std::move(assignedTask));
        }

        line->setDroppedTaskCount(lm.droppedTaskCount());
    }
}

} // namespace gactorio
