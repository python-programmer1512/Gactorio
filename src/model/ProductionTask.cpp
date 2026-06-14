#include "model/ProductionTask.hpp"

#include <algorithm>

// =============================================================================
// ProductionTask.cpp — 한 제품의 공정 진행 상태 추적 구현
// product_ 가 실제 바라보는 Product. currentStepIndex_ 로 경로상의 위치를 센다.
// =============================================================================

namespace gactorio {
namespace {

StepOutputMemento toMemento(const StepOutput& output) {
    return StepOutputMemento(output.itemId, output.productId, output.quantity);
}

StepOutput fromMemento(const StepOutputMemento& memento) {
    return StepOutput{memento.itemId(), memento.productId(), memento.quantity()};
}

} // namespace

// 외부 Product 참조 생성(비소유). ownedProduct_ 는 비움.
ProductionTask::ProductionTask(const Product& product)
    : product_(&product) {}

// Product 공유 소유 생성. product_ 는 소유 객체를 가리킨다.
ProductionTask::ProductionTask(std::shared_ptr<Product> product)
    : ownedProduct_(std::move(product)), product_(ownedProduct_.get()) {}

// 현재 공정 단계. 제품이 없거나 이미 완료면 nullptr.
const ProcessStep* ProductionTask::currentStep() const {
    if (product_ == nullptr || isCompleted()) {
        return nullptr;
    }
    return &product_->getRoute().at(currentStepIndex_);
}

// 다음 단계로 진행(완료 이후엔 무시).
void ProductionTask::advanceStep() {
    if (product_ == nullptr || isCompleted()) {
        return;
    }
    const auto& outputs = product_->getRoute().at(currentStepIndex_).outputs();
    pendingStepOutputs_.insert(pendingStepOutputs_.end(), outputs.begin(), outputs.end());
    ++currentStepIndex_;
    currentStepInputsConsumed_ = false;
}

// 모든 단계를 지났으면 완료. 제품이 없으면(이론상) 완료 취급.
bool ProductionTask::isCompleted() const {
    if (product_ == nullptr) {
        return true;
    }
    return currentStepIndex_ >= product_->getRoute().size();
}

// 경로 전체 진행률(단계 단위, 0.0~1.0). 예: 4단계 중 2단계 끝 → 0.5.
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
