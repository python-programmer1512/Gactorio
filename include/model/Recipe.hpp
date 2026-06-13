#pragma once

// =============================================================================
// Recipe — (입력 재료 → 출력 제품) 변환 규칙을 담는 데이터 객체
// -----------------------------------------------------------------------------
// 한 레시피는 "무엇을 얼마나 넣으면(inputs) 무엇이 얼마나 나오고(outputs), 몇 초
// 걸리는가(durationSeconds)"를 표현한다. 어떤 구체 기계가 실행하는지는 모른다(데이터 지향).
//
// 현 런타임의 실제 생산 흐름은 Product 의 route(ProcessStep) 기반으로 동작하며,
// Recipe 는 CarbonationFactory 가 카탈로그로부터 생성해 보관/조회용으로 들고 있다.
// =============================================================================

#include "common/Types.hpp"

#include <map>
#include <string>

namespace gactorio {

class Recipe {
public:
    Recipe(RecipeId id, std::string name, double durationSeconds);

    RecipeId id() const;                 // 레시피 ID
    const std::string& name() const;     // 레시피 이름
    double durationSeconds() const;      // 총 소요 시간(초)

    void addInput(ItemType itemType, int quantity);    // 입력 재료 누적 추가
    void addOutput(ProductId productId, int quantity);  // 출력 제품 누적 추가

    const std::map<ItemType, int>& inputs() const;     // 입력 재료 맵(종류→수량)
    const std::map<ProductId, int>& outputs() const;   // 출력 제품 맵(ID→수량)

private:
    RecipeId id_;
    std::string name_;
    double durationSeconds_;
    std::map<ItemType, int> inputs_;     // 캡슐화된 입력 목록
    std::map<ProductId, int> outputs_;   // 캡슐화된 출력 목록
};

} // namespace gactorio
