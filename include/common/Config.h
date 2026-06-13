#pragma once

// =============================================================================
// Config.h — 시뮬레이션 튜닝 상수 (자동 생성 파일)
// -----------------------------------------------------------------------------
// AUTO-GENERATED from data/factory_config.json — 손으로 수정하지 말 것.
// 재생성:   python tools/gen_config.py
//
// 의도: "마법의 숫자(magic number)"를 코드 곳곳에 흩지 않고 한 곳(JSON)에서 관리한다.
//       JSON 을 고치고 스크립트를 돌리면 이 헤더의 constexpr 상수가 갱신된다.
//       Machine 의 고장/수리 로직과 Product 경로 시간이 이 값들을 참조한다.
// =============================================================================

namespace gactorio::config {

inline constexpr double kInitialHealth             = 100.0; // 기계 최대 HP
inline constexpr double kDamageChancePerSecond     = 0.5;   // 초당 손상 발생 확률 계수
inline constexpr double kDamageMinHp               = 1.0;   // 1회 손상 최소 HP
inline constexpr double kDamageMaxHp               = 5.0;   // 1회 손상 최대 HP
inline constexpr double kIncrementalRepairHp       = 5.0;   // Repair 버튼 1회 회복 HP
inline constexpr double kRepairAllDelaySeconds     = 3.0;   // 전체수리(정비) 소요 시간(초)

// 아래는 제품별 공정 경로 소요시간(초). data/factory_config.json 에서 생성됨.
inline constexpr double kProductVoltzClassicTotalTime = 40.0;
inline constexpr double kProductVoltzClassicMixingTime = 13.0;
inline constexpr double kProductVoltzClassicQualityTime = 9.0;
inline constexpr double kProductVoltzClassicBottlingTime = 9.0;
inline constexpr double kProductVoltzClassicPackagingTime = 9.0;
inline constexpr double kProductHyperBoltTotalTime = 48.0;
inline constexpr double kProductHyperBoltMixingTime = 18.0;
inline constexpr double kProductHyperBoltQualityTime = 12.0;
inline constexpr double kProductHyperBoltBottlingTime = 9.0;
inline constexpr double kProductHyperBoltPackagingTime = 9.0;
inline constexpr double kProductAuroraZeroTotalTime = 49.0;
inline constexpr double kProductAuroraZeroMixingTime = 17.0;
inline constexpr double kProductAuroraZeroQualityTime = 12.0;
inline constexpr double kProductAuroraZeroBottlingTime = 9.0;
inline constexpr double kProductAuroraZeroPackagingTime = 11.0;

} // namespace gactorio::config
