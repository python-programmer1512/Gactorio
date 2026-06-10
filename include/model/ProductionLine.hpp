#pragma once

#include "common/Types.hpp"
#include "dto/ProductionLineSnapshot.hpp"
#include "model/Machine.hpp"
#include "model/Product.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"

#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace gactorio {

class ProductionLine {
public:
    ProductionLine(ProductionLineId id, std::string name);
    ProductionLine(const ProductionLine&) = delete;
    ProductionLine& operator=(const ProductionLine&) = delete;
    ProductionLine(ProductionLine&&) noexcept = default;
    ProductionLine& operator=(ProductionLine&&) noexcept = default;

    ProductionLineId id() const;
    const std::string& name() const;
    const std::vector<std::unique_ptr<Machine>>& machines() const;

    void setEventBus(EventBus* eventBus);
    void enqueueProduct(std::shared_ptr<Product> product);
    std::size_t queueLength() const;
    std::shared_ptr<ProductionTask> currentTask() const;
    ProductionLineSnapshot getSnapshot() const;
    void assignAvailableTask();
    std::vector<ProductId> collectCompletedProducts();
    void addMachine(std::unique_ptr<Machine> machine);
    Machine* findMachine(MachineId id);
    const Machine* findMachine(MachineId id) const;
    void update(double deltaTime);

    // ---- Memento support (used by Factory::create/restoreFromMemento) ----
    std::vector<ProductId> pendingProductIds() const;
    void clearQueue();
    void clearCompleted();

private:
    ProductionLineId id_;
    std::string name_;
    std::deque<std::shared_ptr<ProductionTask>> taskQueue_;
    std::vector<ProductId> completedProducts_;
    std::vector<std::unique_ptr<Machine>> machines_;
    EventBus* eventBus_ = nullptr;
};

} // namespace gactorio
