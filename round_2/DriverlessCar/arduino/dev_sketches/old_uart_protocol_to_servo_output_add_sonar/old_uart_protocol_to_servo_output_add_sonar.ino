#include  <Servo.h>
#include  <stdint.h>

#define   CFG_ESC_CTRL_PIN                      10
#define   CFG_HC_SR04_TRIG_PIN                  12
#define   CFG_HC_SR04_ECHO_PIN                  11

#define   CFG_SERIAL_BAUD_RATE                  115200
#define   CFG_ESC_NEUTRAL_PWM_ON_TIME           1500
#define   CFG_ESC_MAX_ALLOWED_PWM_ON_TIME       1750
#define   CFG_EMERGENCY_STOP_PULSE_WIDTH_LIMIT  1750 // Approximately 30cm
#define   CFG_HC_SR04_MEASURE_TIMEOUT           60


#define   US_PER_COMMAND_UNIT               ((CFG_ESC_MAX_ALLOWED_PWM_ON_TIME - CFG_ESC_NEUTRAL_PWM_ON_TIME) / 50)
#define   STATE_DIRECTION                   1
#define   STATE_VALUE                       2


static  Servo     mServo;
static  uint32_t  mValue                = CFG_ESC_NEUTRAL_PWM_ON_TIME;
static  bool      mValueReady           = false;
static  bool      mWasInEmergencyStop   = false;
static  long      mEchoPulseWidth;




#define DEBUG_ENABLE  true
#if DEBUG_ENABLE
# define SERIAL_DEBUG_PRINT(x)    Serial.print(x)
# define SERIAL_DEBUG_PRINTLN(x)  Serial.println(x)
#else
# define SERIAL_DEBUG_PRINT(x)
# define SERIAL_DEBUG_PRINTLN(x)
#endif





void setup() {
  Serial.begin(CFG_SERIAL_BAUD_RATE);
  SERIAL_DEBUG_PRINTLN("test_esc");
  mServo.attach(CFG_ESC_CTRL_PIN);
  mServo.writeMicroseconds(CFG_ESC_NEUTRAL_PWM_ON_TIME);

  pinMode(CFG_HC_SR04_TRIG_PIN, OUTPUT);
  pinMode(CFG_HC_SR04_ECHO_PIN, INPUT);
}


void loop() {
  processSerialData();
  pollSonar();
  
  if ((mEchoPulseWidth < 1750) && (mEchoPulseWidth != 0)) {
    if (!mWasInEmergencyStop) {
      // Just entered emergency range. Immediately stop
      mWasInEmergencyStop = true;
      SERIAL_DEBUG_PRINTLN("Emergency Stop");
      mServo.writeMicroseconds(CFG_ESC_NEUTRAL_PWM_ON_TIME);
    }
  }
  else {
    if (mWasInEmergencyStop) {
      // Was held in Emergency Stop. Resume movement
      mWasInEmergencyStop = false;
      SERIAL_DEBUG_PRINTLN(mValue);
      mServo.writeMicroseconds(mValue);
    }
  }
  
  if (mValueReady) {
    // New value received over Serial. Drive that value
    SERIAL_DEBUG_PRINTLN(mValue);
    mServo.writeMicroseconds(mValue);
    mValueReady = false;
  }
}


void processSerialData(void) {
  static char     rxChar;
  static uint32_t rxValue;
  static uint8_t  rxState = STATE_DIRECTION;
  static bool     dirForward;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    if (rxState == STATE_DIRECTION) {
      if (rxChar == 'f') {
        dirForward = true;
      }
      else {
        dirForward = false;
      }
      rxState = STATE_VALUE;
    }
    else if (rxState == STATE_VALUE) {
      if ((rxChar >= '0') && (rxChar <= '9')) {
        rxValue     *= 10;
        rxValue     += rxChar - 0x30;
      }
      else if (rxChar == '\n') {
        rxState = STATE_DIRECTION;
        mValueReady = true;
        if (dirForward) {
          mValue      = CFG_ESC_NEUTRAL_PWM_ON_TIME + (rxValue * US_PER_COMMAND_UNIT);
          rxValue     = 0;
        }
        else {
          mValue      = CFG_ESC_NEUTRAL_PWM_ON_TIME - (rxValue * US_PER_COMMAND_UNIT);
          rxValue     = 0;
        }
      }
    }
  }
}

void pollSonar() {
  digitalWrite(CFG_HC_SR04_TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(CFG_HC_SR04_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(CFG_HC_SR04_TRIG_PIN, LOW);
  mEchoPulseWidth = pulseIn(CFG_HC_SR04_ECHO_PIN, HIGH);
//  mEchoPulseWidth = pulseIn(CFG_HC_SR04_ECHO_PIN, HIGH, CFG_HC_SR04_MEASURE_TIMEOUT);
}

