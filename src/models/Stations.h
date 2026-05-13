#pragma once

#include "Machine.h"

// 4개 구체 스테이션 — factory_config.json의 machines 항목과 동일한 스펙.
// 모두 Machine을 상속하여 stationKey()/displayName()을 override.

class MixingStation : public Machine {
public:
    MixingStation() : Machine(/*hp=*/150, /*parallelSlots=*/2, /*queueCapacity=*/8) {}
    StationKey stationKey() const override   { return StationKey::MIXING; }
    std::string displayName() const override { return "Mixing Station"; }
};

class QualityStation : public Machine {
public:
    QualityStation() : Machine(100, 3, 6) {}
    StationKey stationKey() const override   { return StationKey::QUALITY; }
    std::string displayName() const override { return "Quality Station"; }
};

class BottlingStation : public Machine {
public:
    BottlingStation() : Machine(120, 4, 10) {}
    StationKey stationKey() const override   { return StationKey::BOTTLING; }
    std::string displayName() const override { return "Bottling Station"; }
};

class PackagingStation : public Machine {
public:
    PackagingStation() : Machine(140, 2, 6) {}
    StationKey stationKey() const override   { return StationKey::PACKAGING; }
    std::string displayName() const override { return "Packaging Station"; }
};
