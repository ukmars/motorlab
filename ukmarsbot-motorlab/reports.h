/*
 * File: logger.h
 * Project: mazerunner
 * File Created: Tuesday, 23rd March 2021 10:18:19 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Wednesday, 14th April 2021 4:36:51 pm
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

#ifndef LOGGER_H
#define LOGGER_H

#include "reports.h"
#include "src/encoders.h"
#include "src/motors.h"
#include "src/profile.h"
#include "src/utils.h"
#include <Arduino.h>

class Reporter;
extern Reporter reporter;
class Reporter {

  uint32_t s_start_time;
  uint32_t s_report_time;
  uint32_t s_report_interval = REPORTING_INTERVAL;

public:
  // note that the Serial device has a 64 character buffer and, at 115200 baud
  // 64 characters will take about 6ms to go out over the wire.

  /**
   * The profile reporter will send out a table of space separated
   * data so that the results can be saved to a file or imported to
   * a spreadsheet or other analyss software.
   *
   * Always send the header first because that restarts the elapsed
   * time count.
   *
   * The data includes
   *   time        - in milliseconds since the header was sent
   *   robotPos    - position in mm as reported by the encoders
   *   robotAngle  - angle in degrees as reported by the encoders
   *   fwdPos      - profile profiler setpoint in mm
   *   fwdSpeed    - profile profiler current speed in mm/s
   *   rotpos      - rotation profiler setpoint in deg
   *   rotSpeed    - rotation profiler current speed in deg/s
   *   fwdmVolts    - voltage sent to the motors for profile control
   *   rotmVolts    - voltage sent to the motors for rotation control
   *
   */
  void report_profile_header() {
    Serial.println(F("$time robotPos robotAngle fwdPos  fwdSpeed rotpos rotSpeed fwdmVolts rotmVolts"));
    s_start_time = millis();
    s_report_time = s_start_time;
  }

  void report_profile() {
    if (millis() >= s_report_time) {
      s_report_time += s_report_interval;
      print_justified(int(millis() - s_start_time), 6);
      print_justified(int(encoders.robot_distance()), 6);
      print_justified(int(profile.position()), 6);
      print_justified(int(profile.speed()), 6);
      print_justified(motors.get_fwd_millivolts(), 6);
      Serial.println();
    }
  }

  /**
   * The controller reporter will send out a table of space separated
   * data so that the results can be saved to a file or imported to
   * a spreadsheet or other analyss software.
   *
   * Always send the header first because that restarts the elapsed
   * time count.
   *
   * The data includes
   *   time        - in milliseconds since the header was sent
   *   robotPos    - position in mm as reported by the encoders
   *   robotAngle  - angle in degrees as reported by the encoders
   *   fwdPos      - profile profiler setpoint in mm
   *   fwdSpeed    - profile profiler current speed in mm/s
   *   rotpos      - rotation profiler setpoint in deg
   *   rotSpeed    - rotation profiler current speed in deg/s
   *   fwdmVolts    - voltage sent to the motors for profile control
   *   rotmVolts    - voltage sent to the motors for rotation control
   *
   */
  void report_controller_header() {
    Serial.println(F("$time set_pos robot_pos set_speed robot_speed ctrl_volts ff_volts, motor_volts"));
    s_start_time = millis();
    s_report_time = s_start_time;
  }

  void report_controller(Profile &profile) {

    float setPos = profile.position();
    float robot_pos = encoders.robot_distance();
    float setSpeed = profile.speed();
    float robot_speed = encoders.robot_speed();
    float ctrl_volts = motors.m_ctrl_volts;
    float ff_volts = motors.m_ff_volts;
    float motor_volts = motors.m_motor_volts;
    ;

    Serial.print(millis() - s_start_time);
    Serial.print(' ');
    Serial.print(setPos);
    Serial.print(' ');
    Serial.print(robot_pos);
    Serial.print(' ');
    Serial.print(setSpeed);
    Serial.print(' ');
    Serial.print(robot_speed);
    Serial.print(' ');
    Serial.print(ctrl_volts);
    Serial.print(' ');
    Serial.print(ff_volts);
    Serial.print(' ');
    Serial.print(motor_volts);
    Serial.print(' ');
    Serial.println();
  }
};

#endif