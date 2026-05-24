#pragma once

#include <string>

// 4개 스테이션을 식별하는 키. JSON config의 "stationKey"와 일치.
enum class StationKey {
    MIXING,
    QUALITY,
    BOTTLING,
    PACKAGING
};

// Machine은 추상 기반 클래스. 구체 스테이션 클래스가 상속해서
// stationKey()와 displayName()을 override 한다 (Polymorphism).
class Machine {
public:
    Machine(int hp, int parallelSlots, int queueCapacity);
    virtual ~Machine() = default;

    // ---- 파생 클래스가 반드시 구현해야 하는 메서드 ----
    virtual StationKey stationKey() const = 0;
    virtual std::string displayName() const = 0;

    // ---- 공통 상태 접근자 ----
    int hp() const             { return m_hp; }
    int maxHp() const          { return m_maxHp; }
    int parallelSlots() const  { return m_parallelSlots; }
    int queueCapacity() const  { return m_queueCapacity; }
    int queueSize() const      { return m_queueSize; }
    int activeCount() const    { return m_activeCount; }
    bool isOn() const          { return m_isOn; }

    // ---- 상태 변경 ----
    void togglePower()         { m_isOn = !m_isOn; }
    void enqueue()             { if (m_queueSize < m_queueCapacity) ++m_queueSize; }
    void clearQueue()          { m_queueSize = 0; }

protected:
    int  m_hp;
    int  m_maxHp;
    int  m_parallelSlots;
    int  m_queueCapacity;
    int  m_queueSize    = 0;
    int  m_activeCount  = 0;
    bool m_isOn         = false;
};
