#include "common/ScenarioType.hpp"

namespace gactorio {

std::string scenarioTypeToString(ScenarioType scenario) {
    switch (scenario) {
    case ScenarioType::NormalFlow:
        return "normal-flow";
    case ScenarioType::RandomBreakdowns:
        return "random-breakdowns";
    case ScenarioType::Bottleneck:
        return "bottleneck";
    case ScenarioType::Overflow:
        return "overflow";
    }
    return "normal-flow";
}

std::optional<ScenarioType> scenarioTypeFromString(const std::string& value) {
    if (value == "normal-flow") {
        return ScenarioType::NormalFlow;
    }
    if (value == "random-breakdowns") {
        return ScenarioType::RandomBreakdowns;
    }
    if (value == "bottleneck") {
        return ScenarioType::Bottleneck;
    }
    if (value == "overflow") {
        return ScenarioType::Overflow;
    }
    return std::nullopt;
}

std::string scenarioTypeToDisplayName(ScenarioType scenario) {
    switch (scenario) {
    case ScenarioType::NormalFlow:
        return "Normal Flow";
    case ScenarioType::RandomBreakdowns:
        return "Random Breakdowns";
    case ScenarioType::Bottleneck:
        return "Bottleneck";
    case ScenarioType::Overflow:
        return "Overflow";
    }
    return "Normal Flow";
}

} // namespace gactorio
