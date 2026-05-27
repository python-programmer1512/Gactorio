#include "model/Machine.hpp"

#include "model/MachineStates.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace gactorio {

namespace {

RecipeMemento exportRecipeState(const Recipe& recipe) {
    RecipeMemento state;
    state.id = recipe.id();
    state.name = recipe.name();
    state.durationSeconds = recipe.durationSeconds();
    state.inputs = recipe.inputs();
    state.outputs = recipe.outputs();
    return state;
}

Recipe makeRecipe(const RecipeMemento& state) {
    Recipe recipe(state.id, state.name, state.durationSeconds);
    for (const auto& input : state.inputs) {
        recipe.addInput(input.first, input.second);
    }
    for (const auto& output : state.outputs) {
        recipe.addOutput(output.first, output.second);
    }
    return recipe;
}

MachineTypeKind machineTypeKindOf(const Machine& machine) {
    if (dynamic_cast<const Carbonator*>(&machine) != nullptr) {
        return MachineTypeKind::Carbonator;
    }
    if (dynamic_cast<const Filler*>(&machine) != nullptr) {
        return MachineTypeKind::Filler;
    }
    if (dynamic_cast<const Conveyor*>(&machine) != nullptr) {
        return MachineTypeKind::Conveyor;
    }
    if (dynamic_cast<const Sealer*>(&machine) != nullptr) {
        return MachineTypeKind::Sealer;
    }
    if (dynamic_cast<const Labeler*>(&machine) != nullptr) {
        return MachineTypeKind::Labeler;
    }
    throw std::logic_error("unknown machine type for memento export");
}

} // namespace

Machine::Machine(MachineId id, std::string name)
    : Machine(id, std::move(name), 1.0, 100.0, 0.0) {}

Machine::Machine(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : id_(id),
      name_(std::move(name)),
      state_(std::make_unique<IdleState>()),
      health_(std::clamp(initialHealth, 0.0, 100.0)),
      processingSpeed_(std::max(0.0, processingSpeed)),
      breakdownProbability_(std::clamp(breakdownProbability, 0.0, 1.0)) {
    if (health_ <= 0.0) {
        status_ = MachineStatus::Broken;
        state_ = std::make_unique<BrokenState>();
    }
}

Machine::~Machine() = default;

MachineId Machine::id() const {
    return id_;
}

const std::string& Machine::name() const {
    return name_;
}

double Machine::progress() const {
    return getProgress();
}

std::string Machine::stateName() const {
    return state_ ? state_->name() : "Unknown";
}

const std::optional<Recipe>& Machine::recipe() const {
    return recipe_;
}

bool Machine::hasTask() const {
    return task_ != nullptr && !task_->isCompleted();
}

bool Machine::assignTask(std::shared_ptr<ProductionTask> task) {
    if (!canAcceptTask() || task == nullptr || task->currentStep() == nullptr) {
        return false;
    }
    if (!canProcess(task->currentStep()->requiredRole())) {
        return false;
    }

    task_ = std::move(task);
    progress_ = 0.0;
    notify(EventType::TaskEnqueued, name_ + " accepted " + task_->getProductName());
    notify(EventType::TaskStarted, name_ + " started " + task_->getProductName());
    transitionToWorking("task assigned");
    return true;
}

void Machine::setRecipe(const Recipe& recipe) {
    if (canAcceptRecipe(recipe)) {
        recipe_ = recipe;
    }
}

void Machine::setState(std::unique_ptr<MachineState> state) {
    if (state_) {
        state_->exit(*this);
    }
    state_ = std::move(state);
    if (state_) {
        state_->enter(*this);
    }
}

void Machine::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
}

void Machine::update(double deltaTime) {
    simulationTime_ += std::max(0.0, deltaTime);
    if (state_) {
        state_->update(*this, deltaTime);
    }
}

bool Machine::canAcceptTask() const {
    return task_ == nullptr && status_ == MachineStatus::Idle && health_ > 0.0;
}

bool Machine::canProcess(MachineRole role) const {
    return this->role() == role;
}

MachineSnapshot Machine::getSnapshot() const {
    return MachineSnapshot(id_, name_, typeName(), status_, stateName(), getProgress(), health_);
}

void Machine::forceBreak() {
    transitionToBroken("forced breakdown");
}

void Machine::repair() {
    transitionToMaintenance("repair requested");
}

MachineId Machine::getId() const {
    return id_;
}

const std::string& Machine::getName() const {
    return name_;
}

MachineStatus Machine::getStatus() const {
    return status_;
}

double Machine::getProgress() const {
    if (task_ == nullptr || task_->currentStep() == nullptr) {
        return progress_;
    }

    const auto totalSteps = task_->totalStepCount();
    if (totalSteps == 0) {
        return 1.0;
    }

    const auto currentStepProgress = progress_ / task_->currentStep()->baseDurationSeconds();
    return task_->getProgressInRoute() + (currentStepProgress / static_cast<double>(totalSteps));
}

double Machine::getHealth() const {
    return health_;
}

double Machine::getProcessingSpeed() const {
    return processingSpeed_;
}

double Machine::getBreakdownProbability() const {
    return breakdownProbability_;
}

void Machine::pause() {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }
    transitionToPaused("paused");
    notify(EventType::MachinePaused, name_ + " paused");
}

void Machine::resume() {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }
    if (hasTask()) {
        transitionToWorking("resumed");
        notify(EventType::TaskStarted, name_ + " resumed");
        return;
    }
    transitionToIdle("resumed");
}

void Machine::restoreStateObject(MachineStatus status) {
    status_ = status;
    state_ = makeMachineState(status_);
}

MachineMemento Machine::exportState(const std::unordered_map<const ProductionTask*, TaskMementoId>& taskIds) const {
    MachineMemento state;
    state.type = machineTypeKindOf(*this);
    state.id = id_;
    state.name = name_;
    state.status = status_;
    state.rawProgress = progress_;
    state.health = health_;
    state.processingSpeed = processingSpeed_;
    state.breakdownProbability = breakdownProbability_;
    state.maintenanceElapsed = maintenanceElapsed_;
    state.maintenanceDuration = maintenanceDuration_;
    state.simulationTime = simulationTime_;
    if (recipe_.has_value()) {
        state.recipe = exportRecipeState(*recipe_);
    }
    if (task_ != nullptr) {
        const auto found = taskIds.find(task_.get());
        if (found != taskIds.end()) {
            state.assignedTaskId = found->second;
        }
    }
    return state;
}

void Machine::restoreState(
    const MachineMemento& state,
    const std::unordered_map<TaskMementoId, std::shared_ptr<ProductionTask>>& tasks) {
    id_ = state.id;
    name_ = state.name;
    recipe_.reset();
    if (state.recipe.has_value()) {
        recipe_ = makeRecipe(*state.recipe);
    }

    task_.reset();
    if (state.assignedTaskId.has_value()) {
        const auto found = tasks.find(*state.assignedTaskId);
        if (found != tasks.end()) {
            task_ = found->second;
        }
    }

    progress_ = state.rawProgress;
    health_ = state.health;
    processingSpeed_ = state.processingSpeed;
    breakdownProbability_ = state.breakdownProbability;
    maintenanceElapsed_ = state.maintenanceElapsed;
    maintenanceDuration_ = state.maintenanceDuration;
    simulationTime_ = state.simulationTime;
    restoreStateObject(state.status);
}

void Machine::advanceProduction(double deltaTime) {
    if (task_ != nullptr && task_->currentStep() != nullptr) {
        const auto stepDuration = task_->currentStep()->baseDurationSeconds();
        if (stepDuration <= 0.0) {
            task_->advanceStep();
            notify(EventType::StepCompleted, name_ + " completed a process step");
            progress_ = 0.0;
            return;
        }

        progress_ += std::max(0.0, deltaTime) * processingSpeed_;
        if (progress_ >= stepDuration) {
            progress_ = 0.0;
            task_->advanceStep();
            notify(EventType::StepCompleted, name_ + " completed a process step");
            if (task_->isCompleted()) {
                notify(EventType::ProductCompleted, name_ + " completed " + task_->getProductName());
                task_.reset();
                transitionToIdle("task completed");
            } else {
                task_.reset();
                transitionToIdle("step completed");
            }
        }
        return;
    }

    if (!recipe_.has_value() || recipe_->durationSeconds() <= 0.0) {
        return;
    }

    progress_ += (std::max(0.0, deltaTime) * processingSpeed_) / recipe_->durationSeconds();
    if (progress_ >= 1.0) {
        progress_ = 0.0;
        notify(EventType::ProductCompleted, name_ + " completed " + recipe_->name());
    }
}

void Machine::advanceMaintenance(double deltaTime) {
    maintenanceElapsed_ += std::max(0.0, deltaTime);
    if (maintenanceElapsed_ >= maintenanceDuration_) {
        health_ = 100.0;
        progress_ = 0.0;
        task_.reset();
        notify(EventType::MachineRepaired, name_ + " repaired");
        transitionToIdle("maintenance completed");
    }
}

void Machine::transitionToIdle(const std::string& reason) {
    const auto previous = status_;
    status_ = MachineStatus::Idle;
    maintenanceElapsed_ = 0.0;
    setState(std::make_unique<IdleState>());
    onStateTransition(previous, status_, reason);
}

void Machine::transitionToWorking(const std::string& reason) {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }

    const auto previous = status_;
    status_ = MachineStatus::Working;
    setState(std::make_unique<WorkingState>());
    onStateTransition(previous, status_, reason);
}

void Machine::transitionToPaused(const std::string& reason) {
    const auto previous = status_;
    status_ = MachineStatus::Paused;
    setState(std::make_unique<PausedState>());
    onStateTransition(previous, status_, reason);
}

void Machine::transitionToBroken(const std::string& reason) {
    const auto previous = status_;
    health_ = 0.0;
    progress_ = 0.0;
    status_ = MachineStatus::Broken;
    setState(std::make_unique<BrokenState>());
    onStateTransition(previous, status_, reason);
    notify(EventType::MachineBroken, name_ + " broke down");
}

void Machine::transitionToMaintenance(const std::string& reason) {
    const auto previous = status_;
    maintenanceElapsed_ = 0.0;
    progress_ = 0.0;
    status_ = MachineStatus::Maintenance;
    setState(std::make_unique<MaintenanceState>());
    onStateTransition(previous, status_, reason);
}

void Machine::onStateTransition(MachineStatus from, MachineStatus to, const std::string& reason) {
    if (from == to) {
        return;
    }

    notify(EventType::StateChanged, name_ + " state changed: " + reason);
}

void Machine::notify(EventType type, const std::string& message) const {
    if (eventBus_ != nullptr) {
        eventBus_->publish(Event(simulationTime_, type, id_, message));
    }
}

Carbonator::Carbonator(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string Carbonator::typeName() const {
    return "Carbonator";
}

ProcessType Carbonator::processType() const {
    return ProcessType::Carbonation;
}

MachineRole Carbonator::role() const {
    return MachineRole::Carbonator;
}

bool Carbonator::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

Filler::Filler(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string Filler::typeName() const {
    return "Filler";
}

ProcessType Filler::processType() const {
    return ProcessType::Filling;
}

MachineRole Filler::role() const {
    return MachineRole::Filler;
}

bool Filler::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

Conveyor::Conveyor(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string Conveyor::typeName() const {
    return "Conveyor";
}

ProcessType Conveyor::processType() const {
    return ProcessType::Conveying;
}

MachineRole Conveyor::role() const {
    return MachineRole::Conveyor;
}

bool Conveyor::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

Sealer::Sealer(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string Sealer::typeName() const {
    return "Sealer";
}

ProcessType Sealer::processType() const {
    return ProcessType::Sealing;
}

MachineRole Sealer::role() const {
    return MachineRole::Sealer;
}

bool Sealer::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

Labeler::Labeler(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string Labeler::typeName() const {
    return "Labeler";
}

ProcessType Labeler::processType() const {
    return ProcessType::Labeling;
}

MachineRole Labeler::role() const {
    return MachineRole::Labeler;
}

bool Labeler::canAcceptRecipe(const Recipe& recipe) const {
    (void)recipe;
    return true;
}

} // namespace gactorio
