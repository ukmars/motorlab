/******************************************************************************
 * Project: <<project>>                                                            *
 * File:    config-motorlab.h                                                      *
 * File Created: Saturday, 3rd December 2022 8:57:01 am                            *
 * Author: Peter Harrison                                                          *
 * -----                                                                           *
 * Last Modified: Sunday, 11th December 2022 12:01:43 am                      *
 * -----                                                                           *
 * Copyright 2022 - 2022 Peter Harrison, Micromouseonline                          *
 * -----                                                                           *
 * Licence:                                                                        *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2022 Peter Harrison                                               *
 *                                                                                 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of *
 * this software and associated documentation files (the "Software"), to deal in   *
 * the Software without restriction, including without limitation the rights to    *
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies   *
 * of the Software, and to permit persons to whom the Software is furnished to do  *
 * so, subject to the following conditions:                                        *
 *                                                                                 *
 * The above copyright notice and this permission notice shall be included in all  *
 * copies or substantial portions of the Software.                                 *
 *                                                                                 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
 * SOFTWARE.                                                                       *
 ***********************************************************************************/

#ifndef MOTORLAB_H
#define MOTORLAB_H

#include <Arduino.h>

const uint32_t BAUDRATE = 115200;
const float MAX_MOTOR_VOLTS = 6.0;

//***************************************************************************//

// time between logged lined when reporting is enabled (milliseconds)
const int REPORTING_INTERVAL = 10;

//** STEP 2 DRIVETRAIN ******************************************************//
// Enter these before starting
// STEP 2A
const float GEAR_RATIO = 9.966;
const float ENCODER_PULSES = 12.0;

// encoder polarity is either 1 or -1 and is used to account for reversal of the encoder phases
#define ENCODER_POLARITY (-1)

// similarly, the motors may be wired with different polarity and that is defined here so that
// setting a positive voltage always moves the robot forwards
#define MOTOR_POLARITY (-1)

//*** BATTERY ***************************************************************//
// Battery resistor bridge //Derek Hall//
// The battery measurement is performed by first reducing the battery voltage
// with a potential divider formed by two resistors. Here they are named R1 and R2
// though that may not be their designation on the schematics.
//
// Resistor R1 is the high-side resistor and connects to the battery supply
// Resistor R2 is the low-side resistor and connects to ground
// Battery voltage is measured at the junction of these resistors
// The ADC port used for the conversion will have a full scale reading (FSR) that
// depends on the device being used. Typically that will be 1023 for a 10-bit ADC as
// found on an Arduino but it may be 4095 if you have a 12-bit ADC.
// Finally, the ADC converter on your processor will have a reference voltage. On
// the Arduinos for example, this is 5 Volts. Thus, a full scale reading of
// 1023 would represent 5 Volts, 511 would be 2.5Volts and so on.
//
// in this section you can enter the appropriate values for your ADC and potential
// divider setup to ensure that the battery voltage reading performed by the sensors
// is as accurate as possible.
//
// By calculating the battery multiplier here, you can be sure that the actual
// battery voltage calulation is done as efficiently as possible.
// The compiler will do all these calculations so your program does not have to.

const float BATTERY_R1 = 10000.0; // resistor to battery +
const float BATTERY_R2 = 10000.0; // resistor to Gnd
const float BATTERY_DIVIDER_RATIO = BATTERY_R2 / (BATTERY_R1 + BATTERY_R2);
const float ADC_FSR = 1023.0;    // The maximum reading for the ADC
const float ADC_REF_VOLTS = 5.0; // Reference voltage of ADC

const float BATTERY_MULTIPLIER = (ADC_REF_VOLTS / ADC_FSR / BATTERY_DIVIDER_RATIO);

//*** MOTION CONTROL CONSTANTS **********************************************//

// Motor Feedforward
/***
 * Speed Feedforward is used to add a drive voltage proportional to the motor speed
 * The units are Volts per mm/s and the value will be different for each
 * robot where the motor + gearbox + wheel diamter + robot weight are different
 * You can experimentally determine a suitable value by turning off the controller
 * and then commanding a set voltage to the motors. The same voltage is applied to
 * each motor. Have the robot report its speed regularly or have it measure
 * its steady state speed after a period of acceleration.
 * Do this for several applied voltages from 0.5 Volts to 5 Volts in steps of 0.5V
 * Plot a chart of steady state speed against voltage. The slope of that graph is
 * the speed feedforward, SPEED_FF.
 * Note that the line will not pass through the origin because there will be
 * some minimum voltage needed just to ovecome friction and get the wheels to turn at all.
 * That minimum voltage is the BIAS_FF.
 */
const float Tm = 0.325;
const float Km = 2064.7;
const float SPEED_FF = 1.0 / Km;
const float ACC_FF = Tm / Km;
const float BIAS_FF = 0.145;
const float TOP_SPEED = (6.0 - BIAS_FF) / SPEED_FF;

// profile motion controller constants
/***
 * The PD controller is u = Kp * e + Kd*(e - e_old)
 * But this is a sampled system so the error change is scaled by the sample time
 * Rather than scale it back in the control loop, we pre-multiply
 * Kd by the LOOP_FREQUENCY
 */

const float zeta = 0.707;
const float Td = Tm / 2;

#endif