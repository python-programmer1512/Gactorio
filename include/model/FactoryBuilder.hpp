#pragma once

#include "model/Factory.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <filesystem>
#include <memory>
#include <string_view>

namespace gactorio {

class FactoryBuilder {
public:
    static std::unique_ptr<Factory> createFactory(
        const config_model::FactoryRuntimeContext& context);

    static std::unique_ptr<Factory> createFactoryFromConfigFile(
        const std::filesystem::path& path);

    static std::unique_ptr<Factory> createFactoryFromConfigString(
        std::string_view jsonText);
};

} // namespace gactorio
