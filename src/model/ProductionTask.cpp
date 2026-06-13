#include "model/ProductionTask.hpp"

#include <algorithm>

namespace gactorio {
namespace {

StepOutputMemento toMemento(const StepOutput& output) {
    return StepOutputMemento(output.itemId, output.productId, output.quantity);
}

StepOutput fromMemento(const StepOutputMemento& memento) {
    return StepOutput{memento.itemId(), memento.productId(), memento.quantity()};
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
    const auto& outputs = product_->getRoute().at(currentStepIndex_).outputs();
    pendingStepOutputs_.insert(pendingStepOutputs_.end(), outputs.begin(), outputs.end());
    ++currentStepIndex_;
    currentStepInputsConsumed_ = false;
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
    if (product_ == nullptr) {
        return {};
    }
    return product_->getProductId();
}

const ProductId& ProductionTask::productId() const {
    static const ProductId empty;
    if (product_ == nullptr) {
        return empty;
    }
    return product_->productId();
}

const RecipeId& ProductionTask::recipeId() const {
    static const RecipeId empty;
    if (product_ == nullptr) {
        return empty;
    }
    return product_->defaultRecipeId();
}

const std::string& ProductionTask::getProductName() const {
    static const std::string empty;
    if (product_ == nullptr) {
        return empty;
    }
    return product_->getName();
}

const std::string& ProductionTask::currentStepKind() const {
    static const std::string empty;
    const auto* step = currentStep();
    return step == nullptr ? empty : step->stepKind();
}

const std::vector<ItemRequirement>& ProductionTask::currentStepInputs() const {
    static const std::vector<ItemRequirement> empty;
    const auto* step = currentStep();
    return step == nullptr ? empty : step->inputs();
}

const std::vector<StepOutput>& ProductionTask::currentStepOutputs() const {
    static const std::vector<StepOutput> empty;
    const auto* step = currentStep();
    return step == nullptr ? empty : step->outputs();
}

bool ProductionTask::usesStepLevelIO() const {
    return product_ != nullptr && product_->usesStepLevelIO();
}

bool ProductionTask::hasStepProductOutput() const {
    return product_ != nullptr && product_->hasStepProductOutput();
}

bool ProductionTask::currentStepInputsConsumed() const {
    return currentStepInputsConsumed_;
}

void ProductionTask::markCurrentStepInputsConsumed() {
    currentStepInputsConsumed_ = true;
}

std::vector<StepOutput> ProductionTask::collectPendingStepOutputs() {
    auto outputs = std::move(pendingStepOutputs_);
    pendingStepOutputs_.clear();
    return outputs;
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

ProductionTaskMemento ProductionTask::createMemento() const {
    std::vector<StepOutputMemento> pendingOutputs;
    pendingOutputs.reserve(pendingStepOutputs_.size());
    for (const auto& output : pendingStepOutputs_) {
        pendingOutputs.push_back(toMemento(output));
    }

    return ProductionTaskMemento(
        getProductId(),
        recipeId(),
        std::min(currentStepIndex_, totalStepCount()),
        currentStepInputsConsumed_,
        std::move(pendingOutputs));
}

void ProductionTask::restoreFromMemento(const ProductionTaskMemento& memento) {
    currentStepIndex_ = std::min(memento.currentStepIndex(), totalStepCount());
    currentStepInputsConsumed_ = memento.currentStepInputsConsumed();

    pendingStepOutputs_.clear();
    pendingStepOutputs_.reserve(memento.pendingStepOutputs().size());
    for (const auto& output : memento.pendingStepOutputs()) {
        pendingStepOutputs_.push_back(fromMemento(output));
    }
}

} // namespace gactorio
