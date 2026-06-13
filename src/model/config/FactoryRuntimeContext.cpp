#include "model/config/FactoryRuntimeContext.hpp"

#include "model/config/FactoryConfigLoader.hpp"

#include <utility>

namespace gactorio::config_model {

FactoryRuntimeContext::FactoryRuntimeContext(FactoryConfig config)
    : config_(std::move(config)),
      registry_(config_) {}

const FactoryConfig& FactoryRuntimeContext::config() const noexcept {
    return config_;
}

const DefinitionRegistry& FactoryRuntimeContext::registry() const noexcept {
    return registry_;
}

FactoryRuntimeContext FactoryRuntimeContext::loadFromFile(const std::filesystem::path& path) {
    return FactoryRuntimeContext(FactoryConfigLoader::loadFromFile(path));
}

FactoryRuntimeContext FactoryRuntimeContext::loadFromString(std::string_view jsonText) {
    return FactoryRuntimeContext(FactoryConfigLoader::loadFromString(jsonText));
}

} // namespace gactorio::config_model
