#pragma once

#include "model/memento/FactoryMemento.hpp"
#include "model/Product.hpp"

#include <cstddef>
#include <memory>
#include <string>

namespace gactorio {

class ProductionTask {
public:
    explicit ProductionTask(const Product& product);
    explicit ProductionTask(std::shared_ptr<Product> product);

    const ProcessStep* currentStep() const;
    void advanceStep();
    bool isCompleted() const;
    double getProgressInRoute() const;
    ProductId getProductId() const;
    const std::string& getProductName() const;
    std::size_t currentStepIndex() const;
    std::size_t totalStepCount() const;
    ProductionTaskMemento exportState(TaskMementoId id) const;
    void restoreCurrentStepIndex(std::size_t currentStepIndex);
    static std::shared_ptr<ProductionTask> fromState(const ProductionTaskMemento& state);

private:
    std::shared_ptr<Product> ownedProduct_;
    const Product* product_;
    std::size_t currentStepIndex_ = 0;
};

} // namespace gactorio
