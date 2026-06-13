#pragma once

// =============================================================================
// Product — 완제품 타입 계층 + 제품을 구성하는 값 객체(ItemRequirement/ProcessStep)
// -----------------------------------------------------------------------------
// 구성:
//   ItemRequirement : "이 제품을 만들려면 어떤 원자재가 몇 개 필요한가"
//   ProcessStep     : "공정 경로의 한 단계 = 어떤 역할의 기계가 몇 초 처리하는가"
//   Product(추상)    ──▷ VoltzClassic / HyperBolt / AuroraZero (구체 제품 3종)
//
// 각 구체 제품은 자신의 재료 목록(requirements)과 공정 경로(route)를 갖는다.
// 이 데이터는 ProductCatalog(=JSON에서 생성된 정의표)에서 채워진다.
// "두 개 이상의 제품 단계(시작/끝)"라는 과제 요구를 Product 추상 + 구체 제품으로 충족.
// =============================================================================

#include "common/Types.hpp"

#include <string>
#include <vector>

namespace gactorio {

struct ProductDefinition;   // ProductCatalog.hpp 에 정의된 카탈로그 데이터(전방 선언)

// 한 제품 1단위를 만들기 시작할 때 필요한 (원자재 종류, 수량) 쌍.
class ItemRequirement {
public:
    ItemRequirement(ItemType itemType, int quantity);
    ItemType itemType() const;   // 필요한 원자재 종류
    int quantity() const;        // 필요 수량

private:
    ItemType itemType_;
    int quantity_;
};

// 제품 경로상의 한 공정 단계: 어떤 역할(MachineRole)의 기계가 처리해야 하며,
// 기본 배속에서 몇 초가 걸리는지.
class ProcessStep {
public:
    ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds);
    MachineRole requiredRole() const;            // 이 단계를 처리할 기계 역할
    SimulationTime baseDurationSeconds() const;  // 기본(1배속) 처리 시간
    SimulationTime durationSeconds() const;      // 현재 구현은 base 와 동일

private:
    MachineRole requiredRole_;
    SimulationTime baseDurationSeconds_;
};

// 모든 완제품의 추상 기반. 재료/경로 데이터는 base(Product)에 캡슐화되고,
// 파생 클래스는 가상 getter 로만 노출한다.
class Product {
public:
    virtual ~Product();

    virtual ProductId getProductId() const = 0;                            // 제품 ID
    virtual const std::string& getName() const = 0;                        // 제품명
    virtual const std::vector<ItemRequirement>& getRequirements() const = 0; // 재료 목록
    virtual const std::vector<ProcessStep>& getRoute() const = 0;            // 공정 경로

protected:
    // 카탈로그 정의 하나로 base 데이터를 한 번에 채우는 생성자.
    explicit Product(const ProductDefinition& definition);

    // (직접 값을 넘기는) 대체 생성자.
    Product(ProductId id,
            std::string name,
            std::vector<ItemRequirement> requirements,
            std::vector<ProcessStep> route);

    // 파생 클래스가 base에 저장된 데이터를 읽는 통로(캡슐화 유지).
    ProductId storedProductId() const;
    const std::string& storedName() const;
    const std::vector<ItemRequirement>& storedRequirements() const;
    const std::vector<ProcessStep>& storedRoute() const;

private:
    ProductId id_;                                // 제품 ID(= ProductType 값)
    std::string name_;                            // 제품명
    std::vector<ItemRequirement> requirements_;   // 필요 재료 목록(composition)
    std::vector<ProcessStep> route_;              // 공정 경로(composition)
};

// -----------------------------------------------------------------------------
// 에너지 드링크 3종. 공정 시간은 data/factory_config.json 과 일치:
//   Voltz Classic : MIXING 13s, QUALITY 9s,  BOTTLING 9s, PACKAGING 9s
//   Hyper Bolt    : MIXING 18s, QUALITY 12s, BOTTLING 9s, PACKAGING 9s
//   Aurora Zero   : MIXING 17s, QUALITY 12s, BOTTLING 9s, PACKAGING 11s
// 세 클래스 모두 ProductCatalog 의 정의를 그대로 base 로 전달한다.
// -----------------------------------------------------------------------------
class VoltzClassic final : public Product {
public:
    VoltzClassic();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class HyperBolt final : public Product {
public:
    HyperBolt();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

class AuroraZero final : public Product {
public:
    AuroraZero();
    ProductId getProductId() const override;
    const std::string& getName() const override;
    const std::vector<ItemRequirement>& getRequirements() const override;
    const std::vector<ProcessStep>& getRoute() const override;
};

} // namespace gactorio
