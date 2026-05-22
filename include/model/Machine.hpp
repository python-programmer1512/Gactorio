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
    void repair();
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

class Carbonator final : public Machine {
public:
    Carbonator(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 100.0,
        double breakdownProbability = 0.01);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class Cutter final : public Machine {
public:
    Cutter(
        MachineId id,
        std::string name,
        double processingSpeed = 1.25,
        double initialHealth = 95.0,
        double breakdownProbability = 0.03);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class Conveyor final : public Machine {
public:
    Conveyor(
        MachineId id,
        std::string name,
        double processingSpeed = 2.0,
        double initialHealth = 100.0,
        double breakdownProbability = 0.005);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class Assembler final : public Machine {
public:
    Assembler(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 100.0,
        double breakdownProbability = 0.02);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class Painter final : public Machine {
public:
    Painter(
        MachineId id,
        std::string name,
        double processingSpeed = 0.8,
        double initialHealth = 90.0,
        double breakdownProbability = 0.04);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

} // namespace gactorio
