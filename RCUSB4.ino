/* 
 * RCUSB Interface
 * Modified version
 * Copyright (c) 2021, Mathias Felix
 * 
 * kekse23.de RCUSB
 * Original version
 * Copyright (c) 2019, Nicholas Regitz
 * 
 * Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 */

#include <Joystick.h>

class CAxis
{
public:
    uint16_t    Get(void);
    void        Isr(const uint8_t Input, const uint32_t timestamp);

protected:
private:
    uint16_t         m_PulseTime;
    volatile int32_t m_PositiveEdge;
    volatile int32_t m_NegativeEdge;
    volatile int32_t m_Value;
    volatile bool    m_NewValueIsAvailable;
};


const uint8_t InputCH1(0);
const uint8_t InputCH2(1);
const uint8_t InputCH3(3);
const uint8_t InputCH4(2);

Joystick_ Joystick( JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                    0, 0,                     // Button Count, Hat Switch Count
                    true, true, false,        // X, Y, Z
                    true, true, false,        // Rx, Ry, Rz
                    false, false,             // Rudder, Throttle
                    false, false, false);     // Accelerator, Brake, Steering

CAxis xAxis;
CAxis yAxis;
CAxis RxAxis;
CAxis RyAxis;


void setup()
{
    pinMode(InputCH1, INPUT_PULLUP);
    pinMode(InputCH2, INPUT_PULLUP);
    pinMode(InputCH3, INPUT_PULLUP);
    pinMode(InputCH4, INPUT_PULLUP);

    Joystick.begin();
    Joystick.setXAxisRange(2250, 750);
    Joystick.setYAxisRange(2250, 750);
    Joystick.setRxAxisRange(2250, 750);
    Joystick.setRyAxisRange(2250, 750);

    attachInterrupt(digitalPinToInterrupt(InputCH1), []() {xAxis.Isr(digitalRead(InputCH1), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputCH2), []() {yAxis.Isr(digitalRead(InputCH2), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputCH3), []() {RxAxis.Isr(digitalRead(InputCH3), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputCH4), []() {RyAxis.Isr(digitalRead(InputCH4), micros());}, CHANGE);
}


void loop()
{

    int16_t  Channel1(xAxis.Get());
    int16_t  Channel2(yAxis.Get());
    int16_t  Channel3(RxAxis.Get());
    int16_t  Channel4(RyAxis.Get());

    Joystick.setXAxis(Channel1);
    Joystick.setYAxis(Channel2);
    Joystick.setRxAxis(Channel3);
    Joystick.setRyAxis(Channel4);

    delay(10);

}

void CAxis::Isr(const uint8_t Input, const uint32_t timestamp)
{
    int32_t TimeStamp(timestamp);

    if (static_cast<bool>(Input)) 
    {
        m_PositiveEdge = TimeStamp;
    }
    else if (TimeStamp > m_PositiveEdge)
    {
        m_Value = (m_Value + (TimeStamp - m_PositiveEdge)) / 2;
        m_NewValueIsAvailable = true;
    } else
    {
        // nothing to do.
    }
}

uint16_t CAxis::Get(void)
{
    if (m_NewValueIsAvailable)
    {
        const int32_t LastPulseLength(m_PulseTime);
        m_PulseTime = static_cast<uint16_t>((LastPulseLength + m_Value) / 2);
        m_NewValueIsAvailable = false;
    }
    return m_PulseTime;
}
