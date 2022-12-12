#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"
#include "utils.h"
#include <Arduino.h>
#include <EEPROM.h>

/***
 * Settings is where we hold the working copies of many of the
 * parameters described in the config files
 */
struct Settings {
  struct Data {
    uint8_t control_flags;
    float degPerCount;
    float Km;
    float Tm;
    float zeta;
    float Td;
    float Kp;
    float Kd;
    float biasFF;
    float speedFF;
    float accFF;
  };

  Data data;

  void init(Data defaults) {
    data = defaults;
  }

  /***
   * Read the working settings from EEPROM.
   * Undoes changes
   */
  void read() {
    Data d;
    EEPROM.get(0, d);
    data = d;
  };

  /***
   * Store the current working settings to EEPROM
   */
  void write() {
    EEPROM.put(0, data);
  };

  /***
   * Print the current working settings
   */
  /* clang-format off */
  void print() {
    Serial.print(F("    degPerCount = "));    Serial.println(data.degPerCount, 5);
    Serial.print(F("             Km = "));    Serial.println(data.Km, 2);
    Serial.print(F("             Tm = "));    Serial.println(data.Tm, 5);
    Serial.print(F("         biasFF = "));    Serial.println(data.biasFF, 5);
    Serial.print(F("        speedFF = "));    Serial.println(data.speedFF, 5);
    Serial.print(F("          accFF = "));    Serial.println(data.accFF, 5);
    Serial.print(F("           zeta = "));    Serial.println(data.zeta, 5);
    Serial.print(F("             Td = "));    Serial.println(data.Td, 5);
    Serial.print(F("             KP = "));    Serial.println(data.Kp, 5);
    Serial.print(F("             KD = "));    Serial.println(data.Kd, 5);
  };
};
/* clang-format on */

extern Settings settings;

/***
 * These are the as-compiled defaults. They are
 * used to initialise the working settings on-demand
 */
const Settings::Data defaults = {
  control_flags : 0,
  degPerCount : DEG_PER_COUNT,
  Km : Km,
  Tm : Tm,
  zeta : zeta,
  Td : Td,
  Kp : KP,
  Kd : KD,
  biasFF : BIAS_FF,
  speedFF : SPEED_FF,
  accFF : ACC_FF,
};

#endif