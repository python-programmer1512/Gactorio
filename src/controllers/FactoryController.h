#pragma once

#include "../models/Factory.h"

#include <cstddef>

// FactoryController는 View와 Factory(Model) 사이의 얇은 위임 레이어.
// 구현이 모두 한 줄짜리라 헤더 전용 (.cpp 불필요).
class FactoryController {
public:
    explicit FactoryController(Factory& factory) : m_factory(factory) {}

    // ---- 읽기 (View가 화면에 표시할 데이터) ----
    std::size_t machineCount() const            { return m_factory.machineCount(); }
    const Machine& machineAt(std::size_t i) const { return m_factory.machineAt(i); }

    // ---- 쓰기 (View의 버튼 클릭 등이 호출) ----
    void onTogglePower(std::size_t i) { m_factory.machineAt(i).togglePower(); }
    void onEnqueue(std::size_t i)     { m_factory.machineAt(i).enqueue(); }
    void onClearQueue(std::size_t i)  { m_factory.machineAt(i).clearQueue(); }

private:
    Factory& m_factory;
};
