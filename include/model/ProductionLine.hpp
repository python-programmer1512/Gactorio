#pragma once

#include "common/ScenarioType.hpp"
#include "common/Types.hpp"
#include "model/Machine.hpp"
#include "model/Product.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gactorio {

class Inventory;

struct LineScenarioConfig {
    ScenarioType type;
    std::optional<MachineRole> bottleneckRole;
    double bottleneckSpeedMultiplier;
    std::optional<double> breakdownProbabilityOverride;
    std::optional<std::size_t> queueCapacity;
};

enum class EnqueueResult {
    Accepted,
    RejectedFull,
    LostOverflow
};

class ProductionLine {
public:
    ProductionLine(ProductionLineId id, std::string name);
    ProductionLine(const ProductionLine&) = delete;
    ProductionLine& operator=(const ProductionLine&) = delete;
    ProductionLine(ProductionLine&&) noexcept = default;
    ProductionLine& operator=(ProductionLine&&) noexcept = default;

    ProductionLineId id() const;
    const std::string& name() const;
    const std::string& definitionId() const noexcept;
    const std::vector<std::unique_ptr<Machine>>& machines() const;
    ScenarioType scenario() const;
    ScenarioType getScenario() const;

    void setScenario(ScenarioType scenario);
    void setDefinitionId(std::string definitionId);
    void setEventBus(EventBus* eventBus);
    EnqueueResult enqueueProduct(std::shared_ptr<Product> product);
    std::size_t queueLength() const;
    std::optional<std::size_t> queueCapacity() const;
    std::size_t queueCapacityValueOrZero() const;
    void setQueueCapacity(std::size_t capacity);
    void resetQueueCapacity();
    std::size_t droppedTaskCount() const;
    void setDroppedTaskCount(std::size_t count);
    std::shared_ptr<ProductionTask> currentTask() const;
    void assignAvailableTask();
    void assignAvailableTask(Inventory* inventory);
    std::vector<StepOutput> collectPendingStepOutputs();
    std::vector<ProductId> collectCompletedProducts();
    void addMachine(std::unique_ptr<Machine> machine);
    Machine* findMachine(MachineId id);
    const Machine* findMachine(MachineId id) const;
    void update(double deltaTime);

    // ---- Memento support (used by Factory::create/restoreFromMemento) ----
    std::vector<ProductId> pendingProductIds() const;
    std::vector<ProductionTaskMemento> pendingTaskMementos() const;
    std::optional<std::size_t> taskIndexFor(const ProductionTask* task) const;
    EnqueueResult enqueueTask(std::shared_ptr<ProductionTask> task);
    void clearQueue();
    void clearCompleted();

private:
    ProductionLineId id_;
    std::string name_;
    std::string definitionId_;
    ScenarioType scenario_ = ScenarioType::NormalFlow;
    std::deque<std::shared_ptr<ProductionTask>> taskQueue_;
    std::vector<ProductId> completedProducts_;
    std::vector<std::unique_ptr<Machine>> machines_;
    std::optional<std::size_t> queueCapacity_;
    std::size_t droppedTaskCount_ = 0;
    EventBus* eventBus_ = nullptr;
};

} // namespace gactorio
