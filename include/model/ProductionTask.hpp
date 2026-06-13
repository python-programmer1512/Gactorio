#pragma once

// =============================================================================
// ProductionTask — "제품 1단위를 공정 경로를 따라 만드는 중인 진행 상태"
// -----------------------------------------------------------------------------
// 하나의 Product 를 받아, 그 route(ProcessStep 목록)에서 현재 몇 번째 단계인지를
// currentStepIndex_ 로 추적한다. 기계가 한 단계를 끝내면 advanceStep()으로 다음
// 단계로 넘어가고, 모든 단계를 지나면 isCompleted() == true.
//
// 소유 모델(중요):
//   - shared_ptr 생성자로 만들면 ownedProduct_ 가 Product 수명을 공유 소유.
//   - const Product& 생성자로 만들면 외부 Product 를 product_ 로 가리키기만 함(비소유).
//   product_ 는 항상 "실제로 바라보는 대상" 포인터다.
// =============================================================================

#include "model/Product.hpp"

#include <cstddef>
#include <memory>
#include <string>

namespace gactorio {

class ProductionTask {
public:
    explicit ProductionTask(const Product& product);          // 외부 Product 참조(비소유)
    explicit ProductionTask(std::shared_ptr<Product> product); // Product 공유 소유

    const ProcessStep* currentStep() const;   // 현재 공정 단계(완료/없으면 nullptr)
    void advanceStep();                        // 다음 단계로 진행
    bool isCompleted() const;                  // 모든 단계 통과 여부
    double getProgressInRoute() const;         // 경로 전체 진행률 0.0~1.0(단계 단위)
    ProductId getProductId() const;            // 만드는 제품 ID
    const std::string& getProductName() const; // 만드는 제품명
    std::size_t currentStepIndex() const;      // 현재 단계 인덱스
    std::size_t totalStepCount() const;        // 전체 단계 수

private:
    std::shared_ptr<Product> ownedProduct_;  // (shared 생성자일 때만) Product 공유 소유
    const Product* product_;                 // 실제로 바라보는 Product(비소유 포인터)
    std::size_t currentStepIndex_ = 0;       // 현재 진행 중인 단계 번호(0부터)
};

} // namespace gactorio
