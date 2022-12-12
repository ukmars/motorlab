/*
 * File: mouse.h
 * Project: mazerunner
 * File Created: Friday, 23rd April 2021 9:09:16 am
 * Author: Peter Harrison
 * -----
 * Last Modified: Monday, 26th April 2021 10:42:44 pm
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

#pragma once

#include "Arduino.h"
#include "config.h"
#include "reports.h"
#include "src/adc.h"
#include "src/encoders.h"
#include "src/motors.h"
#include "src/profile.h"
#include "src/settings.h"
#include "src/types.h"
#include "src/utils.h"

class Robot;
extern Robot robot;

class Robot {
public:
  // this is only used to have a trial result survice reset
  float last_result;

  Robot() {
    init();
  }

  void init() {
    reset_drive();
  }

  /**
   * Before the robot begins a sequence of moves, this method can be used to
   * make sure everything starts off in a known state.
   *
   * @brief Reset profiles, counters and controllers. Motors off.
   */
  void reset_drive() {
    motors.stop();
    encoders.reset();
    profile.reset();
    motors.reset_controllers();
  }

  void enable_drive() {
    reset_drive();
    motors.enable_controllers();
  }

  void disable_drive() {
    reset_drive();
    motors.disable_controllers();
  }

  void show_encoders(const Args &args) {
    encoders.reset();
    Serial.println(F("Encoders:"));
    // something to get encoder count for one turn
  }

  float battery_voltage() {
    return adc.get_battery_voltage();
  }

  void do_open_loop_trial(const Args &args) {
    uint32_t endTime = 2000;
    float volts = 3;
    if (args.argc > 1) {
      volts = atoi(args.argv[1]);
    }
    if (args.argc > 2) {
      endTime = atol(args.argv[2]);
    }
    Serial.print("#Open Loop Identification - ");
    Serial.print(volts, 1);
    Serial.println(F(" Volts"));
    Serial.println(F("$time(ms) Volts(V) Speed(deg/s)"));
    encoders.reset();
    motors.disable_controllers();
    motors.set_closed_loop(false);
    motors.set_motor_volts(volts);
    uint32_t start_time = millis();
    uint32_t sample_time = start_time;
    while (millis() - start_time <= endTime) {
      uint32_t now = millis();
      uint32_t elapsed = now - start_time;
      if (now - sample_time < 5) {
        continue;
      }
      sample_time += 5;
      Serial.print(elapsed);
      Serial.print(' ');
      Serial.print(motors.get_motor_volts(), 1);
      Serial.print(' ');
      Serial.print(encoders.robot_speed(), 1);
      Serial.println();
    }
    motors.set_motor_volts(0);
    endTime += 200;
    while (millis() - start_time <= endTime) {
      uint32_t now = millis();
      uint32_t elapsed = now - start_time;
      if (now - sample_time < 5) {
        continue;
      }
      sample_time += 5;
      Serial.print(elapsed);
      Serial.print(' ');
      Serial.print(motors.m_motor_volts, 1);
      Serial.print(' ');
      Serial.print(encoders.robot_speed(), 1);
      Serial.println();
    }
    motors.set_closed_loop(true);
  }

  void do_move_trial(const Args &args) {
    int mode = atoi(args.argv[1]);
    float dist = atof(args.argv[2]);
    float topSpeed = atof(args.argv[3]);
    float endSpeed = atof(args.argv[4]);
    float accel = atof(args.argv[5]);
    if (dist == 0) {
      dist = 1440;
    }
    if (topSpeed == 0) {
      topSpeed = 3600;
    }
    if (endSpeed == 0) {
      endSpeed = 0;
    }
    if (accel == 0) {
      accel = 14400;
    }
    enable_drive();
    Serial.print(F("# "));
    Serial.print(args.argv[0]);
    Serial.print(' ');
    Serial.print(mode);
    Serial.print(' ');
    Serial.print(dist);
    Serial.print(' ');
    Serial.print(topSpeed);
    Serial.print(' ');
    Serial.print(endSpeed);
    Serial.print(' ');
    Serial.print(accel);
    Serial.print(' ');
    Serial.println();
    switch (mode) {
      case 0:
        Serial.println(F("# Full Control"));
        motors.enable_feed_forward();
        motors.enable_controllers();
        break;
      case 1:
        Serial.println(F("# No Feedforward"));
        motors.disable_feed_forward();
        motors.enable_controllers();
        break;
      case 2:
        Serial.println(F("# Only Feedforward"));
        motors.enable_feed_forward();
        motors.disable_controllers();
        break;
    }

    reporter.report_controller_header();
    profile.start(dist, topSpeed, endSpeed, accel);
    while (!profile.is_finished()) {
      reporter.report_controller(profile);
    }
    motors.set_motor_volts(0);
    motors.disable_controllers();
    uint32_t end_time = millis();
    while (millis() - end_time < 200) {
      reporter.report_controller(profile);
    }
    disable_drive();
    Serial.println('#');
  }

  void do_step_trial(const Args &args) {
    float dist = atof(args.argv[1]);
    if (dist == 0) {
      dist = 30;
    }
    Serial.println(F("# Controller Only"));
    enable_drive();
    motors.disable_feed_forward();
    motors.enable_controllers();
    uint32_t start_time = millis();
    reporter.report_controller_header();
    while (millis() - start_time < 100) {
      reporter.report_controller(profile);
    }
    profile.set_position(dist);
    while (millis() - start_time < 600) {
      reporter.report_controller(profile);
    }
    motors.set_motor_volts(0);
    uint32_t end_time = millis();
    while (millis() - end_time < 100) {
      reporter.report_controller(profile);
    }
    disable_drive();
    Serial.println('#');
  }
};
