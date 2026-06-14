#include "controller/ControllerConfigIdAdapters.hpp"
#include "model/config/ConfigIdAdapters.hpp"

#include <cassert>
#include <stdexcept>

namespace {

template <typename Fn>
bool throwsInvalidArgument(Fn&& fn) {
    try {
        fn();
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

} // namespace

int main() {
    using namespace gactorio;
    using namespace gactorio::config_model;

    assert(toItemId(ItemType::Ingredient) == "ingredient");
    assert(itemTypeFromId("ingredient").value() == ItemType::Ingredient);
    assert(!itemTypeFromId("ingredient_x").has_value());

    assert(toProductId(ProductType::VoltzClassic) == "voltz_classic");
    assert(productTypeFromId("voltz_classic").value() == ProductType::VoltzClassic);
    assert(!productTypeFromId("voltz_x").has_value());

    assert(toStationKind(MachineRole::Bottling) == "bottling");
    assert(toStepKind(MachineRole::Packaging) == "packaging");
    assert(machineRoleFromKind("quality").value() == MachineRole::Quality);
    assert(!machineRoleFromKind("quality_x").has_value());

    assert(toStationKind(ProcessType::Mixing) == "mixing");
    assert(toStepKind(ProcessType::Quality) == "quality");
    assert(processTypeFromKind("packaging").value() == ProcessType::Packaging);
    assert(!processTypeFromKind("packaging_x").has_value());

    assert(ctrl::toProductId(ctrl::ProductKind::HyperBolt) == "hyper_bolt");
    assert(ctrl::productKindFromId("aurora_zero").value() == ctrl::ProductKind::AuroraZero);
    assert(!ctrl::productKindFromId("aurora_x").has_value());

    assert(throwsInvalidArgument([] { (void)toItemId(ItemType::Unknown); }));
    assert(throwsInvalidArgument([] { (void)toProductId(ProductType::Unknown); }));
    assert(throwsInvalidArgument([] { (void)toStationKind(MachineRole::Unknown); }));
    assert(throwsInvalidArgument([] { (void)toStepKind(ProcessType::Unknown); }));
    assert(throwsInvalidArgument([] { (void)ctrl::toProductId(ctrl::ProductKind::Unknown); }));

    return 0;
}
