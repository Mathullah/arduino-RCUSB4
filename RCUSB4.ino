/*
 * RCUSB Interface
 * Modified version
 * Copyright (c) 2022, Mathias Felix
 *
 * kekse23.de RCUSB
 * Original version
 * Copyright (c) 2019, Nicholas Regitz
 *
 * Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 */

#include <Joystick.h>

class RcInput
{
public:
    RcInput() :
        m_PulseTime{},
        m_PositiveEdge{},
        m_NegativeEdge{},
        m_Value{},
        m_NewValueIsAvailable{}
    {

    }

    uint16_t    Get();
    void        Isr(uint8_t const Input, uint32_t const TimeStamp);

protected:
private:
    uint16_t         m_PulseTime;
    volatile int32_t m_PositiveEdge;
    volatile int32_t m_NegativeEdge;
    volatile int32_t m_Value;
    volatile bool    m_NewValueIsAvailable;
};


static constexpr uint8_t InputPinCh_1{0};
static constexpr uint8_t InputPinCh_2{1};
static constexpr uint8_t InputPinCh_3{3};
static constexpr uint8_t InputPinCh_4{2};

static constexpr uint16_t AxisUpperBound{2250U};
static constexpr uint16_t AxisLowerBound{750U};

Joystick_ Joystick( JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                    0, 0,                     // Button Count, Hat Switch Count
                    true, true, false,        // X, Y, Z
                    true, true, false,        // Rx, Ry, Rz
                    false, false,             // Rudder, Throttle
                    false, false, false);     // Accelerator, Brake, Steering

RcInput RcChannel_X;
RcInput RcChannel_Y;
RcInput RcChannel_Rx;
RcInput RcChannel_Ry;


void setup()
{
    pinMode(InputPinCh_1, INPUT_PULLUP);
    pinMode(InputPinCh_2, INPUT_PULLUP);
    pinMode(InputPinCh_3, INPUT_PULLUP);
    pinMode(InputPinCh_4, INPUT_PULLUP);

    Joystick.begin();

    Joystick.setXAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setYAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setRxAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setRyAxisRange(AxisUpperBound, AxisLowerBound);

    /* Config interrupts with corresponding callback functions (lambdas) */
    attachInterrupt(digitalPinToInterrupt(InputPinCh_1), []() {RcChannel_X.Isr(digitalRead(InputPinCh_1), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_2), []() {RcChannel_Y.Isr(digitalRead(InputPinCh_2), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_3), []() {RcChannel_Rx.Isr(digitalRead(InputPinCh_3), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_4), []() {RcChannel_Ry.Isr(digitalRead(InputPinCh_4), micros());}, CHANGE);
}


void loop()
{
    /* Get value from each channel and pass it to the joystick */
    Joystick.setXAxis(RcChannel_X.Get());
    Joystick.setYAxis(RcChannel_Y.Get());
    Joystick.setRxAxis(RcChannel_Rx.Get());
    Joystick.setRyAxis(RcChannel_Ry.Get());

    delay(10);

}

void RcInput::Isr(uint8_t const Input, uint32_t const TimeStamp)
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

uint16_t RcInput::Get()
{
    if (m_NewValueIsAvailable)
    {
        auto const LastPulseLength{m_PulseTime};
        m_PulseTime = static_cast<uint16_t>((LastPulseLength + m_Value) / 2);
        m_NewValueIsAvailable = false;
    }
    return m_PulseTime;
}
