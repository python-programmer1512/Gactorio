#pragma once

#include <optional>
#include <string>

namespace gactorio {

enum class ScenarioType {
    NormalFlow,
    RandomBreakdowns,
    Bottleneck,
    Overflow
};

std::string scenarioTypeToString(ScenarioType scenario);
std::optional<ScenarioType> scenarioTypeFromString(const std::string& value);
std::string scenarioTypeToDisplayName(ScenarioType scenario);

} // namespace gactorio
