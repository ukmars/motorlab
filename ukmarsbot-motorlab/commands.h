/******************************************************************************
 * Project: <<project>>                                                       *
 * File:    commands.h                                                        *
 * File Created: Saturday, 10th December 2022 8:16:32 pm                      *
 * Author: Peter Harrison                                                     *
 * -----                                                                      *
 * Last Modified: Saturday, 10th December 2022 11:34:49 pm                    *
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
#pragma once
#include "robot.h"
#include "src/types.h"

/***
 * These are all the 'public' commands understood by the command line interpreter
 */

cli_status_t send_id(const Args &args);

cli_status_t init_settings(const Args &args);
cli_status_t read_settings(const Args &args);
cli_status_t write_settings(const Args &args);
cli_status_t print_settings(const Args &args);

cli_status_t set_get_km(const Args &args);
cli_status_t set_get_tm(const Args &args);
cli_status_t set_get_kp(const Args &args);
cli_status_t set_get_kd(const Args &args);
cli_status_t set_get_zeta(const Args &args);
cli_status_t set_get_td(const Args &args);

cli_status_t set_get_bias_ff(const Args &args);
cli_status_t set_get_speed_ff(const Args &args);
cli_status_t set_get_acc_ff(const Args &args);

cli_status_t get_battery_volts(const Args &args);
cli_status_t do_move(const Args &args);
cli_status_t do_step(const Args &args);
cli_status_t do_encoders(const Args &args);
cli_status_t do_open_loop(const Args &args);

cli_status_t action(const Args &args);

/***
 * this function template allows us to get or set the value
 * of any variable regardless of type
 */

template <class T>
void cmdSetGet(T &var, T min, T max, const Args &args, int dp = 2) {
  if (args.argc > 1) {
    var = atof(args.argv[1]);
    if (var > max) {
      var = max;
    }
    if (var < min) {
      var = min;
    }
  }
  Serial.print(args.argv[0]);
  Serial.print(F(" = "));
  Serial.println(var, dp);
}