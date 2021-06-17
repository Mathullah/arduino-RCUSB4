/* 
 * kekse23.de RCUSB
 * Copyright (c) 2019, Nicholas Regitz
 * 
 * Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 */

#include <Joystick.h>

class CAxis
{
public:
    typedef bool (*func_t)(void);

    uint16_t    Get(void);
    void        Isr(void);

    CAxis(const func_t GetPinState);
protected:

private:
    func_t      m_GetPinState;
    uint16_t    m_PulseTime;

    volatile int32_t   m_PositiveEdge;
    volatile int32_t   m_NegativeEdge;
    volatile int32_t   m_Value;

    volatile bool      m_NewValueIsAvailable;
};



const uint8_t ch1_input_pin(0);
const uint8_t ch2_input_pin(1);
const uint8_t ch3_input_pin(3);
const uint8_t ch4_input_pin(2);


Joystick_ Joystick( JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                    0, 0,                     // Button Count, Hat Switch Count
                    true, true, false,        // X, Y, Z
                    true, true, false,        // Rx, Ry, Rz
                    false, false,             // Rudder, Throttle
                    false, false, false);     // Accelerator, Brake, Steering


auto getCh1Input = []() { return static_cast<bool>(digitalRead(ch1_input_pin)); };
CAxis xAxis(getCh1Input);
auto getCh2Input = []() { return static_cast<bool>(digitalRead(ch2_input_pin)); };
CAxis yAxis(getCh1Input);


void setup()
{
/*
    Serial.begin(19200);

    while (!Serial) {
      ; // wait until connected
    }
*/
    /* Interrupt inputs shall always be tied to a defined state! */
    pinMode(ch1_input_pin, INPUT_PULLUP);
    pinMode(ch2_input_pin, INPUT_PULLUP);


    Joystick.begin();
    Joystick.setXAxisRange(2250, 750);
    Joystick.setYAxisRange(2250, 750);
    Joystick.setRxAxisRange(2250, 750);
    Joystick.setRyAxisRange(2250, 750);


    attachInterrupt(digitalPinToInterrupt(ch1_input_pin), []() { xAxis.Isr(); }, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ch2_input_pin), []() { yAxis.Isr(); }, CHANGE);

}


void loop()
{

    int16_t  Channel1(xAxis.Get());
    int16_t  Channel2(yAxis.Get());
    int16_t  Channel3(1500);
    int16_t  Channel4(1500);

    Joystick.setXAxis(Channel1);
    Joystick.setYAxis(Channel2);
    Joystick.setRxAxis(Channel3);
    Joystick.setRyAxis(Channel4);

    delay(10);


/*
  Serial.print("Output of channel 1: ");
  Serial.println(Channel1);

  Serial.print("Output of channel 2: ");
  Serial.println(Channel2);

  delay(1000);
  */

}


CAxis::CAxis(const func_t GetPinState) :
    m_GetPinState(GetPinState)
{

}

void CAxis::Isr(void)
{

    int32_t TimeStamp(micros());

    if (m_GetPinState()) 
    {
        m_PositiveEdge = TimeStamp;
    }
    else if (TimeStamp > m_PositiveEdge)
    {
        /* This is a simple mean value filter. But the division
           should not be done in an interrupt context.
        */
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
        m_PulseTime = static_cast<uint16_t>((LastPulseLength + m_Value) / 2);       // mean value
        m_NewValueIsAvailable = false;
    }
    return m_PulseTime;
}
