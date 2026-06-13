#include "model/ProductionLine.hpp"

#include <algorithm>
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

} // namespace

ProductionLine::ProductionLine(ProductionLineId id, std::string name)
    : id_(id), name_(std::move(name)) {}

ProductionLineId ProductionLine::id() const {
    return id_;
}

const std::string& ProductionLine::name() const {
    return name_;
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

            const auto* step = task->currentStep();
            if (step != nullptr && machine->canProcess(step->requiredRole())) {
                machine->assignTask(task);
                break;
            }
        }
    }
}

std::vector<ProductId> ProductionLine::collectCompletedProducts() {
    for (auto it = taskQueue_.begin(); it != taskQueue_.end();) {
        if (*it != nullptr && (*it)->isCompleted()) {
            completedProducts_.push_back((*it)->getProductId());
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

void ProductionLine::clearQueue() {
    taskQueue_.clear();
}

void ProductionLine::clearCompleted() {
    completedProducts_.clear();
}

} // namespace gactorio
