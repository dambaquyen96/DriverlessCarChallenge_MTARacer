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
  * @file     encoder_count_to_distance.ino
  * @version  v1.0 2017/01/23
  * @about    This is an Arduino sketch designed to check the relation between
  *           encoder count and distance travelled on FPT Driverless Car
  *
  *           Requirement:
  *             The following libraries installed in Arduino Library Manager:
  *               Encoder by Paul Stoffregen
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
  *             CFG_ENCODER_PIN_A, CFG_ENCODER_PIN_B
  *               Change the encoder input pin
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
  *             configured speed after 5 seconds, and stop after encoder count
  *             exceed the configured threshold amount
  *             
  *             The ESC should be turned on when the Arduino is outputing the
  *             "neutral speed" signal. User could use the 5-second delay for
  *             this
  * 
  */


#include <Encoder.h>
#include <Servo.h>

#define CFG_SERIAL_BAUD_RATE    115200
#define CFG_ENCODER_PIN_A       3
#define CFG_ENCODER_PIN_B       2
#define CFG_ENCODER_COUNT_THRESHOLD 20000
#define CFG_ESC_RUN_ON_TIME     1620
#define CFG_ESC_PIN             10
#define CFG_ESC_NEUTRAL_ON_TIME 1500

static Servo mServo;
static Encoder mEncoder(CFG_ENCODER_PIN_A, CFG_ENCODER_PIN_B);

void setup() {
  Serial.begin(CFG_SERIAL_BAUD_RATE);
  
  mServo.attach(CFG_ESC_PIN);
  mServo.writeMicroseconds(CFG_ESC_NEUTRAL_ON_TIME);
  mEncoder.write(0);

  delay(5000);

  mServo.writeMicroseconds(CFG_ESC_RUN_ON_TIME);

  Serial.println("begin");
}

void loop() {
  static int32_t encoderCount = 0;
  encoderCount = mEncoder.read();
  if (encoderCount > CFG_ENCODER_COUNT_THRESHOLD) {
    mServo.writeMicroseconds(CFG_ESC_NEUTRAL_ON_TIME);
    mEncoder.write(0);
    Serial.println("end");
  }
  else {
    Serial.println(encoderCount);
  }
}
