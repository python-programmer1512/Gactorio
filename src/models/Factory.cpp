#include "Factory.h"
#include "Stations.h"

Factory::Factory() {
    // 4개 스테이션을 순서대로 생성 (생산 라인 순서: 배합 → 품질 → 충전 → 포장)
    m_machines.push_back(std::make_unique<MixingStation>());
    m_machines.push_back(std::make_unique<QualityStation>());
    m_machines.push_back(std::make_unique<BottlingStation>());
    m_machines.push_back(std::make_unique<PackagingStation>());
}
