#include "model/ProductCatalog.hpp"

#include "common/Config.h"

#include <algorithm>
#include <memory>

// =============================================================================
// ProductCatalog.cpp — 제품 정의표(데이터)와 생성 팩토리 구현
// 새 제품 추가 = 아래 definitions 리스트에 항목 하나 추가. 다른 코드 수정 불필요(OCP).
// =============================================================================

namespace gactorio {

namespace {

// 카탈로그 정의 하나로 만들어지는 범용 Product 구현체. 구체 제품 클래스(VoltzClassic
// 등)를 일일이 거치지 않고 정의표만으로 Product 를 찍어내기 위한 내부 클래스.
class CatalogProduct final : public Product {
public:
    explicit CatalogProduct(const ProductDefinition& definition)
        : Product(definition) {}

    ProductId getProductId() const override { return storedProductId(); }
    const std::string& getName() const override { return storedName(); }
    const std::vector<ItemRequirement>& getRequirements() const override { return storedRequirements(); }
    const std::vector<ProcessStep>& getRoute() const override { return storedRoute(); }
};

} // namespace

// 전체 제품 정의표. static 이라 프로그램 수명 동안 한 번만 생성되고 불변.
// 각 항목: id/type/key/name/tier/총시간/재료목록/공정경로. 시간은 config:: 상수(=JSON).
const std::vector<ProductDefinition>& productDefinitions() {
    static const std::vector<ProductDefinition> definitions = {
        {
            static_cast<ProductId>(ProductType::VoltzClassic),
            ProductType::VoltzClassic,
            "VoltzClassic",
            "Voltz Classic",
            "standard",
            config::kProductVoltzClassicTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  2),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductVoltzClassicMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductVoltzClassicQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductVoltzClassicBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductVoltzClassicPackagingTime),
            },
        },
        {
            static_cast<ProductId>(ProductType::HyperBolt),
            ProductType::HyperBolt,
            "HyperBolt",
            "Hyper Bolt",
            "premium",
            config::kProductHyperBoltTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  3),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductHyperBoltMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductHyperBoltQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductHyperBoltBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductHyperBoltPackagingTime),
            },
        },
        {
            static_cast<ProductId>(ProductType::AuroraZero),
            ProductType::AuroraZero,
            "AuroraZero",
            "Aurora Zero",
            "specialty",
            config::kProductAuroraZeroTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  2),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductAuroraZeroMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductAuroraZeroQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductAuroraZeroBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductAuroraZeroPackagingTime),
            },
        },
    };

    return definitions;
}

// ID로 정의 조회(선형 탐색, 항목 수가 적어 충분). 없으면 nullptr.
const ProductDefinition* findProductDefinition(ProductId id) {
    const auto& definitions = productDefinitions();
    const auto it = std::find_if(
        definitions.begin(),
        definitions.end(),
        [id](const ProductDefinition& definition) {
            return definition.id == id;
        });
    return it == definitions.end() ? nullptr : &(*it);
}

// 타입으로 조회: 타입을 ID로 캐스팅해 위임(ProductType 값 == ProductId).
const ProductDefinition* findProductDefinition(ProductType type) {
    return findProductDefinition(static_cast<ProductId>(type));
}

// 팩토리: ID에 해당하는 정의가 있으면 그 정의로 CatalogProduct 생성, 없으면 nullptr.
std::shared_ptr<Product> createProduct(ProductId id) {
    const auto* definition = findProductDefinition(id);
    if (definition == nullptr) {
        return nullptr;
    }
    return std::make_shared<CatalogProduct>(*definition);
}

std::shared_ptr<Product> createProduct(ProductType type) {
    return createProduct(static_cast<ProductId>(type));
}

} // namespace gactorio
