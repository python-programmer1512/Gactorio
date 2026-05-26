#pragma once

namespace gactorio {

enum class FactoryCommandResult {
    Success,
    NotFound,
    InvalidRequest,
    UnknownProduct,
    InsufficientMaterials
};

} // namespace gactorio
