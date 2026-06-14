#pragma once

#include "model/Product.hpp"
#include "model/memento/FactoryMemento.hpp"

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
    const ProductId& productId() const;
    const RecipeId& recipeId() const;
    const std::string& getProductName() const;
    const std::string& currentStepKind() const;
    const std::vector<ItemRequirement>& currentStepInputs() const;
    const std::vector<StepOutput>& currentStepOutputs() const;
    bool usesStepLevelIO() const;
    bool hasStepProductOutput() const;
    bool currentStepInputsConsumed() const;
    void markCurrentStepInputsConsumed();
    std::vector<StepOutput> collectPendingStepOutputs();
    std::size_t currentStepIndex() const;
    std::size_t totalStepCount() const;
    ProductionTaskMemento createMemento() const;
    void restoreFromMemento(const ProductionTaskMemento& memento);

private:
    std::shared_ptr<Product> ownedProduct_;
    const Product* product_;
    std::size_t currentStepIndex_ = 0;
    bool currentStepInputsConsumed_ = false;
    std::vector<StepOutput> pendingStepOutputs_;
};

} // namespace gactorio
