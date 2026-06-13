#include "model/Product.hpp"

#include "model/ProductCatalog.hpp"

#include <stdexcept>
#include <utility>

// =============================================================================
// Product.cpp — ItemRequirement/ProcessStep/Product(추상) 및 구체 제품 3종 구현
// 구체 제품들은 자신의 ProductType 으로 카탈로그 정의를 찾아 base 에 넘긴다.
// =============================================================================

namespace gactorio {

namespace {

// 타입에 해당하는 카탈로그 정의를 찾는다. 없으면(데이터 누락) 논리 오류 예외.
const ProductDefinition& definitionFor(ProductType type) {
    const auto* definition = findProductDefinition(type);
    if (definition == nullptr) {
        throw std::logic_error("missing product definition");
    }
    return *definition;
}

} // namespace

// -----------------------------------------------------------------------------
// ItemRequirement — (원자재 종류, 수량) 값 객체
// -----------------------------------------------------------------------------
ItemRequirement::ItemRequirement(ItemType itemType, int quantity)
    : itemType_(itemType), quantity_(quantity) {}

ItemType ItemRequirement::itemType() const { return itemType_; }
int      ItemRequirement::quantity() const { return quantity_; }

// -----------------------------------------------------------------------------
// ProcessStep — (필요 역할, 기본 처리 시간) 값 객체. durationSeconds 는 현재 base 와 동일.
// -----------------------------------------------------------------------------
ProcessStep::ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds)
    : requiredRole_(requiredRole), baseDurationSeconds_(baseDurationSeconds) {}

MachineRole    ProcessStep::requiredRole() const        { return requiredRole_; }
SimulationTime ProcessStep::baseDurationSeconds() const { return baseDurationSeconds_; }
SimulationTime ProcessStep::durationSeconds() const     { return baseDurationSeconds_; }

// -----------------------------------------------------------------------------
// Product (추상 base) — 재료/경로 데이터를 캡슐화. 두 생성자(정의 기반/직접 값) 제공.
// -----------------------------------------------------------------------------
Product::Product(ProductId id,
                 std::string name,
                 std::vector<ItemRequirement> requirements,
                 std::vector<ProcessStep> route)
    : id_(id),
      name_(std::move(name)),
      requirements_(std::move(requirements)),
      route_(std::move(route)) {}

Product::Product(const ProductDefinition& definition)
    : Product(
          definition.id,
          definition.name,
          definition.requirements,
          definition.route) {}

Product::~Product() = default;

ProductId          Product::storedProductId()    const { return id_; }
const std::string& Product::storedName()         const { return name_; }
const std::vector<ItemRequirement>& Product::storedRequirements() const { return requirements_; }
const std::vector<ProcessStep>&     Product::storedRoute()        const { return route_; }

// -----------------------------------------------------------------------------
// Voltz Classic — 기본형 에너지 드링크(빠르고 저렴).
//   경로 시간(data/factory_config.json): 13 + 9 + 9 + 9 = 40초
//   세 구체 제품 모두 자기 ProductType 으로 카탈로그 정의를 찾아 base 에 전달한다.
// -----------------------------------------------------------------------------
VoltzClassic::VoltzClassic()
    : Product(definitionFor(ProductType::VoltzClassic)) {}

ProductId          VoltzClassic::getProductId() const         { return storedProductId(); }
const std::string& VoltzClassic::getName() const              { return storedName(); }
const std::vector<ItemRequirement>& VoltzClassic::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     VoltzClassic::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Hyper Bolt — 프리미엄 고카페인. MIXING/QUALITY 공정이 더 무겁다.
//   경로 시간: 18 + 12 + 9 + 9 = 48초
// -----------------------------------------------------------------------------
HyperBolt::HyperBolt()
    : Product(definitionFor(ProductType::HyperBolt)) {}

ProductId          HyperBolt::getProductId() const            { return storedProductId(); }
const std::string& HyperBolt::getName() const                 { return storedName(); }
const std::vector<ItemRequirement>& HyperBolt::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     HyperBolt::getRoute()        const { return storedRoute(); }

// -----------------------------------------------------------------------------
// Aurora Zero — 무가당 스페셜. QUALITY 검사와 PACKAGING 이 특히 신중하다.
//   경로 시간: 17 + 12 + 9 + 11 = 49초
// -----------------------------------------------------------------------------
AuroraZero::AuroraZero()
    : Product(definitionFor(ProductType::AuroraZero)) {}

ProductId          AuroraZero::getProductId() const           { return storedProductId(); }
const std::string& AuroraZero::getName() const                { return storedName(); }
const std::vector<ItemRequirement>& AuroraZero::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     AuroraZero::getRoute()        const { return storedRoute(); }

} // namespace gactorio
