#include "model/ProductionLine.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class TestProduct final : public gactorio::Product {
public:
    TestProduct(
        gactorio::ProductId id,
        std::string name,
        std::vector<gactorio::ProcessStep> route)
        : Product(
              std::move(id),
              "test_recipe",
              std::move(name),
              {},
              std::move(route)) {}

    gactorio::ProductId getProductId() const override { return storedProductId(); }
    const gactorio::ProductId& productId() const override { return storedProductIdRef(); }
    const gactorio::RecipeId& defaultRecipeId() const override { return storedDefaultRecipeId(); }
    const std::string& getName() const override { return storedName(); }
    const std::vector<gactorio::ItemRequirement>& getRequirements() const override {
        return storedRequirements();
    }
    const std::vector<gactorio::ProcessStep>& getRoute() const override { return storedRoute(); }
};

class TestMachine final : public gactorio::Machine {
public:
    TestMachine(
        gactorio::MachineId id,
        std::string name,
        std::string acceptedStepKind)
        : Machine(id, std::move(name), 1.0, 100.0, 0.0),
          acceptedStepKind_(std::move(acceptedStepKind)) {}

    std::string typeName() const override { return "Test Machine"; }
    std::string stationKind() const override { return acceptedStepKind_; }
    bool acceptsStep(std::string_view stepKind) const override {
        return stepKind == acceptedStepKind_;
    }
    gactorio::ProcessType processType() const override { return gactorio::ProcessType::Unknown; }
    gactorio::MachineRole role() const override { return gactorio::MachineRole::Unknown; }
    bool canAcceptRecipe(const gactorio::Recipe& recipe) const override {
        (void)recipe;
        return true;
    }

private:
    std::string acceptedStepKind_;
};

std::shared_ptr<gactorio::Product> makeProduct(
    std::string stepKind,
    double duration = 1.0) {
    std::vector<gactorio::ProcessStep> route;
    route.emplace_back(std::move(stepKind), duration);
    return std::make_shared<TestProduct>("test_product", "Test Product", std::move(route));
}

} // namespace

int main() {
    gactorio::ProductionLine legacyLine(1, "Legacy Line");
    legacyLine.addMachine(std::make_unique<gactorio::MixingStation>(1, "Mixer", 1.0, 100.0, 0.0));
    legacyLine.enqueueProduct(makeProduct("mixing"));
    legacyLine.assignAvailableTask();
    assert(legacyLine.machines()[0]->currentTask() != nullptr);
    assert(legacyLine.machines()[0]->currentTask()->currentStepKind() == "mixing");

    gactorio::ProductionLine customLine(2, "Custom Line");
    customLine.addMachine(std::make_unique<TestMachine>(2, "Flavor Station", "flavoring"));
    customLine.enqueueProduct(makeProduct("flavoring"));
    customLine.assignAvailableTask();
    assert(customLine.machines()[0]->currentTask() != nullptr);
    assert(customLine.machines()[0]->currentTask()->currentStepKind() == "flavoring");
    assert(customLine.machines()[0]->getStatus() == gactorio::MachineStatus::Working);

    gactorio::ProductionLine mismatchLine(3, "Mismatch Line");
    mismatchLine.addMachine(std::make_unique<TestMachine>(3, "Packager", "packaging"));
    mismatchLine.enqueueProduct(makeProduct("mixing"));
    mismatchLine.assignAvailableTask();
    assert(mismatchLine.machines()[0]->currentTask() == nullptr);
    assert(mismatchLine.machines()[0]->getStatus() == gactorio::MachineStatus::Idle);
    assert(mismatchLine.queueLength() == 1);

    return 0;
}
