#ifndef ADC_H
#define ADC_H

#include "../config.h"
#include <Arduino.h>
#include <util/atomic.h>
#include <wiring_private.h>

class AnalogueConverter {

public:
  AnalogueConverter(){};

  /**
   *  The default for the Arduino is to give a slow ADC clock for maximum
   *  SNR in the results. That typically means a prescale value of 128
   *  for the 16MHz ATMEGA328P running at 16MHz. Conversions then take more
   *  than 100us to complete. In this application, we want to be able to
   *  perform about 16 conversions in around 500us. To do that the prescaler
   *  is reduced to a value of 32. This gives an ADC clock speed of
   *  500kHz and a single conversion in around 26us. SNR is still pretty good
   *  at these speeds:
   *  http://www.openmusiclabs.com/learning/digital/atmega-m_adc_reading/
   *
   * @brief change the ADC prescaler to give a suitable conversion rate.
   */
  void init() {
    // Change the clock prescaler from 128 to 32 for a 500kHz clock
    bitSet(ADCSRA, ADPS2);
    bitClear(ADCSRA, ADPS1);
    bitSet(ADCSRA, ADPS0);
  }

  void start_adc_cycle() {
    bitSet(ADCSRA, ADIE);              // enable the ADC interrupt
    start_conversion(BATTERY_ADC_PIN); // begin a conversion to get things started
  }

  /***
   * NOTE: Manual analogue conversions
   * ADC channels are automatically converted
   * by the sensor interrupt. Attempting to performa a manual ADC
   * conversion with the Arduino AnalogueIn() function will disrupt
   * that process so avoid doing that.
   */

  const uint8_t ADC_REF = DEFAULT;

  void start_conversion(uint8_t pin) {
    if (pin >= 14)
      pin -= 14; // allow for channel or pin numbers
                 // set the analog reference (high two bits of ADMUX) and select the
                 // channel (low 4 bits).  Result is right-adjusted
    ADMUX = (ADC_REF << 6) | (pin & 0x07);
    // start the conversion
    sbi(ADCSRA, ADSC);
  }

  int get_adc_result() {
    return ADC;
  }

  /***
   * Although only a single channel is used here, the scheme is left
   * in place for easy expansion. For example, the code could be
   * used to test and demonstrate a servo or some other system
   * with analogue feedback
   */
  void update_channel() {
    m_battery_adc = get_adc_result();
    bitClear(ADCSRA, ADIE); // turn off the interrupt
    m_battery_volts = BATTERY_MULTIPLIER * m_battery_adc;
    m_battery_compensation = 255.0 / m_battery_volts;
  }

  float get_battery_comp() {
    return m_battery_compensation;
  };

  float get_battery_voltage() {
    return m_battery_volts;
  }

private:
  volatile int m_battery_adc;
  volatile float m_battery_volts;
  volatile float m_battery_compensation;
};

extern AnalogueConverter adc;
#endif