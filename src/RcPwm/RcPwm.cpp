
#include "RcPwm.hpp"



void RcPwm::Isr(uint8_t const Input, uint32_t const TimeStamp)
{
    auto const Time{static_cast<int32_t>(TimeStamp)};

    if (static_cast<bool>(Input)) // capture positive edge
    {
        m_PositiveEdge = Time;
    }
    else if (Time > m_PositiveEdge)  // capture negative edge (at least 1us later)
    {
        m_Value = (m_Value + (Time - m_PositiveEdge)) / 2;
        m_NewValueIsAvailable = true;
    } else
    {
        // nothing to do.
    }
}

uint16_t RcPwm::Get()
{
    if (m_NewValueIsAvailable)
    {
        auto const LastPulseLength{m_PulseTime};
        m_PulseTime = static_cast<uint16_t>((LastPulseLength + m_Value) / 2);
        m_NewValueIsAvailable = false;
    }
    return m_PulseTime;
}
