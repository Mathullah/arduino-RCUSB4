

#ifndef RC_PWM_H
#define RC_PWM_H

#if defined(AVR) && defined(ARDUINO_AVR_MICRO)
    #include <Arduino.h>
#else
    #include <cstdint>
#endif

class RcPwm
{
public:
    RcPwm() :
        m_PulseTime{},
        m_PositiveEdge{},
        m_Value{},
        m_NewValueIsAvailable{}
    {
    }

    uint16_t Get();
    void     Isr(uint8_t const Input, uint32_t const TimeStamp);

protected:

private:
    uint16_t         m_PulseTime;
    volatile int32_t m_PositiveEdge;
    volatile int16_t m_Value;
    volatile bool    m_NewValueIsAvailable;
};

#endif