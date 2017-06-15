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
  * @file     test_esc_servo_output_fixed.ino
  * @version  v1.0 2017/01/20
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the ESC and engine on FPT Driverless Car.
  * 
  *           Configuration: 
  *             The following macros could be changed to configured the sketch
  *             
  *             CFG_ESC_RUN_ON_TIME
  *               Change the on-time of the PWM signal controlling the ESC to
  *               drive the engine at the desire speed
  *
  *               Valid value must be between 1500 and 2000. 
  *
  *               Experiments show that to have the car run, values below 1600 
  *               result in the ESC drives too little voltage and values above 
  *               1800 causes the engine to run too fast
  *             
  *             CFG_ESC_PIN
  *               Change the pin that should output the ESC controlling signal
  *
  *               Note that on the FPT Driverless Car, the pin assignment is 
  *               fixed, so only configure the pins if you know what you are 
  *               doing
  *
  *             CFG_ESC_NEUTRAL_ON_TIME
  *               Change the on-time of the PWM signal controlling the ESC to
  *               keep the engine stopped
  *             
  *           Behavior:
  *             Running this sketch, Arduino will start running at the 
  *             configured speed after 5 seconds.
  *             
  *             The ESC should be turned on when the Arduino is outputing the
  *             "neutral speed" signal. User could use the 5-second delay for
  *             this
  * 
  */


#include <Servo.h>

#define CFG_ESC_RUN_ON_TIME 1650
#define CFG_ESC_PIN 10
#define CFG_ESC_NEUTRAL_ON_TIME 1500

static Servo mServo;

void setup() {
  mServo.attach(CFG_ESC_PIN);
  mServo.writeMicroseconds(CFG_ESC_NEUTRAL_ON_TIME);
  delay(5000);
  mServo.writeMicroseconds(CFG_ON_TIME);
}

void loop() {
  // put your main code here, to run repeatedly:

}
