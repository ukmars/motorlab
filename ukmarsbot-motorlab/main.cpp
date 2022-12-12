/*
 * File: mazerunner.ino
 * Project: mazerunner
 * File Created: Monday, 5th April 2021 8:38:15 am
 * Author: Peter Harrison
 * -----
 * Last Modified: Thursday, 8th April 2021 8:38:41 am
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
#include "commands.h"
#include "config.h"
#include "reports.h"
#include "robot.h"
#include "src/adc.h"
#include "src/cli.h"
#include "src/encoders.h"
#include "src/motors.h"
#include "src/settings.h"
#include "src/systick.h"
#include <Arduino.h>

// Global objects
Systick systick;
AnalogueConverter adc;
Encoders encoders;
// Sensors sensors;
Motors motors;
Profile profile;
Settings settings;
Robot robot;
CommandLineInterface cli;
Reporter reporter;

void setup() {
  Serial.begin(BAUDRATE);
  adc.init();
  systick.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  settings.init(defaults);
  motors.setup();
  encoders.setup();

  Serial.println(F("MOTORLAB 1.0"));

  cli.add_cmd(send_id, PSTR("*IDN?"), PSTR("Request robot ID"));
  cli.add_cmd(print_settings, PSTR("$"), PSTR("Display all Setting"));
  cli.add_cmd(write_settings, PSTR("!"), PSTR("Write settings to EEPROM"));
  cli.add_cmd(read_settings, PSTR("@"), PSTR("Read settings from EEPROM"));
  cli.add_cmd(init_settings, PSTR("#"), PSTR("Initialise settings to defaults"));
  cli.add_cmd(set_get_km, PSTR("KM"), PSTR("Set/Get Km"));
  cli.add_cmd(set_get_tm, PSTR("TM"), PSTR("Set/Get Tm"));
  cli.add_cmd(set_get_kp, PSTR("KP"), PSTR("Set/Get Kp"));
  cli.add_cmd(set_get_kd, PSTR("KD"), PSTR("Set/Get Kd"));
  cli.add_cmd(set_get_zeta, PSTR("ZETA"), PSTR("Set/Get Damping Ratio, zeta"));
  cli.add_cmd(set_get_td, PSTR("TD"), PSTR("Set/Get settling time, Td"));
  cli.add_cmd(set_get_bias_ff, PSTR("BIASFF"), PSTR("Set/Get bias feed forward"));
  cli.add_cmd(set_get_speed_ff, PSTR("SPEEDFF"), PSTR("Set/Get speed feedforward"));
  cli.add_cmd(get_battery_volts, PSTR("BATT"), PSTR("Get battery Voltage"));
  cli.add_cmd(do_move, PSTR("MOVE"), PSTR("Execute move profile"));
  cli.add_cmd(do_step, PSTR("STEP"), PSTR("Execute single step"));
  cli.add_cmd(do_encoders, PSTR("ENC"), PSTR("Set/Get KM"));
  cli.add_cmd(do_open_loop, PSTR("VOLTS"), PSTR("Execute open loop"));
  cli.prompt();
}

void loop() {
  if (cli.read_serial() > 0) {
    cli.interpret_line();
  }
}

/**
 * Measurements indicate that even at 1500mm/s the total load due to
 * the encoder interrupts is less than 3% of the available bandwidth.
 */

// INT1 will respond to the XOR-ed pulse train from the right encoder
// runs in constant time of around 3us per interrupt.
// would be faster with direct port access
ISR(INT1_vect) {
  encoders.encoder_input_change();
}

ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) {
  systick.update();
}

ISR(ADC_vect) {
  adc.update_channel();
}
