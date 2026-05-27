#include "model/ProductionTask.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

namespace {

std::shared_ptr<Product> makeProduct(ProductId productId) {
    switch (static_cast<ProductType>(productId)) {
    case ProductType::SodaCan:
        return std::make_shared<SodaCan>();
    case ProductType::SparklingWater:
        return std::make_shared<SparklingWater>();
    case ProductType::EnergyDrink:
        return std::make_shared<EnergyDrink>();
    case ProductType::Unknown:
    default:
        return nullptr;
    }
}

} // namespace

ProductionTask::ProductionTask(const Product& product)
    : product_(&product) {}

ProductionTask::ProductionTask(std::shared_ptr<Product> product)
    : ownedProduct_(std::move(product)), product_(ownedProduct_.get()) {}

const ProcessStep* ProductionTask::currentStep() const {
    if (product_ == nullptr || isCompleted()) {
        return nullptr;
    }
    return &product_->getRoute().at(currentStepIndex_);
}

void ProductionTask::advanceStep() {
    if (product_ == nullptr || isCompleted()) {
        return;
    }
    ++currentStepIndex_;
}

bool ProductionTask::isCompleted() const {
    if (product_ == nullptr) {
        return true;
    }
    return currentStepIndex_ >= product_->getRoute().size();
}

double ProductionTask::getProgressInRoute() const {
    if (product_ == nullptr || product_->getRoute().empty()) {
        return 1.0;
    }

    const auto completedSteps = std::min(currentStepIndex_, product_->getRoute().size());
    return static_cast<double>(completedSteps) / static_cast<double>(product_->getRoute().size());
}

ProductId ProductionTask::getProductId() const {
    return product_->getProductId();
}

const std::string& ProductionTask::getProductName() const {
    return product_->getName();
}

std::size_t ProductionTask::currentStepIndex() const {
    return currentStepIndex_;
}

std::size_t ProductionTask::totalStepCount() const {
    if (product_ == nullptr) {
        return 0;
    }
    return product_->getRoute().size();
}

ProductionTaskMemento ProductionTask::exportState(TaskMementoId id) const {
    ProductionTaskMemento state;
    state.taskId = id;
    state.productId = getProductId();
    state.currentStepIndex = currentStepIndex_;
    return state;
}

void ProductionTask::restoreCurrentStepIndex(std::size_t currentStepIndex) {
    currentStepIndex_ = currentStepIndex;
}

std::shared_ptr<ProductionTask> ProductionTask::fromState(const ProductionTaskMemento& state) {
    auto product = makeProduct(state.productId);
    if (product == nullptr) {
        return nullptr;
    }

    auto task = std::make_shared<ProductionTask>(std::move(product));
    task->restoreCurrentStepIndex(state.currentStepIndex);
    return task;
}

} // namespace gactorio
