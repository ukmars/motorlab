/*
 * File: encoders.h
 * Project: mazerunner
 * File Created: Saturday, 27th March 2021 3:43:33 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Monday, 5th April 2021 3:05:30 pm
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

#ifndef ENCODERS_H
#define ENCODERS_H

/****************************************************************************/
/*   ENCODERS                                                               */
/****************************************************************************/

/*
   from ATMega328p datasheet section 12:
   The ATMega328p can generate interrupt as a result of changes of state on two of its pins:

   PD2 for INT0  - Arduino Digital Pin 2
   PD3 for INT1  - Arduino Digital Pin 3

   The INT0 and INT1 interrupts can be triggered by a falling or rising edge or a low level.
   This is set up as indicated in the specification for the External Interrupt Control Register A –
   EICRA.

   The External Interrupt 0 is activated by the external pin INT0 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT0 pin that activate
   the interrupt are defined as

   ISC01 ISC00  Description
     0     0    Low Level of INT0 generates interrupt
     0     1    Logical change of INT0 generates interrupt
     1     0    Falling Edge of INT0 generates interrupt
     1     1    Rising Edge of INT0 generates interrupt


   The External Interrupt 1 is activated by the external pin INT1 if the SREG I-flag and the
   corresponding interrupt mask are set. The level and edges on the external INT1 pin that activate
   the interrupt are defined in Table 12-1

   ISC11 ISC10  Description
     0     0    Low Level of INT1 generates interrupt
     0     1    Logical change of INT1 generates interrupt
     1     0    Falling Edge of INT1 generates interrupt
     1     1    Rising Edge of INT1 generates interrupt

   To enable these interrupts, bits must be set in the external interrupt mask register EIMSK

   EIMSK:INT0 (bit 0) enables the INT0 external interrupt
   EIMSK:INT1 (bit 1) enables the INT1 external interrupt

*/
#include "../config.h"
#include <Arduino.h>
#include <stdint.h>
#include <util/atomic.h>

/***
 *
 * CAUTION: Changing the length of the moving averager will add delay to
 * the feedback loop and is going to have an effect on the PD controller.
 * Do not change this value unless you know what you are doing and are
 * prepared to re-tune the controllers - both forward and rotation.
 *
 */
const int AVERAGER_LENGTH = 8;

class Encoders;

extern Encoders encoders;
class Encoders {
public:
  void setup() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      pinMode(ENCODER_CLK, INPUT);
      pinMode(ENCODER_DIR, INPUT);
      // pin change interrupt
      bitClear(EICRA, ISC11);
      bitSet(EICRA, ISC10);
      bitSet(EIMSK, INT1);
    }
    reset();
  }

  void reset() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_right_counter = 0;
      m_robot_distance = 0;
      for (int i = 0; i < AVERAGER_LENGTH; i++) {
        m_right_history[i] = 0;
      }
      m_right_averager_total = 0;
      m_right_total = 0;
    }
  }

  void encoder_input_change() {
    static bool oldA = false;
    static bool oldB = false;
    bool newB = digitalRead(ENCODER_DIR);
    bool newA = digitalRead(ENCODER_CLK) ^ newB;
    int delta = ENCODER_POLARITY * ((oldA ^ newB) - (newA ^ oldB));
    m_right_counter += delta;
    oldA = newA;
    oldB = newB;
  }

  void update() {
    int right_delta = 0;
    // Make sure values don't change while being read. Be quick.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      right_delta = m_right_counter;
      m_right_counter = 0;
      m_right_total += right_delta;
    }
    // update the moving average
    m_right_averager_total -= m_right_history[m_averager_index];
    m_right_averager_total += right_delta;
    m_right_history[m_averager_index] = right_delta;

    m_averager_index += 1;
    if (m_averager_index >= AVERAGER_LENGTH) {
      m_averager_index = 0;
    }
    float right_change = m_right_averager_total * (1.0 / AVERAGER_LENGTH) * DEG_PER_COUNT;
    m_fwd_change = right_change;
    m_robot_distance += m_fwd_change;
  }

  float robot_distance() {
    float distance;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { distance = m_robot_distance; }
    return distance;
  }

  float robot_speed() {
    float speed;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { speed = LOOP_FREQUENCY * m_fwd_change; }
    return speed;
  }

  float robot_fwd_change() {
    float distance;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { distance = m_fwd_change; }
    return distance;
  }

  int m_right_total;

  // None of the variables in this file should be directly available to the rest
  // of the code without a guard to ensure atomic access
private:
  volatile float m_robot_distance;
  // the change in angle in the last tick.
  float m_fwd_change;
  // internal use only to track encoder input edges
  int m_right_counter;

  int8_t m_right_history[AVERAGER_LENGTH];
  int m_averager_index;
  int m_right_averager_total;
};

#endif