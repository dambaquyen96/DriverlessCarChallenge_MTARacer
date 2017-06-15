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
  * @file     sonar_sensor_test.ino
  * @version  v1.0 2017/01/05 
  * @about    This is an Arduino sketch designed to test the functionality of 
  *           the HC-SR04 sonar sensor in FPT Driverless Car.
  *
  *           Configuration: 
  *             User could configure the sensor Trig and Echo pin by changing 
  *             the macros CFG_HC_SR04_TRIG_PIN and CFG_HC_SR04_ECHO_PIN.
  *
  *           Behavior:
  *             Once started running this sketch, the Arduino would repeately 
  *             print out the width of the pulse measured from the sensor and 
  *             the equivalent distance in centimeters.
  * 
  */

// Compile time warning if the sketch is not being compiled for Arduino Uno
#ifndef ARDUINO_AVR_UNO
# pragma message "SKETCH INTENDED FOR ARDUINO UNO ONLY"
#endif


// ===== CONFIGURATION MACROS =================================================
#define   CFG_SERIAL_BAUD_RATE  115200
#define   CFG_HC_SR04_TRIG_PIN  11
#define   CFG_HC_SR04_ECHO_PIN  9


// ===== MEMBER VARIABLES =====================================================
static  long  mEchoPulseWidth;


// ===== ARDUINO's BASIC FUNCTIONALITY FUNCTION ===============================

/** 
  * @brief    setup() function run once at the beginning
  */
void setup() {
  Serial.begin(CFG_SERIAL_BAUD_RATE);
  pinMode(CFG_HC_SR04_TRIG_PIN, OUTPUT);
  pinMode(CFG_HC_SR04_ECHO_PIN, INPUT);
}


/** 
  * @brief    loop() function run repeatedly after setup()
  */
void loop() {
  static  float  distanceCentimeters;
  
  // Poll sensor for new measurement
  pollSensor();
  
  // Calculate distance from measurement. Formulat derived from information provided in HC-SR04 datasheet
  distanceCentimeters = mEchoPulseWidth * 340 / 2 / 1000000;
  
  // Display the result
  Serial.print("Echo pulse width  = "); Serial.println(mEchoPulseWidth);
  Serial.print("Distance(cm)      = "); Serial.println(distanceCentimeters);
  Serial.println();
  
  // Delay 1000ms between two consecutive measurements
  delay(1000);
}

/** 
  * @brief  Helper function to poll the sonar sensor for new measurement
  */
void pollSensor() {
  
  // Generate a 5ms pulse at Trigger pin
  digitalWrite(CFG_HC_SR04_TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(CFG_HC_SR04_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(CFG_HC_SR04_TRIG_PIN, LOW);
  
  // Measure the Echo pulse width
  mEchoPulseWidth = pulseIn(CFG_HC_SR04_ECHO_PIN, HIGH);
}

