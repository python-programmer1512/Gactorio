#include "model/ProductionTask.hpp"

#include <algorithm>

namespace gactorio {

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

} // namespace gactorio
