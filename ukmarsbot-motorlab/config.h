/*
 * File: config.h
 * Project: mazerunner
 * File Created: Monday, 29th March 2021 11:04:59 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Thursday, 6th May 2021 9:17:32 am
 * Modified By: Peter Harrison
 * -----
 * MIT License
 *
 * Copyright (c) 2021 Peter Harrison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/***
 * The config.h file determines the actual robot variant that
 * will be the target for the build.
 *
 * This files lets you pick a specific robot that has its unique
 * configuration stored in that file. In this way, you can write
 * generic code that will work with a variety of different actual
 * robots. there are a number of example robot files in the project.
 * You should pick one that is closest to your setup, copy it and
 * then amend the details in that copy. Finally, add or modify
 * the selection settings below to use your new robot configuration.
 *
 */

/***
 * Start with the pinouts for the robot. These are the pin
 * definitions for the UKMARSBOT V1.x mainboard and should be
 * suitable for a number of derivatives.
 */

const int SERIAL_TX = 0;
const int SERIAL_RX = 1;
const int ENCODER_CLK = 3;
const int ENCODER_DIR = 5;
const int MOTOR_DIR = 8;
const int MOTOR_PWM = 10;
const int BATTERY_ADC_PIN = A7;
const int BATTERY_CHANNEL = 7;

/***
 * these are the defaults for some system-wide settings regardless of the robot
 * or environment. It would be best not to mess with these without good reason.
 * Sometimes the controller needs the interval, sometimes the frequency
 * define one and pre-calculate the other. The compiler does the work and no flash or
 * RAM storage is used. Constants are used for better type checking and traceability.
 */

const float LOOP_FREQUENCY = 500.0f;
const float LOOP_INTERVAL = (1.0f / LOOP_FREQUENCY);

/*************************************************************************/
/***
 * Since you may build for different physical robots, their characteristics
 * are kept in their own config files. Add you robot to the list and create
 * a corresponding config file with its custom values.
 *
 * If you have only one robot then you can reduce this section to a single
 * include line.
 */
#define ROBOT_MOTORLAB 99
#define ROBOT_MOTORLAB_UNLOADED 98

#define ROBOT_NAME ROBOT_MOTORLAB

// get the configuration for this setup
// it has stuff like encoder pulse and gear ratio

#if ROBOT_NAME == ROBOT_MOTORLAB
  #include "config-motorlab.h"
#elif ROBOT_NAME == ROBOT_MOTORLAB_UNLOADED
  #include "config-motorlab-unloaded.h"
#else
  #error "NO ROBOT DEFINED"
#endif

/*************************************************************************/
/*************************************************************************/

//***************************************************************************//

const float DEG_PER_COUNT = 360 / (ENCODER_PULSES * GEAR_RATIO);
const float COUNTS_PER_DEG = 1.0 / DEG_PER_COUNT;

const float KP = 16 * Tm / (Km * zeta * zeta * Td * Td);
const float KD = (8 * Tm - Td) / (Km * Td);

#endif
