#include "model/ProductionLine.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

namespace {

std::unique_ptr<Machine> createMachineFromState(const MachineMemento& state) {
    switch (state.type) {
    case MachineTypeKind::Carbonator:
        return std::make_unique<Carbonator>(
            state.id,
            state.name,
            state.processingSpeed,
            state.health,
            state.breakdownProbability);
    case MachineTypeKind::Filler:
        return std::make_unique<Filler>(
            state.id,
            state.name,
            state.processingSpeed,
            state.health,
            state.breakdownProbability);
    case MachineTypeKind::Conveyor:
        return std::make_unique<Conveyor>(
            state.id,
            state.name,
            state.processingSpeed,
            state.health,
            state.breakdownProbability);
    case MachineTypeKind::Sealer:
        return std::make_unique<Sealer>(
            state.id,
            state.name,
            state.processingSpeed,
            state.health,
            state.breakdownProbability);
    case MachineTypeKind::Labeler:
        return std::make_unique<Labeler>(
            state.id,
            state.name,
            state.processingSpeed,
            state.health,
            state.breakdownProbability);
    default:
        return nullptr;
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

const std::vector<std::unique_ptr<Machine>>& ProductionLine::machines() const {
    return machines_;
}

void ProductionLine::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
    for (auto& machine : machines_) {
        machine->setEventBus(eventBus_);
    }
}

void ProductionLine::enqueueProduct(std::shared_ptr<Product> product) {
    if (product == nullptr) {
        return;
    }

    auto task = std::make_shared<ProductionTask>(std::move(product));
    if (eventBus_ != nullptr) {
        eventBus_->publish(Event(0.0, EventType::TaskEnqueued, 0, name_ + " enqueued " + task->getProductName()));
    }
    taskQueue_.push_back(std::move(task));
}

std::size_t ProductionLine::queueLength() const {
    return taskQueue_.size();
}

std::shared_ptr<ProductionTask> ProductionLine::currentTask() const {
    if (taskQueue_.empty()) {
        return nullptr;
    }
    return taskQueue_.front();
}

ProductionLineSnapshot ProductionLine::getSnapshot() const {
    std::string currentName;
    double currentProgress = 0.0;
    if (!taskQueue_.empty()) {
        currentName = taskQueue_.front()->getProductName();
        currentProgress = taskQueue_.front()->getProgressInRoute();
    }

    ProductionLineSnapshot snapshot(id_, name_, taskQueue_.size(), currentName, currentProgress);
    for (const auto& machine : machines_) {
        snapshot.addMachine(machine->getSnapshot());
    }
    return snapshot;
}

void ProductionLine::addMachine(std::unique_ptr<Machine> machine) {
    machine->setEventBus(eventBus_);
    machines_.push_back(std::move(machine));
}

void ProductionLine::assignAvailableTask() {
    if (taskQueue_.empty()) {
        return;
    }

    const auto task = taskQueue_.front();
    const auto* step = task->currentStep();
    if (step == nullptr) {
        return;
    }

    const auto isTaskAlreadyAssigned = std::any_of(
        machines_.begin(),
        machines_.end(),
        [](const std::unique_ptr<Machine>& machine) {
            return machine->hasTask();
        });

    if (isTaskAlreadyAssigned) {
        return;
    }

    for (auto& machine : machines_) {
        if (machine->canAcceptTask() && machine->canProcess(step->requiredRole())) {
            machine->assignTask(task);
            return;
        }
    }
}

std::vector<ProductId> ProductionLine::collectCompletedProducts() {
    while (!taskQueue_.empty() && taskQueue_.front()->isCompleted()) {
        completedProducts_.push_back(taskQueue_.front()->getProductId());
        taskQueue_.pop_front();
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

ProductionLineMemento ProductionLine::exportState(
    std::unordered_map<const ProductionTask*, TaskMementoId>& taskIds,
    TaskMementoId& nextTaskId) const {
    ProductionLineMemento state;
    state.id = id_;
    state.name = name_;
    state.completedProducts = completedProducts_;

    for (const auto& task : taskQueue_) {
        if (task == nullptr) {
            continue;
        }

        auto found = taskIds.find(task.get());
        if (found == taskIds.end()) {
            found = taskIds.emplace(task.get(), nextTaskId++).first;
        }
        state.taskQueue.push_back(task->exportState(found->second));
    }

    state.machines.reserve(machines_.size());
    for (const auto& machine : machines_) {
        state.machines.push_back(machine->exportState(taskIds));
    }

    return state;
}

void ProductionLine::restoreState(
    const ProductionLineMemento& state,
    std::unordered_map<TaskMementoId, std::shared_ptr<ProductionTask>>& restoredTasks) {
    id_ = state.id;
    name_ = state.name;
    taskQueue_.clear();
    completedProducts_ = state.completedProducts;
    machines_.clear();

    for (const auto& taskState : state.taskQueue) {
        auto task = ProductionTask::fromState(taskState);
        if (task == nullptr) {
            continue;
        }
        restoredTasks[taskState.taskId] = task;
        taskQueue_.push_back(std::move(task));
    }

    for (const auto& machineState : state.machines) {
        auto machine = createMachineFromState(machineState);
        if (machine == nullptr) {
            continue;
        }
        machine->restoreState(machineState, restoredTasks);
        machine->setEventBus(eventBus_);
        machines_.push_back(std::move(machine));
    }
}

void ProductionLine::update(double deltaTime) {
    assignAvailableTask();
    for (auto& machine : machines_) {
        machine->update(deltaTime);
    }
    (void)collectCompletedProducts();
}

} // namespace gactorio
