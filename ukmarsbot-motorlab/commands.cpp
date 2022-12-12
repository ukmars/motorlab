/******************************************************************************
 * Project: <<project>>                                                       *
 * File:    commands.cpp                                                      *
 * File Created: Saturday, 10th December 2022 8:18:14 pm                      *
 * Author: Peter Harrison                                                     *
 * -----                                                                      *
 * Last Modified: Sunday, 11th December 2022 11:52:16 am                      *
 * -----                                                                      *
 * Copyright 2022 - 2022 Peter Harrison, Micromouseonline                     *
 * -----                                                                      *
 * Licence:                                                                   *
 * MIT License                                                                *
 *                                                                            *
 * Copyright (c) 2022 Peter Harrison                                          *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of *
 * this software and associated documentation files (the "Software"), to deal in *
 * the Software without restriction, including without limitation the rights to *
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies *
 * of the Software, and to permit persons to whom the Software is furnished to do *
 * so, subject to the following conditions:                                   *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                            *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER     *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE *
 * SOFTWARE.                                                                  *
 ******************************************************************************/

#include "commands.h"
#include "config.h"
#include "src/adc.h"
#include "src/settings.h"
#include "src/types.h"
#include <Arduino.h>

cli_status_t send_id(const Args &args) {
  Serial.println(F("MOTORLAB V1.0"));
  return CLI_OK;
}

cli_status_t init_settings(const Args &args) {
  settings.init(defaults);
  return cli_status_t();
}

cli_status_t print_settings(const Args &args) {
  settings.print();
  return cli_status_t();
}

cli_status_t set_get_km(const Args &args) {
  cmdSetGet(settings.data.Km, 0.0f, 10000.0f, args, 1);
  return cli_status_t();
}

cli_status_t set_get_tm(const Args &args) {
  cmdSetGet(settings.data.Tm, 0.0f, 10.0f, args, 6);
  return cli_status_t();
}

cli_status_t set_get_kp(const Args &args) {
  cmdSetGet(settings.data.Kp, 0.0f, 10.0f, args, 6);
  return cli_status_t();
}

cli_status_t set_get_kd(const Args &args) {
  cmdSetGet(settings.data.Kd, 0.0f, 10.0f, args, 6);
  return cli_status_t();
}

cli_status_t set_get_zeta(const Args &args) {
  cmdSetGet(settings.data.zeta, 0.0f, 10.0f, args, 6);
  return cli_status_t();
}

cli_status_t set_get_td(const Args &args) {
  cmdSetGet(settings.data.Td, 0.0f, 1.0f, args, 6);
  return cli_status_t();
}

cli_status_t set_get_bias_ff(const Args &args) {
  cmdSetGet(settings.data.biasFF, 0.0f, 10.0f, args, 3);
  return cli_status_t();
}

cli_status_t set_get_speed_ff(const Args &args) {
  cmdSetGet(settings.data.speedFF, 0.0f, 10.0f, args, 5);
  return cli_status_t();
}

cli_status_t set_get_acc_ff(const Args &args) {
  cmdSetGet(settings.data.accFF, 0.0f, 10.0f, args, 6);
  return cli_status_t();
}

cli_status_t write_settings(const Args &args) {
  settings.write();
  return cli_status_t();
}

cli_status_t read_settings(const Args &args) {
  settings.read();
  return cli_status_t();
}

cli_status_t action(const Args &args) {
  Serial.print(args.argc);
  Serial.print(':');
  Serial.println(args.argv[0]);
  return CLI_OK;
};

cli_status_t get_battery_volts(const Args &args) {
  Serial.print(robot.battery_voltage(), 2);
  Serial.println(F(" Volts"));
  return cli_status_t();
}

cli_status_t do_move(const Args &args) {
  robot.do_move_trial(args);
  return cli_status_t();
}

cli_status_t do_step(const Args &args) {
  robot.do_step_trial(args);
  return cli_status_t();
}

cli_status_t do_encoders(const Args &args) {
  // add a function to calibrat ethe encoder/gearbox resolution
  return cli_status_t();
}

cli_status_t do_open_loop(const Args &args) {
  robot.do_open_loop_trial(args);
  return cli_status_t();
}
