/**
 * @file RCUSB4.ino
 * @author Mathias Felix
 * @brief RC to USB Interface main file, modified version
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

#include "src/RcPwm/RcPwm.hpp"
#include <Joystick.h>

static constexpr uint8_t InputPinCh_1{3};
static constexpr uint8_t InputPinCh_2{2};
static constexpr uint8_t InputPinCh_3{0};
static constexpr uint8_t InputPinCh_4{1};

static constexpr uint16_t AxisUpperBound{2250U};
static constexpr uint16_t AxisLowerBound{750U};

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                   0, 0,                 // Button Count, Hat Switch Count
                   true, true, false,    // X, Y, Z
                   true, true, false,    // Rx, Ry, Rz
                   false, false,         // Rudder, Throttle
                   false, false, false); // Accelerator, Brake, Steering

RcPwm RcChannel_X;
RcPwm RcChannel_Y;
RcPwm RcChannel_Rx;
RcPwm RcChannel_Ry;

void setup()
{
    pinMode(InputPinCh_1, INPUT_PULLUP);
    pinMode(InputPinCh_2, INPUT_PULLUP);
    pinMode(InputPinCh_3, INPUT_PULLUP);
    pinMode(InputPinCh_4, INPUT_PULLUP);

    Joystick.begin(false); // initAutoSendState not set

    Joystick.setXAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setYAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setRxAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setRyAxisRange(AxisUpperBound, AxisLowerBound);

    /* clang-format off */
    attachInterrupt(digitalPinToInterrupt(InputPinCh_1), []() {RcChannel_X.Isr(digitalRead(InputPinCh_1), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_2), []() {RcChannel_Y.Isr(digitalRead(InputPinCh_2), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_3), []() {RcChannel_Rx.Isr(digitalRead(InputPinCh_3), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_4), []() {RcChannel_Ry.Isr(digitalRead(InputPinCh_4), micros());}, CHANGE);
    /* clang-format on */
}

void loop()
{
    /* Get value from each channel and pass it to the joystick */
    Joystick.setXAxis(RcChannel_X.Get());
    Joystick.setYAxis(RcChannel_Y.Get());
    Joystick.setRxAxis(RcChannel_Rx.Get());
    Joystick.setRyAxis(RcChannel_Ry.Get());

    Joystick.sendState();

    delay(10);
}
