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
  * @file     pca9685.ino
  * @version  v1.0 2017/01/05
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the Steering System of FPT Driverless Car. 
  *
  *           Premier:
  *             The "Steering System," as referred to in this file, consists of 
  *             an Adafruit PCA9685 module, its adapter board and the steering 
  *             servo. The Jetson TK1 board control the steering wheels by 
  *             sending I2C messages to the Adafruit PCA9685 module.
  *
  *             By design, the steering system is controlled by the Jetson TK1 
  *             computer board. However, user could connect the PCA9685 module 
  *             to an Arduino UNO instead of the Jetson TK1.
  *           
  *             Connection to the UNO should be as described here:
  *               PCA9685 GND -> UNO GND
  *               PCA9685 SDA -> UNO A4
  *               PCA9685 SCL -> UNO A5
  *               PCA9685 VCC -> UNO 5V
  *             
  *           Behavior
  *             Running this sketch, the UNO receives user input through the 
  *             Serial peripheral to send a value to the PCA9685. The PCA9685
  *             will control the steering servo using that value.
  *
  *             Experiments showed that the value is about the range (136, 484).
  *
  *             To change the PCA9685 value, the user could send the following 
  *             messages via UART:
  *             
  *               [VALUE]'\n'
  *                   Set the current steering value to [VALUE]. Eg. "250\n"
  *               "a"
  *                   Decrease the steering value by 1, turning the wheel left
  *               "d"
  *                   Increase the steering value by 1, turning the wheel right
  *               "q"
  *                   Decrease the steering value by 5, turning the wheel left
  *               "e"
  *                   Increase the steering value by 5, turning the wheel right
  *               "w"
  *                   Reset the steering value to 280 (neutral position)
  *           
  *           The following configurable macros are available:
  *               CFG_SERIAL_BAUD_RATE
  *                   Configure the baud rate of Serial (UART) communication
  *               CFG_PWM_CHANNEL_A
  *                   Configure which PCA9685 channel should generate the servo
  *                   controlling signal.
  *                   
  *                   Note that on the FPT Driverless Car, the PWM channel 
  *                   controlling the steering servo is fixed, so only configure 
  *                   that if you know what you are doing
  *
  *               CFG_ENABLE_VALID_VALUE_SATURATION
  *                   Disabling the saturation of the Steering Value. Going 
  *                   outside of the valid range could permanently damage the 
  *                   steering servo. But doing that is required to find out 
  *                   the actual absolute limit for calibration purpose.
  *                   Only disable this if you know what you are doing.
  *           
  *             
  */



#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

#define   CFG_SERIAL_BAUD_RATE              115200
#define   CFG_PWM_CHANNEL_A                 0
//#define   CFG_PWM_CHANNEL_B                 2
//#define   CFG_PWM_CHANNEL_C                 4
#define CFG_ENABLE_VALID_VALUE_SATURATION   true

#define   PWM_NEUTRAL         280
#define   PWM_MAX             484
#define   PWM_MIN             136

Adafruit_PWMServoDriver mAdafruitPwmServoDriver = Adafruit_PWMServoDriver();
static  uint32_t  mValue;

void setup() {
  Serial.begin(115200);
  Serial.println("16 channel PWM test!");

  mAdafruitPwmServoDriver.begin();
  mAdafruitPwmServoDriver.setPWMFreq(50);
  mValue = PWM_NEUTRAL;
  mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_A, 0, mValue);
//  mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_B, 0, mValue);
//  mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_C, 0, mValue);
}

void loop() {
  static uint32_t mLastValue = mValue;
  
  processSerialData();
  if (mValue != mLastValue) {
#   if CFG_ENABLE_VALID_VALUE_SATURATION

      // Saturate value between limits
      if (mValue < PWM_MIN) {
        mValue = PWM_MIN;
      }
      else if (mValue > PWM_MAX) {
        mValue = PWM_MAX;
      }
    
#   endif // CFG_ENABLE_VALID_VALUE_SATURATION
    
    Serial.print("mValue = "); Serial.println(mValue);
    
    // Send the value to PCA9685
    mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_A, 0, mValue);
//    mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_B, 0, mValue);
//    mAdafruitPwmServoDriver.setPWM(CFG_PWM_CHANNEL_C, 0, mValue);

    mLastValue = mValue;
  }
  
}

void processSerialData(void) {
  static char     rxChar;
  static uint32_t rxValue;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    if (rxChar == '\n') {
      mValue      = rxValue;
    }
    else if ((rxChar >= '0') && (rxChar <= '9')) {
      if (rxValue == mValue) {
        rxValue   = 0;
      }
      rxValue     *= 10;
      rxValue     += rxChar - 0x30;
    }
    else if (rxChar == 'a') {
      rxValue     = --mValue;
    }
    else if (rxChar == 'd') {
      rxValue     = ++mValue;
    }
    else if (rxChar == 'q') {
      mValue      += 5;
      rxValue     = mValue;
    }
    else if (rxChar == 'e') {
      mValue      -= 5;
      rxValue     = mValue;
    }
    else if (rxChar == 'w') {
      mValue      = PWM_NEUTRAL;
      rxValue     = mValue;
    }
  }
}
