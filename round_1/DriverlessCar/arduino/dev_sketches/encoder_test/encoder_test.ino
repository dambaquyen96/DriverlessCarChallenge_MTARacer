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
  * @file     encoder_test.ino
  * @version  v1.0 2017/01/04

  * @about    This is an Arduino sketch designed to test the functionality of 
  *           an encoder hardware. The sketch is intended for use with Arduino
  *           Uno only, although compilation for other boards should be 
  *           possible with some modification.
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
  *             As the encoder rotates, a value reflecting how the encoder is 
  *             rotating (increasing when the encoder is rotating one way, 
  *             and decreasing when the encoder is rotating the other way).
  * 
  */

// Compile time warning if the sketch is not being compiled for Arduino Uno
#ifndef ARDUINO_AVR_UNO
# pragma message "SKETCH INTENDED FOR ARDUINO UNO ONLY"
#endif

// ===== LIBRARIES INCLUDES ===================================================
#include  <stdint.h>

// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define   ENCODER_OPTIMIZE_INTERRUPTS
#include  <Encoder.h>


// ===== CONFIGURATION MACROS =================================================
#define   CFG_SERIAL_BAUD_RATE        115200
#define   CFG_ENCODER_PIN_A           2
#define   CFG_ENCODER_PIN_B           3
#define   CFG_DEFAULT_ENCODER_VAL     0 // Default Encoder::read() value

// Default last read value. Just need to be different from 
//    CFG_DEFAULT_ENCODER_VAL
#define   CFG_DEFAULT_LAST_READ_VAL   CFG_DEFAULT_ENCODER_VAL + 1


// ===== MEMBER VARIABLES =====================================================

// Encoder object
static Encoder  mEncoder(CFG_ENCODER_PIN_A, CFG_ENCODER_PIN_B);

// Variable to store the last Encoder::read() value, used to detect new value.
static int32_t  mLastEncoderValue = CFG_DEFAULT_LAST_READ_VAL; 

// Variable to store current Encoder::read()
static int32_t  mNewEncoderValue;


// ===== ARDUINO's BASIC FUNCTIONALITY FUNCTION ===============================

/** 
  * @brief    setup() function run once at the beginning
  */
void setup() {
  Serial.begin(CFG_SERIAL_BAUD_RATE);
  Serial.println("encoder_test_sketch");
  mEncoder.write(CFG_DEFAULT_ENCODER_VAL);
}

/** 
  * @brief    loop() function run repeatedly after setup()
  */
void loop() {
  mNewEncoderValue = mEncoder.read();
  if (mLastEncoderValue != mNewEncoderValue) {
    
    // If new value is different from last displayed value, print the new value
    //    and update the last value variable
    Serial.println(mNewEncoderValue);
    mLastEncoderValue = mNewEncoderValue;
  }
}
