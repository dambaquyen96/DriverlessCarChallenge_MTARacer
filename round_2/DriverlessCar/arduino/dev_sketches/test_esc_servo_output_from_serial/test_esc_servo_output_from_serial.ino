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
  * @file     test_esc_servo_output_from_serial.ino
  * @version  v1.0 2017/01/20
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the ESC and engine on FPT Driverless Car.
  * 
  *           Configuration: 
  *             The following macros could be changed to configured the sketch
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
  *             Running this sketch, Arduino will receive user input through
  *             Serial and use the value received as the on-time for the PWM 
  *             signal controlling the ESC. 
  *             
  *             Experiments show that to have the car run, values below 1600 
  *             result in the ESC drives too little voltage and values above 
  *             1800 causes the engine to run too fast
  * 
  */


#include  <Servo.h>
#include  <stdint.h>

#define   CFG_ESC_PIN               10
#define   CFG_ESC_NEUTRAL_ON_TIME   1500

static  Servo     mServo;
static  uint32_t  mValue;
static  bool      mValueReady;

void setup() {
  Serial.begin(115200);
  Serial.println("test_esc");
  mServo.attach(ESC_PIN);
  mServo.writeMicroseconds(ESC_NEUTRAL_ON_TIME);
}

void loop() {
  processSerialData();
  if (mValueReady) {
    Serial.print("mValue = "); Serial.println(mValue);
    mServo.writeMicroseconds(mValue);
    mValueReady = false;
  }
}

void processSerialData(void) {
  static char     rxChar;
  static uint32_t rxValue;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    if (rxChar == '\n') {
      mValueReady = true;
      mValue      = rxValue;
      rxValue     = 0;
    }
    else if ((rxChar >= '0') && (rxChar <= '9')) {
      rxValue     *= 10;
      rxValue     += rxChar - 0x30;
    }
  }
}
