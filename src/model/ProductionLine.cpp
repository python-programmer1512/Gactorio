#include "model/ProductionLine.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

namespace {

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

void ProductionLine::addMachine(std::unique_ptr<Machine> machine) {
    machine->setEventBus(eventBus_);
    machines_.push_back(std::move(machine));
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
