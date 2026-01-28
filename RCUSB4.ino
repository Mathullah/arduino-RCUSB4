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

static constexpr uint16_t AxisUpperBound{2100U};
static constexpr uint16_t AxisLowerBound{900U};

// Create the Joystick object with X and Y axis enabled and 2 buttons
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK,
                   2U,     // Button count
                   0U,     // Hat switch count
                   true,   // X axis
                   true,   // Y axis
                   false,  // Z axis
                   false,  // Rx axis
                   false,  // Ry axis
                   false,  // Rz axis
                   false,  // rudder
                   false,  // throttle
                   false,  // accelerator
                   false,  // brake
                   false); // steering


RcPwm RcChannel_X;
RcPwm RcChannel_Y;
RcPwm RcChannel_3;
RcPwm RcChannel_4;

void setup()
{
    pinMode(InputPinCh_1, INPUT_PULLUP);
    pinMode(InputPinCh_2, INPUT_PULLUP);
    pinMode(InputPinCh_3, INPUT_PULLUP);
    pinMode(InputPinCh_4, INPUT_PULLUP);

    Joystick.begin(false); // initAutoSendState not set

    Joystick.setXAxisRange(AxisUpperBound, AxisLowerBound);
    Joystick.setYAxisRange(AxisUpperBound, AxisLowerBound);

    /* clang-format off */
    attachInterrupt(digitalPinToInterrupt(InputPinCh_1), []() {RcChannel_X.Isr(digitalRead(InputPinCh_1), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_2), []() {RcChannel_Y.Isr(digitalRead(InputPinCh_2), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_3), []() {RcChannel_3.Isr(digitalRead(InputPinCh_3), micros());}, CHANGE);
    attachInterrupt(digitalPinToInterrupt(InputPinCh_4), []() {RcChannel_4.Isr(digitalRead(InputPinCh_4), micros());}, CHANGE);
    /* clang-format on */
}

void loop()
{
    /* Get value from each channel and pass it to the joystick. */
    Joystick.setXAxis(RcChannel_X.Get());
    Joystick.setYAxis(RcChannel_Y.Get());

    static uint8_t buttonState_Ch3 = 0U;
    static uint8_t buttonState_Ch4 = 0U;

    auto const Channel3 {RcChannel_3.Get()};
    buttonState_Ch3 = (Channel3 > 1800U) ? 1U : (Channel3 < 1600U) ? 0U : buttonState_Ch3;
    Joystick.setButton(0, buttonState_Ch3);

    auto const Channel4 {RcChannel_4.Get()};
    buttonState_Ch4 = (Channel4 > 1800U) ? 1U : (Channel4 < 1600U) ? 0U : buttonState_Ch4;
    Joystick.setButton(1, buttonState_Ch4);

    Joystick.sendState();

    delay(10);
}
