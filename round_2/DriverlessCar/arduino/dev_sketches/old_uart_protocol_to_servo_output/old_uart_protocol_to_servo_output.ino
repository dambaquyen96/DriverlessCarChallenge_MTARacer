#include  <Servo.h>
#include  <stdint.h>

#define   CFG_ESC_CTRL_PIN                  10
#define   CFG_SERIAL_BAUD_RATE              115200
#define   CFG_ESC_NEUTRAL_PWM_ON_TIME       1500
#define   CFG_ESC_MAX_ALLOWED_PWM_ON_TIME   1750


#define   US_PER_COMMAND_UNIT               ((CFG_ESC_MAX_ALLOWED_PWM_ON_TIME - CFG_ESC_NEUTRAL_PWM_ON_TIME) / 50)
#define   STATE_DIRECTION                   1
#define   STATE_VALUE                       2


static  Servo     mServo;
static  uint32_t  mValue;
static  bool      mValueReady = false;





#define DEBUG_ENABLE  false
#if DEBUG_ENABLE
# define SERIAL_DEBUG_PRINT(x)    Serial.print(x)
# define SERIAL_DEBUG_PRINTLN(x)  Serial.println(x)
#else
# define SERIAL_DEBUG_PRINT(x)
# define SERIAL_DEBUG_PRINTLN(x)
#endif





void setup() {
  Serial.begin(CFG_SERIAL_BAUD_RATE);
  SERIAL_DEBUG_PRINTLN("jetson_to_esc");
  mServo.attach(CFG_ESC_CTRL_PIN);
  mServo.writeMicroseconds(CFG_ESC_NEUTRAL_PWM_ON_TIME);
}


void loop() {
  processSerialData();
  if (mValueReady) {
    SERIAL_DEBUG_PRINT("mValue = "); SERIAL_DEBUG_PRINTLN(mValue);
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
