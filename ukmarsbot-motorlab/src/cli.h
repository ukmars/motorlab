/*
 * File: cli.h
 * Project: mazerunner
 * File Created: Sunday, 28th March 2021 2:44:57 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Monday, 5th April 2021 2:59:37 pm
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

#include "types.h"
#include "utils.h"
#include <Arduino.h>
#include <stdint.h>

const int INPUT_BUFFER_SIZE = 32;
const int MAX_CMD_COUNT = 24;

class CommandLineInterface {

public:
  /***
   * Read characters from the serial port into the buffer.
   * return 1 if there is a complete line avaialable
   * return 0 if not.
   *
   * Input is optionally echoed back through the serial port
   * and can be edited by the user using the backspace key.
   *
   * Accepted characters are converted to upper case for
   * convenience.
   *
   * Lines are terminated with a LINEFEED character which is
   * echoed but not placed in the buffer.
   *
   * All printable characters are placed in a buffer with a
   * maximum length of just 32 characters. You could make this
   * longer but there should be little practical need.
   *
   * All other characters, including carriage returns are ignored.
   *
   */
  const char BACKSPACE = 0x08;

  void echo(char c) {
    if (m_echo) {
      Serial.print(c);
    }
  }

  int read_serial() {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') {
        echo(c);
        return 1;
      } else if (c == BACKSPACE) {
        if (m_index > 0) {
          m_index--;
          m_input_buffer[m_index] = 0;
          echo(c);
          echo(' ');
          echo(c);
        }
      } else if (isPrintable(c)) {
        c = toupper(c);
        echo(c);
        if (m_index < INPUT_BUFFER_SIZE - 1) {
          m_input_buffer[m_index++] = c;
          m_input_buffer[m_index] = 0;
        }
      } else {
        // drop the character silently
      }
    }
    return 0;
  }

  /***
   * Input lines are parsed into a number of space-separated
   * tokens which are stored in an argument structure, Args.
   *
   * This structure has an integer count of the number of tokens
   * and an array of the token values as strings.
   *
   * After tokenising, the arguments are examined and processed.
   *
   * Single character commands are handled separately for simplicity.
   *
   * Commands that consist of more than one token have their own
   * handler which executes a function that is passed a reference to
   * the list of tokens as an argument.
   *
   * Once a command line has been dealt with, the input buffer is
   * cleared, that means that new characters that arrive while a
   * function is executing will be lost. A side effect of that
   * is that commands cannot be aborted over the serial link.
   *
   * NOTES:
   *    - serial input is dealt with by polling so you must
   *      frequently check for new input in the main program loop.
   *    - tokenising uses the input buffer so no extra storage space
   *      is used.
   *
   */
  void interpret_line() {
    Args args = get_tokens();
    execute(args);
    clear_input();
    prompt();
  }

  /***
   * Tokenising is a process where the input buffer is examined
   * for separators that divide one string from another. Here
   * the separator is a space character, a comma or an '=' sign.
   * consecutive separators are treated as one.
   *
   * The start of each separate string is recorded in an array
   * in the Args structure and a null character is placed at its
   * end. In this way, the original string gets to look like a
   * sequence of shorter strings - the tokens. The argv array
   * is a list of where each starts and you can think if it as
   * aan array of strings. The argc element keeps count of how
   * many tokens were found.
   *
   * If you wanted to list all the tokens after processing, you
   * would just use the code:
   *
   *      for (int i = 0; i < args.argc; i++) {
   *        Serial.println(args.argv[i]);
   *      }
   *
   */
  Args get_tokens() {
    Args args = {0};
    char *line = m_input_buffer;
    char *token;
    for (token = strtok(line, " ,="); token != NULL; token = strtok(NULL, " ,=")) {
      args.argv[args.argc] = token;
      args.argc++;
      if (args.argc == MAX_ARGC)
        break;
    }
    return args;
  }

  /***
   * Run a complex command. These all start with a string and have
   * arguments. The command string can be a single letter.
   *
   * The arguments will be passed on to the robot.
   */
  void execute(const Args &args) {
    // 'internal' cli commands
    if (strcmp_P(args.argv[0], PSTR("ECHO")) == 0) {
      if (strcmp_P(args.argv[1], PSTR("ON")) == 0) {
        enable_echo();
        return;
      } else {
        disable_echo();
        return;
      };
    } else if (strcmp_P(args.argv[0], PSTR("?")) == 0) {
      help();
      return;
    }
    // 'public' commands
    for (int i = 0; i < m_last_command; i++) {
      if (strcmp_P(args.argv[0], m_commands[i]) == 0) {
        m_actions[i](args);
        return;
      }
    }
    Serial.print('"');
    Serial.print(args.argv[0]);
    Serial.print('"');
    Serial.print(' ');
    Serial.print(F("Unknown Command\n"));
  }

  void clear_input() {
    m_index = 0;
    m_input_buffer[m_index] = 0;
  }

  void help() {
    Serial.print(m_last_command);
    Serial.println(F(" commands"));
    for (int i = 0; i < m_last_command; i++) {
      int n = Serial.print((__FlashStringHelper *)m_commands[i]);
      n = 10 - n;
      while (n--) {
        Serial.write(' ');
      }
      Serial.print((__FlashStringHelper *)m_help_texts[i]);
      Serial.println();
    }
  }

  void prompt() {
    Serial.print('>');
    Serial.print(' ');
  }

  void enable_echo() {
    m_echo = true;
  }

  void disable_echo() {
    m_echo = false;
  }

  void add_cmd(cmd_func_ptr_t func, const char *cmd, const char *help) {
    if (m_last_command >= MAX_CMD_COUNT) {
      Serial.println(F("Too many Commands"));
      return; // drop the method
    }
    m_commands[m_last_command] = cmd;
    m_actions[m_last_command] = func;
    m_help_texts[m_last_command] = help;
    m_last_command++;
  }

private:
  char m_input_buffer[INPUT_BUFFER_SIZE];
  uint8_t m_index = 0;
  bool m_echo = true;
  int m_last_command = 0;
  const char *m_commands[MAX_CMD_COUNT] = {};
  const char *m_help_texts[MAX_CMD_COUNT] = {};
  cmd_func_ptr_t m_actions[MAX_CMD_COUNT] = {};
};
