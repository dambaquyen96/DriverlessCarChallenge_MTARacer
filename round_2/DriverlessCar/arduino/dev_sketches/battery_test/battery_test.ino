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
  * @file     battery_test.ino
  * @version  v1.0 2017/01/19
  * @about    This is an Arduino sketch designed to test the battery drainage 
  *           of FPT Driverless Car.
  *
  *           Setup:
  *             Override the control of steering servo by connecting:
  *               - The control pin to Arduino pin 6
  *               - The ground pin to Arduino GND
  *
  *           Behavior:
  *             Turn on/Reset Arduino. The steering wheels should return to 
  *             the neutral position.
  *
  *             After 5s, the steering wheel should move to their rightmost
  *             position. The car will start running non-stop after 1650s.
  *
  *             Start a stopwatch, grab a drink and wait until the car runs out 
  *             of battery.
  * 
  */

#include <Servo.h>


static Servo mEscServo;
static Servo mSteerServo;

void setup() {
  Serial.begin(115200);
  Serial.println("battery_test");

  mEscServo.attach(10);
  mSteerServo.attach(6);
  
  // Steering wheels to neutral position
  Serial.println("Steer: 1500");
  mSteerServo.writeMicroseconds(1500);
  
  // ESC control signal to neutral state
  Serial.println("ESC: 1500");
  mEscServo.writeMicroseconds(1500);
  
  delay(5000);
  
  // Steer to rightmost position
  Serial.println("Steer: 2000");
  mSteerServo.writeMicroseconds(2000);

  delay(1000);
  
  // Start running
  Serial.println("ESC: 1650");
  mEscServo.writeMicroseconds(1650);
  
}

void loop() {
  // Empty
}

