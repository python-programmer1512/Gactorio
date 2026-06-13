#pragma once

#include "model/config/FactoryConfig.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gactorio::config_model {

class FactoryConfigError : public std::runtime_error {
public:
    explicit FactoryConfigError(const std::string& message);
};

class FactoryConfigLoader {
public:
    static FactoryConfig loadFromFile(const std::filesystem::path& path);
    static FactoryConfig loadFromString(std::string_view jsonText);
};

} // namespace gactorio::config_model
