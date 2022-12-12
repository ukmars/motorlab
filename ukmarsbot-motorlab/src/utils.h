#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <Arduino.h>
#define MAX_DIGITS 8

// simple formatting functions for printing maze costs
inline void print_hex_2(unsigned char value) {
  if (value < 16) {
    Serial.print('0');
  }
  Serial.print(value, HEX);
}

inline void print_justified(int32_t value, int width) {
  int v = value;
  int w = width;
  w--;
  if (v < 0) {
    w--;
  }
  while (v /= 10) {
    w--;
  }
  while (w > 0) {
    Serial.write(' ');
    --w;
  }
  Serial.print(value);
}

inline void print_justified(int value, int width) {
  print_justified(int32_t(value), width);
}

/***
 * Scan a character array for an integer.
 * Begin scan at line[pos]
 * Assumes no leading spaces.
 * Stops at first non-digit.
 * MODIFIES pos so that it points to the first non-digit
 * MODIFIES value ONLY IF a valid integer is converted
 * RETURNS  boolean status indicating success or error
 *
 * optimisations are possible but may not be worth the effort
 */
inline uint8_t read_integer(const char *line, int &value) {
  char *ptr = (char *)line;
  char c = *ptr++;
  bool is_minus = false;
  uint8_t digits = 0;
  if (c == '-') {
    is_minus = true;
    c = *ptr++;
  }
  int32_t number = 0;
  while (c >= '0' and c <= '9') {
    if (digits++ < MAX_DIGITS) {
      number = 10 * number + (c - '0');
    }
    c = *ptr++;
  }
  if (digits > 0) {
    value = is_minus ? -number : number;
  }
  return digits;
}

/***
 * Scan a character array for a float.
 * This is a much simplified and limited version of the library function atof()
 * It will not convert exponents and has a limited range of valid values.
 * They should be more than adequate for the robot parameters however.
 * Begin scan at line[pos]
 * Assumes no leading spaces.
 * Only scans MAX_DIGITS characters
 * Stops at first non-digit, or decimal point.
 * MODIFIES pos so that it points to the first character after the number
 * MODIFIES value ONLY IF a valid float is converted
 * RETURNS  boolean status indicating success or error
 *
 * optimisations are possible but may not be worth the effort
 */
inline uint8_t read_float(const char *line, float &value) {

  char *ptr = (char *)line;
  char c = *ptr++;
  uint8_t digits = 0;

  bool is_minus = false;
  if (c == '-') {
    is_minus = true;
    c = *ptr++;
  }

  uint32_t a = 0.0;
  int exponent = 0;
  while (c >= '0' and c <= '9') {
    if (digits++ < MAX_DIGITS) {
      a = a * 10 + (c - '0');
    }
    c = *ptr++;
  };
  if (c == '.') {
    c = *ptr++;
    while (c >= '0' and c <= '9') {
      if (digits++ < MAX_DIGITS) {
        a = a * 10 + (c - '0');
        exponent = exponent - 1;
      }
      c = *ptr++;
    }
  }
  float b = a;
  while (exponent < 0) {
    b *= 0.1;
    exponent++;
  }
  if (digits > 0) {
    value = is_minus ? -b : b;
  }
  return digits;
}

/* Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the chromiumos LICENSE file.
 * Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A
 * table-based algorithm would be faster, but for only a few bytes it isn't
 * worth the code size. */
inline uint8_t crc8(const void *vptr, int len) {
  const uint8_t *data = (const uint8_t *)vptr;
  unsigned crc = 0;
  int i, j;
  for (j = len; j; j--, data++) {
    crc ^= (*data << 8);
    for (i = 8; i; i--) {
      if (crc & 0x8000)
        crc ^= (0x1070 << 3);
      crc <<= 1;
    }
  }
  return (uint8_t)(crc >> 8);
}

#endif