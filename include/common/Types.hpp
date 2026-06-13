#pragma once

// =============================================================================
// Types.hpp — 프로젝트 전역에서 공유하는 "값 타입" 모음
// -----------------------------------------------------------------------------
// 이 헤더에는 클래스가 없습니다. 오직 다음 두 종류만 정의합니다.
//   1) ID 별칭(alias): 정수 하나로 무엇을 식별하는지 "이름"을 붙여 가독성/안전성 향상
//   2) enum class: 도메인에서 의미가 고정된 분류 값(아이템 종류, 제품 종류, 상태 등)
//
// 모든 선언은 gactorio 네임스페이스에 둡니다. gactorio:: 는 백엔드(Model+Controller)
// 전용 네임스페이스이며, View(JS)는 절대 이 타입들을 직접 보지 못합니다.
// (View가 보는 타입은 controller/Controller.h 의 ctrl:: 네임스페이스에 따로 있음)
// =============================================================================

#include <cstdint>   // std::uint64_t 등 고정폭 정수 타입

namespace gactorio {

// -----------------------------------------------------------------------------
// 시뮬레이션 시간: 초 단위 실수. delta time 누적/비교에 쓰이므로 double 사용.
using SimulationTime = double;

// -----------------------------------------------------------------------------
// ID 별칭들. 전부 64비트 부호없는 정수지만, 별칭으로 "이 숫자가 무엇의 ID인지"를
// 코드에서 드러냄. (컴파일러 수준의 강타입은 아니지만 가독성/문서화 목적.)
using ItemId           = std::uint64_t;   // 원자재 식별자
using ProductId        = std::uint64_t;   // 완제품 식별자
using RecipeId         = std::uint64_t;   // 레시피 식별자
using MachineId        = std::uint64_t;   // 기계 식별자
using LineId           = std::uint64_t;   // 생산라인 식별자
using ProductionLineId = LineId;          // LineId 의 동의어(가독성용 별칭)

// -----------------------------------------------------------------------------
// 에너지 드링크 공장 도메인 enum 들
// -----------------------------------------------------------------------------

// 원자재 종류. 값은 재고 맵의 키이자 View로 넘길 때 문자열 ID("1".."5")로도 쓰임.
enum class ItemType : std::uint64_t {
    Unknown     = 0,
    Ingredient  = 1,   // 카페인/타우린/당 혼합 원료
    Water       = 2,   // 정제수
    EmptyBottle = 3,   // 비어 있는 병(충전 대상)
    Label       = 4,   // 라벨 스티커
    Package     = 5    // 외포장(박스/슈링크랩)
};

// 완제품(에너지 드링크) 종류. 값(101~103)은 그대로 ProductId 로도 사용됨.
enum class ProductType : std::uint64_t {
    Unknown      = 0,
    VoltzClassic = 101,   // 기본형 (Voltz Classic)
    HyperBolt    = 102,   // 프리미엄 (Hyper Bolt)
    AuroraZero   = 103    // 무가당 스페셜 (Aurora Zero)
};

// 공정 종류. 제품 경로(ProcessStep)에서 "어떤 종류의 가공인지"를 표현.
enum class ProcessType {
    Unknown,
    Mixing,      // 원료 + 물 혼합/블렌딩
    Quality,     // 농도 검사 + 탄산 주입
    Bottling,    // 세척 + 충전 + 밀봉
    Packaging    // 라벨링 + 포장
};

// 기계 역할. 한 기계가 처리할 수 있는 공정 종류를 1:1로 나타냄.
// ProcessStep::requiredRole 과 Machine::role() 을 비교해 작업 배정 가능 여부 판단.
enum class MachineRole {
    Unknown,
    Mixing,
    Quality,
    Bottling,
    Packaging
};

// 기계 상태값(enum). State 패턴의 "상태 객체"(MachineState)와 별개로, 조회/DTO/메멘토
// 복원/가드 조건에 쓰기 위해 보관하는 스칼라 상태값.
// 주의: Working/Running, Error/Broken 은 같은 값을 가리키는 별칭(enum aliasing).
enum class MachineStatus {
    Idle,                  // 유휴: 작업/레시피 없음
    Running,
    Working = Running,     // 가동 중(= Running 과 동일 값)
    Paused,                // 일시정지(현재 구현은 Idle로 환원)
    Blocked,               // 대기/막힘(예약 값)
    Error,
    Broken = Error,        // 고장(HP 0, = Error 와 동일 값)
    Maintenance            // 정비 중(수리 진행)
};

// 시뮬레이션 이벤트 종류. Observer(EventLog/Statistics)가 이 값으로 분기 처리.
enum class EventType {
    Info,
    TaskEnqueued,      // 작업이 라인 큐에 등록됨
    TaskStarted,       // 기계가 작업을 시작/재개함
    StepCompleted,     // 한 공정 단계 완료
    ProductCompleted,  // 한 제품(전 공정) 완료
    MachineBroken,     // 기계 고장 발생
    MachineRepaired,   // 기계 수리 완료
    StateChanged,      // 상태 전이 발생
    InputsConsumed,    // 원자재 소비됨(예약)
    MachinePaused      // 기계 일시정지됨
};

} // namespace gactorio
