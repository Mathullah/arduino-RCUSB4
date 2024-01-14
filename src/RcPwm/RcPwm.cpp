/**
 * @file RcPwm.cpp
 * @author Mathias Felix
 * @brief RC to USB Interface RC PWM class
 *
 *        Original version by kekse23.de RCUSB
 *        Copyright (c) 2019, Nicholas Regitz
 *
 *        Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 *        https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "RcPwm.hpp"
#include <util/atomic.h>

void RcPwm::Isr(uint8_t const Input, uint32_t const TimeStamp)
{
    auto const Time{static_cast<int32_t>(TimeStamp)};

    if (static_cast<bool>(Input)) // capture positive edge
    {
        m_PositiveEdge = Time;
    } else if (Time > m_PositiveEdge) // capture negative edge
    {
        m_Value               = static_cast<int16_t>(Time - m_PositiveEdge);
        m_NewValueIsAvailable = true;
    } else
    {
        // nothing to do.
    }
}

uint16_t RcPwm::Get()
{
    bool     IsAvailable{};
    uint16_t Pulse{};

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        IsAvailable           = m_NewValueIsAvailable;
        m_NewValueIsAvailable = false;
        Pulse                 = m_Value;
    }

    if (IsAvailable)
    {
        m_PulseTime = static_cast<uint16_t>((m_PulseTime + Pulse) / 2);
    }

    return m_PulseTime;
}
