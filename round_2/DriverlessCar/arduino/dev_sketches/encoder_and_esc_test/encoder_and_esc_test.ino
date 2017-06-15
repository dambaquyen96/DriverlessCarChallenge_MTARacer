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
  * @file     encoder_and_esc.ino
  * @version  v1.0 2017/01/20
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the encoder and ESC on FPT Driverless Car.
  *
  *           Requirement:
  *             The following libraries installed in Arduino Library Manager:
  *               Encoder by Paul Stoffregen
  *
  *           Configuration: 
  *             User could configure the encoder pins by changing the macros
  *             CFG_ENCODER_PIN_A and CFG_ENCODER_PIN_B. 
  *
  *             Note that on the FPT Driverless Car, the pin assignment is 
  *             fixed, so only configure the pins if you know what you are 
  *             doing
  *
  *           Behavior:
  *             Running this sketch, Arduino will update Encoder count value
  *             continuously. 
  *             User could enter a value between 1500 and 1750 to run the 
  *             engine, or just push the car to see Encoder value change
  * 
  */


#define   ENCODER_OPTIMIZE_INTERRUPTS
#include  <Encoder.h>

#include  <Servo.h>
#include  <stdint.h>

#define   ESC_PIN               10
#define   ENCODER_PIN_A         3
#define   ENCODER_PIN_B         2
#define   ESC_NEUTRAL_ON_TIME   1500

static Encoder    mEncoder(ENCODER_PIN_A, ENCODER_PIN_B);
static Servo      mServo;
static uint32_t   mValue;
static bool       mValueReady;
static int32_t    mNewEncoderValue;
static int32_t    mLastEncoderValue = mNewEncoderValue; 

void setup() {
  Serial.begin(115200);
  Serial.println("test_esc");
  mServo.attach(ESC_PIN);
  mServo.writeMicroseconds(ESC_NEUTRAL_ON_TIME);
  mEncoder.write(0);
}

void loop() {
  processSerialData();
  if (mValueReady) {
    
    // Control ESC with value received from Serial
    Serial.print("Servo: "); Serial.println(mValue);
    mServo.writeMicroseconds(mValue);
    mValueReady = false;
  }
  
  mNewEncoderValue = mEncoder.read();
  if (mLastEncoderValue != mNewEncoderValue) {
    
    // If new value is different from last displayed value, print the new value
    // and update the last value variable
    Serial.print("Encoder:  "); Serial.println(mNewEncoderValue);
    mLastEncoderValue = mNewEncoderValue;
  }
}

void processSerialData(void) {
  static char     rxChar;
  static uint32_t rxValue;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    // '\n' marks end of value
    if (rxChar == '\n') {
      mValueReady = true;
      mValue      = rxValue;
      rxValue     = 0;
    }
    // Receive value
    else if ((rxChar >= '0') && (rxChar <= '9')) {
      rxValue     *= 10;
      rxValue     += rxChar - 0x30;
    }
  }
}
