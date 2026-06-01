#pragma once

#include "common/Types.hpp"
#include "dto/MachineSnapshot.hpp"
#include "model/ProductionTask.hpp"
#include "model/Recipe.hpp"
#include "model/events/Event.hpp"
#include "model/events/EventBus.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gactorio {

class MachineState;
class IdleState;
class WorkingState;
class BrokenState;
class MaintenanceState;

class Machine {
public:
    Machine(MachineId id, std::string name);
    virtual ~Machine();

    MachineId id() const;
    const std::string& name() const;
    double progress() const;
    std::string stateName() const;
    bool hasTask() const;
    const std::optional<Recipe>& recipe() const;

    bool assignTask(std::shared_ptr<ProductionTask> task);
    void setRecipe(const Recipe& recipe);
    void setEventBus(EventBus* eventBus);
    virtual void update(double deltaTime);
    bool canAcceptTask() const;
    bool canProcess(MachineRole role) const;
    MachineSnapshot getSnapshot() const;
    void forceBreak();
    // repair() starts the long maintenance routine (full restore after delay).
    void repair();
    // Adds a small fixed amount of HP (config::kIncrementalRepairHp).
    // Cannot resurrect a Broken machine — use repair()/repairAll for that.
    void incrementalRepair();

    // ---- Memento support (used by Factory::restoreFromMemento) -----------
    // Drops any in-flight task, zeroes progress, overwrites HP, and forces
    // the machine into the given status.
    void resetForRestore(double newHealth, MachineStatus newStatus);

    MachineId getId() const;
    const std::string& getName() const;
    MachineStatus getStatus() const;
    double getProgress() const;
    double getHealth() const;
    double getProcessingSpeed() const;
    double getBreakdownProbability() const;
    void pause();
    void resume();

    virtual std::string typeName() const = 0;
    virtual ProcessType processType() const = 0;
    virtual MachineRole role() const = 0;
    virtual bool canAcceptRecipe(const Recipe& recipe) const = 0;

protected:
    Machine(
        MachineId id,
        std::string name,
        double processingSpeed,
        double initialHealth,
        double breakdownProbability);

    void notify(EventType type, const std::string& message) const;

private:
    friend class IdleState;
    friend class WorkingState;
    friend class BrokenState;
    friend class MaintenanceState;

    void setState(std::unique_ptr<MachineState> state);
    void transitionToIdle(const std::string& reason);
    void transitionToWorking(const std::string& reason);
    void transitionToBroken(const std::string& reason);
    void transitionToMaintenance(const std::string& reason);
    void onStateTransition(MachineStatus from, MachineStatus to, const std::string& reason);
    void advanceProduction(double deltaTime);
    void advanceMaintenance(double deltaTime);

    MachineId id_;
    std::string name_;
    std::optional<Recipe> recipe_;
    std::shared_ptr<ProductionTask> task_;
    std::unique_ptr<MachineState> state_;
    EventBus* eventBus_ = nullptr;
    MachineStatus status_ = MachineStatus::Idle;
    double progress_ = 0.0;
    double health_ = 100.0;
    double processingSpeed_ = 1.0;
    double breakdownProbability_ = 0.0;
    double maintenanceElapsed_ = 0.0;
    double maintenanceDuration_ = 2.0;
    SimulationTime simulationTime_ = 0.0;
};

// -----------------------------------------------------------------------------
// Four concrete stations. Each is responsible for one MachineRole and one
// ProcessType, and accepts any recipe whose role matches.
//
// Specs mirror data/factory_config.json:
//   MixingStation    HP 150, parallel 2, queue 8  - heaviest blending work
//   QualityStation   HP 100, parallel 3, queue 6  - precision check + carbonation
//   BottlingStation  HP 120, parallel 4, queue 10 - wash, fill, seal throughput
//   PackagingStation HP 140, parallel 2, queue 6  - label + pack, final stage
// -----------------------------------------------------------------------------
class MixingStation final : public Machine {
public:
    MixingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 150.0,
        double breakdownProbability = 0.02);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class QualityStation final : public Machine {
public:
    QualityStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 100.0,
        double breakdownProbability = 0.03);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class BottlingStation final : public Machine {
public:
    BottlingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 120.0,
        double breakdownProbability = 0.02);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class PackagingStation final : public Machine {
public:
    PackagingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 140.0,
        double breakdownProbability = 0.015);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

} // namespace gactorio
