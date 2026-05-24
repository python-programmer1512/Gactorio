#include "model/Machine.hpp"

#include "model/MachineStates.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

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
    transitionToIdle("paused");
    notify(EventType::MachinePaused, name_ + " paused");
}

void Machine::resume() {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }
    transitionToWorking("resumed");
    notify(EventType::TaskStarted, name_ + " resumed");
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

// -----------------------------------------------------------------------------
// Four concrete stations. The base Machine handles state/health/progress;
// each station only declares its identity and which recipe it can serve.
// -----------------------------------------------------------------------------
MixingStation::MixingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string MixingStation::typeName() const                      { return "Mixing Station"; }
ProcessType MixingStation::processType() const                   { return ProcessType::Mixing; }
MachineRole MixingStation::role() const                          { return MachineRole::Mixing; }
bool MixingStation::canAcceptRecipe(const Recipe& recipe) const  { (void)recipe; return true; }

QualityStation::QualityStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string QualityStation::typeName() const                     { return "Quality Station"; }
ProcessType QualityStation::processType() const                  { return ProcessType::Quality; }
MachineRole QualityStation::role() const                         { return MachineRole::Quality; }
bool QualityStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

BottlingStation::BottlingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string BottlingStation::typeName() const                     { return "Bottling Station"; }
ProcessType BottlingStation::processType() const                  { return ProcessType::Bottling; }
MachineRole BottlingStation::role() const                         { return MachineRole::Bottling; }
bool BottlingStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

PackagingStation::PackagingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string PackagingStation::typeName() const                     { return "Packaging Station"; }
ProcessType PackagingStation::processType() const                  { return ProcessType::Packaging; }
MachineRole PackagingStation::role() const                         { return MachineRole::Packaging; }
bool PackagingStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

} // namespace gactorio
