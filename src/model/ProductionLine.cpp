#include "model/ProductionLine.hpp"

#include "model/Inventory.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>

namespace gactorio {

namespace {

LineScenarioConfig lineScenarioConfigFor(ScenarioType scenario) {
    switch (scenario) {
    case ScenarioType::NormalFlow:
        return {scenario, std::nullopt, 1.0, std::nullopt, std::nullopt};
    case ScenarioType::RandomBreakdowns:
        return {scenario, std::nullopt, 1.0, 0.06, std::nullopt};
    case ScenarioType::Bottleneck:
        return {scenario, MachineRole::Bottling, 0.5, std::nullopt, std::nullopt};
    case ScenarioType::Overflow:
        return {scenario, std::nullopt, 1.0, std::nullopt, 2};
    }
    return {ScenarioType::NormalFlow, std::nullopt, 1.0, std::nullopt, std::nullopt};
}

bool isAssignedToMachine(
    const std::vector<std::unique_ptr<Machine>>& machines,
    const ProductionTask* task) {
    return std::any_of(
        machines.begin(),
        machines.end(),
        [task](const std::unique_ptr<Machine>& machine) {
            return machine->currentTask() == task;
        });
}

void rollbackInputs(Inventory& inventory, const std::vector<ItemRequirement>& inputs) {
    for (const auto& input : inputs) {
        inventory.addItem(input.itemId(), input.quantity());
    }
}

} // namespace

ProductionLine::ProductionLine(ProductionLineId id, std::string name)
    : id_(id), name_(std::move(name)) {}

ProductionLineId ProductionLine::id() const {
    return id_;
}

const std::string& ProductionLine::name() const {
    return name_;
}

const std::string& ProductionLine::definitionId() const noexcept {
    return definitionId_;
}

const std::vector<std::unique_ptr<Machine>>& ProductionLine::machines() const {
    return machines_;
}

ScenarioType ProductionLine::scenario() const {
    return scenario_;
}

ScenarioType ProductionLine::getScenario() const {
    return scenario();
}

void ProductionLine::setScenario(ScenarioType scenario) {
    scenario_ = scenario;

    for (auto& machine : machines_) {
        machine->resetScenarioModifiers();
    }
    resetQueueCapacity();

    const auto config = lineScenarioConfigFor(scenario_);
    if (config.breakdownProbabilityOverride.has_value()) {
        for (auto& machine : machines_) {
            machine->setScenarioBreakdownProbabilityOverride(config.breakdownProbabilityOverride);
        }
    }

    if (config.bottleneckRole.has_value()) {
        for (auto& machine : machines_) {
            if (machine->role() == *config.bottleneckRole) {
                machine->setScenarioSpeedMultiplier(config.bottleneckSpeedMultiplier);
            }
        }
    }

    if (config.queueCapacity.has_value()) {
        setQueueCapacity(*config.queueCapacity);
    }
}

void ProductionLine::setDefinitionId(std::string definitionId) {
    definitionId_ = std::move(definitionId);
}

void ProductionLine::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
    for (auto& machine : machines_) {
        machine->setEventBus(eventBus_);
    }
}

EnqueueResult ProductionLine::enqueueProduct(std::shared_ptr<Product> product) {
    if (product == nullptr) {
        return EnqueueResult::RejectedFull;
    }

    if (queueCapacity_.has_value() && taskQueue_.size() >= *queueCapacity_) {
        ++droppedTaskCount_;
        if (eventBus_ != nullptr) {
            eventBus_->publish(Event(
                0.0,
                EventType::Info,
                id_,
                "Line " + std::to_string(id_) + " overflow: product task dropped because queue capacity was exceeded"));
        }
        return scenario_ == ScenarioType::Overflow
            ? EnqueueResult::LostOverflow
            : EnqueueResult::RejectedFull;
    }

    auto task = std::make_shared<ProductionTask>(std::move(product));
    if (eventBus_ != nullptr) {
        eventBus_->publish(Event(0.0, EventType::TaskEnqueued, 0, name_ + " enqueued " + task->getProductName()));
    }
    taskQueue_.push_back(std::move(task));
    return EnqueueResult::Accepted;
}

std::size_t ProductionLine::queueLength() const {
    return taskQueue_.size();
}

std::optional<std::size_t> ProductionLine::queueCapacity() const {
    return queueCapacity_;
}

std::size_t ProductionLine::queueCapacityValueOrZero() const {
    return queueCapacity_.value_or(0);
}

void ProductionLine::setQueueCapacity(std::size_t capacity) {
    queueCapacity_ = capacity;
}

void ProductionLine::resetQueueCapacity() {
    queueCapacity_.reset();
}

std::size_t ProductionLine::droppedTaskCount() const {
    return droppedTaskCount_;
}

void ProductionLine::setDroppedTaskCount(std::size_t count) {
    droppedTaskCount_ = count;
}

std::shared_ptr<ProductionTask> ProductionLine::currentTask() const {
    if (taskQueue_.empty()) {
        return nullptr;
    }
    return taskQueue_.front();
}

void ProductionLine::addMachine(std::unique_ptr<Machine> machine) {
    machine->setEventBus(eventBus_);
    machines_.push_back(std::move(machine));
    setScenario(scenario_);
}

void ProductionLine::assignAvailableTask() {
    assignAvailableTask(nullptr);
}

void ProductionLine::assignAvailableTask(Inventory* inventory) {
    if (taskQueue_.empty()) {
        return;
    }

    for (auto& machine : machines_) {
        if (!machine->canAcceptTask()) {
            continue;
        }

        for (const auto& task : taskQueue_) {
            if (task == nullptr || task->isCompleted()) {
                continue;
            }
            if (isAssignedToMachine(machines_, task.get())) {
                continue;
            }

            const auto& stepKind = task->currentStepKind();
            if (stepKind.empty() || !machine->acceptsStep(stepKind)) {
                continue;
            }

            bool consumedInputs = false;
            if (task->usesStepLevelIO() && !task->currentStepInputsConsumed()) {
                const auto& inputs = task->currentStepInputs();
                if (inventory == nullptr) {
                    if (!inputs.empty()) {
                        continue;
                    }
                } else if (!inventory->consume(inputs)) {
                    continue;
                }
                consumedInputs = inventory != nullptr && !inputs.empty();
            }

            if (machine->assignTask(task)) {
                if (task->usesStepLevelIO()) {
                    task->markCurrentStepInputsConsumed();
                }
                break;
            }

            if (consumedInputs && inventory != nullptr) {
                rollbackInputs(*inventory, task->currentStepInputs());
            }
        }
    }
}

std::vector<StepOutput> ProductionLine::collectPendingStepOutputs() {
    std::vector<StepOutput> outputs;
    for (const auto& task : taskQueue_) {
        if (task == nullptr) {
            continue;
        }
        auto taskOutputs = task->collectPendingStepOutputs();
        outputs.insert(
            outputs.end(),
            std::make_move_iterator(taskOutputs.begin()),
            std::make_move_iterator(taskOutputs.end()));
    }
    return outputs;
}

std::vector<ProductId> ProductionLine::collectCompletedProducts() {
    for (auto it = taskQueue_.begin(); it != taskQueue_.end();) {
        if (*it != nullptr && (*it)->isCompleted()) {
            if (!(*it)->usesStepLevelIO() || !(*it)->hasStepProductOutput()) {
                completedProducts_.push_back((*it)->getProductId());
            }
            it = taskQueue_.erase(it);
        } else {
            ++it;
        }
    }

    auto completed = std::move(completedProducts_);
    completedProducts_.clear();
    return completed;
}

Machine* ProductionLine::findMachine(MachineId id) {
    for (auto& machine : machines_) {
        if (machine->getId() == id) {
            return machine.get();
        }
    }
    return nullptr;
}

const Machine* ProductionLine::findMachine(MachineId id) const {
    for (const auto& machine : machines_) {
        if (machine->getId() == id) {
            return machine.get();
        }
    }
    return nullptr;
}

void ProductionLine::update(double deltaTime) {
    assignAvailableTask();
    for (auto& machine : machines_) {
        machine->update(deltaTime);
    }
    (void)collectCompletedProducts();
    assignAvailableTask();
}

// ---- Memento support --------------------------------------------------------
std::vector<ProductId> ProductionLine::pendingProductIds() const {
    std::vector<ProductId> out;
    out.reserve(taskQueue_.size());
    for (const auto& task : taskQueue_) {
        if (task != nullptr) out.push_back(task->getProductId());
    }
    return out;
}

std::vector<ProductionTaskMemento> ProductionLine::pendingTaskMementos() const {
    std::vector<ProductionTaskMemento> out;
    out.reserve(taskQueue_.size());
    for (const auto& task : taskQueue_) {
        if (task != nullptr) {
            out.push_back(task->createMemento());
        }
    }
    return out;
}

std::optional<std::size_t> ProductionLine::taskIndexFor(const ProductionTask* task) const {
    if (task == nullptr) {
        return std::nullopt;
    }

    for (std::size_t i = 0; i < taskQueue_.size(); ++i) {
        if (taskQueue_[i].get() == task) {
            return i;
        }
    }
    return std::nullopt;
}

EnqueueResult ProductionLine::enqueueTask(std::shared_ptr<ProductionTask> task) {
    if (task == nullptr) {
        return EnqueueResult::RejectedFull;
    }

    taskQueue_.push_back(std::move(task));
    return EnqueueResult::Accepted;
}

void ProductionLine::clearQueue() {
    taskQueue_.clear();
}

void ProductionLine::clearCompleted() {
    completedProducts_.clear();
}

} // namespace gactorio
