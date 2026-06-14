#pragma once

#include "model/config/DefinitionRegistry.hpp"
#include "model/config/FactoryConfig.hpp"

#include <filesystem>
#include <string_view>

namespace gactorio::config_model {

class FactoryRuntimeContext {
public:
    explicit FactoryRuntimeContext(FactoryConfig config);

    FactoryRuntimeContext(const FactoryRuntimeContext&) = delete;
    FactoryRuntimeContext& operator=(const FactoryRuntimeContext&) = delete;
    FactoryRuntimeContext(FactoryRuntimeContext&&) = delete;
    FactoryRuntimeContext& operator=(FactoryRuntimeContext&&) = delete;

    const FactoryConfig& config() const noexcept;
    const DefinitionRegistry& registry() const noexcept;

    static FactoryRuntimeContext loadFromFile(const std::filesystem::path& path);
    static FactoryRuntimeContext loadFromString(std::string_view jsonText);

private:
    FactoryConfig config_;
    DefinitionRegistry registry_;
};

} // namespace gactorio::config_model
