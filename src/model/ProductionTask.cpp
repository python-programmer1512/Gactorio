#include "model/ProductionTask.hpp"

#include <algorithm>

// =============================================================================
// ProductionTask.cpp — 한 제품의 공정 진행 상태 추적 구현
// product_ 가 실제 바라보는 Product. currentStepIndex_ 로 경로상의 위치를 센다.
// =============================================================================

namespace gactorio {

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
    ++currentStepIndex_;
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
