#pragma once

// =============================================================================
// ProductCatalog — 제품 "정의표"(레지스트리)와 제품 생성 팩토리 함수
// -----------------------------------------------------------------------------
// 목적(OCP/DIP): 새 제품을 추가할 때 곳곳의 if/switch 를 고치는 대신, 정의표
//   (productDefinitions)에 한 줄 추가하고 그 정의로부터 Product 를 생성하도록 한다.
//   Controller/Factory 는 구체 제품 클래스(VoltzClassic 등) 대신 이 레지스트리에
//   의존한다.
//
// UML 의존 관계:
//   ProductCatalog ◆ ProductDefinition  (불변 카탈로그 데이터를 소유)
//   ProductCatalog ⇢ Product            (createProduct 가 Product 객체를 생성)
// =============================================================================

#include "model/Product.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gactorio {

// 제품 한 종에 대한 모든 메타데이터를 담는 "값 운반용" 구조체(DTO 성격).
// 시뮬레이션 entity 가 아니라 카탈로그 정의이므로 public 필드를 허용한다.
struct ProductDefinition {
    ProductId id;                                 // 제품 ID(= ProductType 값)
    ProductType type;                             // 제품 종류 enum
    std::string key;                              // 안정적 UI 키 (예: "VoltzClassic")
    std::string name;                             // 표시 이름 (예: "Voltz Classic")
    std::string tier;                             // 등급 (standard/premium/specialty)
    double totalDurationSeconds;                  // 전 공정 합산 소요 시간
    std::vector<ItemRequirement> requirements;    // 필요 재료
    std::vector<ProcessStep> route;               // 공정 경로
};

// 전체 제품 정의 목록(프로그램 수명 동안 불변, 정적 저장).
const std::vector<ProductDefinition>& productDefinitions();
// ID/타입으로 정의를 조회(없으면 nullptr).
const ProductDefinition* findProductDefinition(ProductId id);
const ProductDefinition* findProductDefinition(ProductType type);

// 정의로부터 Product 객체를 생성(팩토리). 알 수 없는 ID/타입이면 nullptr.
std::shared_ptr<Product> createProduct(ProductId id);
std::shared_ptr<Product> createProduct(ProductType type);

} // namespace gactorio
