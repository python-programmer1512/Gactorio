#include "Machine.h"

Machine::Machine(int hp, int parallelSlots, int queueCapacity)
    : m_hp(hp)
    , m_maxHp(hp)
    , m_parallelSlots(parallelSlots)
    , m_queueCapacity(queueCapacity)
{
}
