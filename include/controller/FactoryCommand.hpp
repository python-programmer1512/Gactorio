#pragma once

// =============================================================================
// FactoryCommandResult — 백엔드 컨트롤러 명령의 결과 코드
// -----------------------------------------------------------------------------
// gactorio::FactoryController 의 명령 메서드들이 반환하는 작은 enum.
// View-facing ctrl::Controller 는 이 값을 bool(성공 여부)로 변환해 JS 로 넘긴다.
// =============================================================================

namespace gactorio {

enum class FactoryCommandResult {
    Success,         // 정상 처리
    NotFound,        // 대상(기계/라인 등)을 찾지 못함
    InvalidRequest   // 요청 자체가 부적절(인자 오류, 상태 부적합 등)
};

} // namespace gactorio
