#include "model/Product.hpp"

#include "model/ProductCatalog.hpp"
#include "model/config/ConfigIdAdapters.hpp"

#include <algorithm>
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

std::string stepKindFor(MachineRole role) {
    try {
        return config_model::toStepKind(role);
    } catch (const std::invalid_argument&) {
        return {};
    }
}

} // namespace

// -----------------------------------------------------------------------------
// ItemRequirement — (원자재 종류, 수량) 값 객체
// -----------------------------------------------------------------------------
ItemRequirement::ItemRequirement(std::string itemId, int quantity)
    : itemId_(std::move(itemId)), quantity_(quantity) {}

ItemRequirement::ItemRequirement(ItemType itemType, int quantity)
    : ItemRequirement(config_model::toItemId(itemType), quantity) {}

const std::string& ItemRequirement::itemId() const { return itemId_; }

ItemType ItemRequirement::itemType() const {
    return config_model::itemTypeFromId(itemId_).value_or(ItemType::Unknown);
}

int ItemRequirement::quantity() const { return quantity_; }

// -----------------------------------------------------------------------------
// StepOutput
// -----------------------------------------------------------------------------
bool StepOutput::isItem() const noexcept {
    return itemId.has_value();
}

bool StepOutput::isProduct() const noexcept {
    return productId.has_value();
}

// -----------------------------------------------------------------------------
// ProcessStep — (필요 역할, 기본 처리 시간) 값 객체. durationSeconds 는 현재 base 와 동일.
// -----------------------------------------------------------------------------
ProcessStep::ProcessStep(MachineRole requiredRole, SimulationTime baseDurationSeconds)
    : stepKind_(stepKindFor(requiredRole)),
      requiredRole_(requiredRole),
      baseDurationSeconds_(baseDurationSeconds) {}

ProcessStep::ProcessStep(std::string stepKind, SimulationTime baseDurationSeconds)
    : stepKind_(std::move(stepKind)),
      requiredRole_(config_model::machineRoleFromKind(stepKind_).value_or(MachineRole::Unknown)),
      baseDurationSeconds_(baseDurationSeconds) {}

ProcessStep::ProcessStep(
    std::string stepKind,
    MachineRole legacyRequiredRole,
    SimulationTime baseDurationSeconds)
    : stepKind_(std::move(stepKind)),
      requiredRole_(legacyRequiredRole),
      baseDurationSeconds_(baseDurationSeconds) {}

ProcessStep::ProcessStep(
    std::string id,
    std::string stepKind,
    MachineRole legacyRequiredRole,
    SimulationTime baseDurationSeconds,
    std::vector<ItemRequirement> inputs,
    std::vector<StepOutput> outputs)
    : id_(std::move(id)),
      stepKind_(std::move(stepKind)),
      requiredRole_(legacyRequiredRole),
      baseDurationSeconds_(baseDurationSeconds),
      inputs_(std::move(inputs)),
      outputs_(std::move(outputs)) {}

const std::string& ProcessStep::id() const noexcept { return id_; }
const std::string& ProcessStep::stepKind() const noexcept { return stepKind_; }
MachineRole ProcessStep::requiredRole() const noexcept { return requiredRole_; }
bool ProcessStep::hasLegacyRequiredRole() const noexcept { return requiredRole_ != MachineRole::Unknown; }
std::optional<MachineRole> ProcessStep::legacyRequiredRole() const noexcept {
    if (!hasLegacyRequiredRole()) {
        return std::nullopt;
    }
    return requiredRole_;
}
SimulationTime ProcessStep::baseDurationSeconds() const noexcept { return baseDurationSeconds_; }
SimulationTime ProcessStep::durationSeconds() const noexcept { return baseDurationSeconds_; }
const std::vector<ItemRequirement>& ProcessStep::inputs() const noexcept { return inputs_; }
const std::vector<StepOutput>& ProcessStep::outputs() const noexcept { return outputs_; }

// -----------------------------------------------------------------------------
// Product (추상 base) — 재료/경로 데이터를 캡슐화. 두 생성자(정의 기반/직접 값) 제공.
// -----------------------------------------------------------------------------
Product::Product(ProductId id,
                 RecipeId defaultRecipeId,
                 std::string name,
                 std::vector<ItemRequirement> requirements,
                 std::vector<ProcessStep> route)
    : id_(std::move(id)),
      defaultRecipeId_(std::move(defaultRecipeId)),
      name_(std::move(name)),
      requirements_(std::move(requirements)),
      route_(std::move(route)) {}

Product::Product(const ProductDefinition& definition)
    : Product(
          definition.id,
          definition.defaultRecipeId,
          definition.name,
          definition.requirements,
          definition.route) {}

Product::~Product() = default;

bool Product::usesStepLevelIO() const {
    return std::any_of(
        route_.begin(),
        route_.end(),
        [](const ProcessStep& step) {
            return !step.id().empty() || !step.inputs().empty() || !step.outputs().empty();
        });
}

bool Product::hasStepProductOutput() const {
    return std::any_of(
        route_.begin(),
        route_.end(),
        [](const ProcessStep& step) {
            return std::any_of(
                step.outputs().begin(),
                step.outputs().end(),
                [](const StepOutput& output) {
                    return output.isProduct();
                });
        });
}

ProductId          Product::storedProductId()    const { return id_; }
const ProductId&   Product::storedProductIdRef() const { return id_; }
const RecipeId&    Product::storedDefaultRecipeId() const { return defaultRecipeId_; }
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
const ProductId&   VoltzClassic::productId() const            { return storedProductIdRef(); }
const RecipeId&    VoltzClassic::defaultRecipeId() const      { return storedDefaultRecipeId(); }
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
const ProductId&   HyperBolt::productId() const               { return storedProductIdRef(); }
const RecipeId&    HyperBolt::defaultRecipeId() const         { return storedDefaultRecipeId(); }
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
const ProductId&   AuroraZero::productId() const              { return storedProductIdRef(); }
const RecipeId&    AuroraZero::defaultRecipeId() const        { return storedDefaultRecipeId(); }
const std::string& AuroraZero::getName() const                { return storedName(); }
const std::vector<ItemRequirement>& AuroraZero::getRequirements() const { return storedRequirements(); }
const std::vector<ProcessStep>&     AuroraZero::getRoute()        const { return storedRoute(); }

} // namespace gactorio
