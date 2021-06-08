/* 
 * kekse23.de RCUSB
 * Copyright (c) 2019, Nicholas Regitz
 * 
 * Diese Datei ist Lizensiert unter der Creative Commons 4.0 CC BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
 */

#include <Joystick.h>
#include "AVRPort23.h"

class CAxis
{
public:
    uint16_t Get(void);


protected:

private:
    void        (*m_ReadPin)(void);
    uint16_t    m_Value;
    bool        m_NewValueIsAvailable;
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


CAxis xAxis;

volatile unsigned long Time[4];
volatile unsigned int Value[4];
volatile bool ValChanged[4];
unsigned int NewValue[4];


void setup()
{

    pinMode(ch1_input_pin, INPUT);
    pinMode(ch2_input_pin, INPUT);
    pinMode(ch3_input_pin, INPUT);
    pinMode(ch4_input_pin, INPUT);

    Joystick.begin();
    Joystick.setXAxisRange(2250, 750);
    Joystick.setYAxisRange(2250, 750);
    Joystick.setRxAxisRange(2250, 750);
    Joystick.setRyAxisRange(2250, 750);
  
    attachInterrupt(digitalPinToInterrupt(ch1_input_pin), isr1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ch2_input_pin), isr2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ch3_input_pin), isr3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ch4_input_pin), isr4, CHANGE);

}



void loop()
{   
    if (ValChanged[0])
    {
        /* Another filter function - Why filtering twice? */
        NewValue[0] = (NewValue[0] + Value[0]) / 2;

        Joystick.setXAxis(NewValue[0]);
        ValChanged[0] = false;
    }

  if (ValChanged[1])
  {
    NewValue[1] = (NewValue[1]+Value[1])/2;
    Joystick.setYAxis(NewValue[1]);
    ValChanged[1] = false;
  }
  
   if (ValChanged[2])
  {
    NewValue[2] = (NewValue[2]+Value[2])/2;
    Joystick.setRxAxis(NewValue[2]);
    ValChanged[2] = false;
  }
  
   if (ValChanged[3])
  {
    NewValue[3] = (NewValue[3]+Value[3])/2;
    Joystick.setRyAxis(NewValue[3]);
    ValChanged[3] = false;
  }
  
  delay(8);
}

void isr1(void)
{
    uint32_t TimeStamp(micros());

    if (static_cast<bool>(digitalRead(ch1_input_pin)))    // assuming positive edge
    {
        Time[0] = TimeStamp;
    }
    else if (TimeStamp > Time[0])   // assuming negative edge
    {
        /* This is a simple mean value filter. But the division
           should not be done in an interrupt context.
           TimeStamp - Time[0] represent the pulse width
        */
        Value[0] = (Value[0] + (TimeStamp - Time[0])) / 2;

        ValChanged[0] = true;
    } else
    {
        // nothign to do (positive edge and negative edge in t)
    }
}

void isr2()
{
  if (static_cast<bool>(digitalRead(ch2_input_pin))) Time[1] = micros();
  else if (micros() > Time[1])
  {
    Value[1] = (Value[1]+(micros()-Time[1]))/2;
    ValChanged[1] = true;
  }
}

void isr3()
{
  if (static_cast<bool>(digitalRead(ch3_input_pin))) Time[2] = micros();
  else if (micros() > Time[2])
  {
    Value[2] = (Value[2]+(micros()-Time[2]))/2;
    ValChanged[2] = true;
  }
}

void isr4()
{
  if (static_cast<bool>(digitalRead(ch4_input_pin))) Time[3] = micros();
  else if (micros() > Time[3])
  {
    Value[3] = (Value[3]+(micros()-Time[3]))/2;
    ValChanged[3] = true;
  }
}
