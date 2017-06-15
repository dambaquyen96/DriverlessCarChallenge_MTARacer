/** ===== PERMISSION NOTICE ===================================================
  * 
  * Copyright 2017 FPT Corporation
  *
  * Permission is hereby granted, free of charge, to any person obtaining a 
  * copy of this software and associated documentation files (the "Software"), 
  * to deal in the Software without restriction, including without limitation 
  * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  * and/or sell copies of the Software, and to permit persons to whom the 
  * Software is furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in 
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
  * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
  * DEALINGS IN THE SOFTWARE.
  *
  * ===========================================================================
*/


/** ===== FILE & SKETCH DESCRIPTION ===========================================
  * @file     test_button.ino
  * @version  v1.0 2017/01/20
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the button and LED part of the FPT Driverless Car
  *
  *           Requirement:
  *             The following libraries installed in Arduino Library Manager:
  *               TimerOne by Jesse Tane, Jerome Despatis, Michael Polli, 
  *                           Dan Clemens, Paul Stoffregen
  *
  *           Configuration: 
  *             User could configure the button input and LED pins by changing 
  *             the macros BUTTON_PIN and LED_PIN. 
  *
  *             Note that on the FPT Driverless Car, the pin assignment is 
  *             fixed, so only configure the pins if you know what you are 
  *             doing
  *
  *           Behavior:
  *             Running this sketch, Arduino will blink the LED quickly when 
  *             the button is pressed, and more slowly when the button is
  *             released.
  * 
  */


#include <TimerOne.h>

#define BUTTON_PIN 6
#define LED_PIN 7

static volatile uint8_t period = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Timer1.initialize(125000);
  Timer1.attachInterrupt(interruptHandler);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void interruptHandler() {
  static volatile uint8_t counter = 0;
  counter++;

  // Blink LED
  if (counter >= period) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    counter = 0;
  }
}

void loop() {
  // Change blink period based on button state
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println(1);
    period = 2;
  }
  else {
    Serial.println(0);
    period = 1;
  }
}
