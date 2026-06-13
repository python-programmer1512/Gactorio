#include "model/Machine.hpp"

#include "common/Config.h"
#include "model/MachineStates.hpp"

#include <algorithm>
#include <random>
#include <utility>

namespace gactorio {

namespace {
// Shared per-thread RNG for HP-damage rolls. Avoids the overhead of
// reseeding every tick while keeping behaviour reproducible-ish per run.
std::mt19937& rng() {
    thread_local std::mt19937 gen{std::random_device{}()};
    return gen;
}
double uniform01() {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng());
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
      health_(std::clamp(initialHealth, 0.0, config::kInitialHealth)),
      processingSpeed_(std::max(0.0, processingSpeed)),
      breakdownProbability_(std::clamp(breakdownProbability, 0.0, 1.0)) {
    maintenanceDuration_ = config::kRepairAllDelaySeconds;
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

const ProductionTask* Machine::currentTask() const {
    return hasTask() ? task_.get() : nullptr;
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

void Machine::forceBreak() {
    // forceBreak is a debug / test hook — it discards the in-flight task so
    // the machine returns cleanly to Idle after maintenance. (Game-flow
    // damage from advanceProduction does not call this path, so the task is
    // preserved there and Repair restarts it from progress 0.)
    task_.reset();
    transitionToBroken("forced breakdown");
}

void Machine::repair() {
    transitionToMaintenance("repair requested");
}

void Machine::resetForRestore(double newHealth, MachineStatus newStatus) {
    task_.reset();
    progress_           = 0.0;
    maintenanceElapsed_ = 0.0;
    health_             = std::clamp(newHealth, 0.0, config::kInitialHealth);

    switch (newStatus) {
    case MachineStatus::Broken:
        status_ = MachineStatus::Broken;
        setState(std::make_unique<BrokenState>());
        break;
    case MachineStatus::Maintenance:
        status_ = MachineStatus::Maintenance;
        setState(std::make_unique<MaintenanceState>());
        break;
    case MachineStatus::Working:
    case MachineStatus::Idle:
    default:
        status_ = MachineStatus::Idle;
        setState(std::make_unique<IdleState>());
        break;
    }
}

void Machine::incrementalRepair() {
    // Quick +X HP boost. Does not change state, does not revive a broken
    // machine — use repair()/repairAll for that.
    if (status_ == MachineStatus::Broken) {
        return;
    }
    health_ = std::min(100.0, health_ + config::kIncrementalRepairHp);
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

void Machine::resetScenarioModifiers() {
    scenarioSpeedMultiplier_ = 1.0;
    scenarioBreakdownProbabilityOverride_.reset();
}

void Machine::setScenarioSpeedMultiplier(double multiplier) {
    scenarioSpeedMultiplier_ = std::max(0.0, multiplier);
}

void Machine::setScenarioBreakdownProbabilityOverride(std::optional<double> probability) {
    if (probability.has_value()) {
        scenarioBreakdownProbabilityOverride_ = std::clamp(*probability, 0.0, 1.0);
        return;
    }
    scenarioBreakdownProbabilityOverride_.reset();
}

double Machine::effectiveProcessingSpeed() const {
    return processingSpeed_ * scenarioSpeedMultiplier_;
}

double Machine::effectiveBreakdownProbability() const {
    return scenarioBreakdownProbabilityOverride_.value_or(breakdownProbability_);
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
    // ----- Random wear-and-tear -------------------------------------------
    // While a machine has a task and is working, every tick there is a small
    // chance of taking random HP damage. Probability + damage range come from
    // data/factory_config.json via the generated gactorio::config:: constants.
    // If HP reaches zero the task is preserved so Repair-All can resume it
    // from progress 0.
    if (task_ != nullptr) {
        const double dt = std::max(0.0, deltaTime);
        const double damageChance = effectiveBreakdownProbability() * dt;
        if (uniform01() < damageChance) {
            const double range = config::kDamageMaxHp - config::kDamageMinHp;
            const double dmg   = config::kDamageMinHp + uniform01() * range;
            health_ = std::max(0.0, health_ - dmg);
            if (health_ <= 0.0) {
                progress_ = 0.0;
                transitionToBroken("HP depleted while working");
                return;
            }
        }
    }
    // ----------------------------------------------------------------------

    if (task_ != nullptr && task_->currentStep() != nullptr) {
        const auto stepDuration = task_->currentStep()->baseDurationSeconds();
        if (stepDuration <= 0.0) {
            task_->advanceStep();
            notify(EventType::StepCompleted, name_ + " completed a process step");
            progress_ = 0.0;
            return;
        }

        progress_ += std::max(0.0, deltaTime) * effectiveProcessingSpeed();
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

    progress_ += (std::max(0.0, deltaTime) * effectiveProcessingSpeed()) / recipe_->durationSeconds();
    if (progress_ >= 1.0) {
        progress_ = 0.0;
        notify(EventType::ProductCompleted, name_ + " completed " + recipe_->name());
    }
}

void Machine::advanceMaintenance(double deltaTime) {
    maintenanceElapsed_ += std::max(0.0, deltaTime);
    if (maintenanceElapsed_ >= maintenanceDuration_) {
        health_ = config::kInitialHealth;
        progress_ = 0.0;
        notify(EventType::MachineRepaired, name_ + " repaired");
        if (task_ != nullptr) {
            // A task survived the breakdown (i.e. HP depleted naturally).
            // Resume it from the start of its current step.
            notify(EventType::TaskStarted, name_ + " restarted " + task_->getProductName());
            transitionToWorking("repair complete, resuming task");
        } else {
            // No task in flight (forceBreak or finished cleanly).
            transitionToIdle("maintenance completed");
        }
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
    // Block from Broken (must repair first), but allow from Maintenance —
    // that's how a finished repair resumes the preserved task.
    if (status_ == MachineStatus::Broken) {
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
