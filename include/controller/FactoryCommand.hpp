#pragma once

namespace gactorio {

enum class FactoryCommandResult {
    Success,
    NotFound,
    InvalidRequest,
    OverflowDropped
};

} // namespace gactorio

