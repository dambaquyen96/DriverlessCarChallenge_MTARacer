#include "fdc_cfg.h"
#include "fdc_pid.h"
#include <Encoder.h>
#include <Servo.h>
#include <stdio.h>

#define MAIN_DEBUG_ENABLE  true

#if (CFG_MASTER_DEBUG_ENABLE && MAIN_DEBUG_ENABLE)
# define SERIAL_DEBUG_PRINT(x)    Serial.print(x)
# define SERIAL_DEBUG_PRINTLN(x)  Serial.println(x)
#else
# define SERIAL_DEBUG_PRINT(x)
# define SERIAL_DEBUG_PRINTLN(x)
#endif

#define LED_ON LOW
#define LED_OFF HIGH
#define BUTTON_PRESSED LOW
#define BUTTON_RELEASED HIGH

#define BLINK_LED() digitalWrite(CFG_BUTTON_LED_PIN, !digitalRead(CFG_BUTTON_LED_PIN))

typedef enum {
  INPUT_INVALID,
  INPUT_SERVO,
  INPUT_AVERAGE,
  INPUT_INTERVAL,
  INPUT_SET_POINT,
  INPUT_KP,
  INPUT_KI,
  INPUT_KD,
  INPUT_I_MAX,
  INPUT_CHANGE_CONTROL_MODE,
} SerialInputType_t;


typedef enum {
  SERIAL_STATE_GET_TYPE,
  SERIAL_STATE_GET_VALUE,
} SerialState_t;

typedef enum {
  CONTROL_MODE_PID,
  CONTROL_MODE_FORCED,
} ControlMode_t;

static Encoder mEncoder(CFG_ENCODER_PIN_B, CFG_ENCODER_PIN_A);
static Servo mEscControlLer;
static volatile bool mAppTimer2InterruptFlag = false;
static SerialInputType_t mSerialInputType;
static uint32_t mSerialInputVal;
static char serialTxBuffer[100];


void timer2Setup() {
  // Draft:
  // Timer2 clock source is 16,000,000 MHz
  // At maximum prescaler of 1024, this means 15625 ticks/s
  // Because Timer2 is a 8-bit timer, so the Output Compare Match value cannot be greater than 255
  // Also 15625 = 5^6
  // So the lowest round frequency we can achieve is 15625 / 125 = 125 Hz
  // With OC = 250, we have 62.5Hz, not so horrible to work with
  // Add a app level prescaler of 16, we got 3.90625Hz. Not too far from intended 4Hz.
  // Eh, good enough
  // So, prescale by 1024, so CS2[2:0] = b111. Output Compare = 250, so OCR2A = 250

  // Stop interrupts
  cli();
  
  // Clear TC2 Control Register A & B (TCCR2A, TCCR2B)
  TCCR2A = 0;
  TCCR2B = 0;

  // Clear TC2 Counter Value Register (TCNT2)
  TCNT2 = 0;
  
  // Write TC2 Control Registers to set Waveform Generation Mode (WGM) to Clear Timer on Compare Match (CTC) mode
  // WGM[2:0] = b010
  TCCR2A |= (1 << WGM21);
  
  // Write TC2 Control Register B to set Clock Select (CS) to clkIO/1024 (prescale by 1024)
  // CS[2:0] = b111
  TCCR2B |= (1 << CS20);
  TCCR2B |= (1 << CS21);
  TCCR2B |= (1 << CS22);

  // Set TC2 Output Compare Register A to 250
  OCR2A = 250;

  // Write TC2 Interrupt Mask Register to enable interrupt on Output Compare A Match
  // OCIE2A = b1
  TIMSK2 |= (1 << OCIE2A);

  // Enable interrupts
  sei();
}

ISR(TIMER2_COMPA_vect) {
  static volatile uint8_t appPrescaler;
  static volatile uint32_t runCounter = 0;
  appPrescaler++;
  if (appPrescaler >= CFG_TIMER2_APP_PRESCALER) {
    mAppTimer2InterruptFlag = true;
    
    fdc_pid_process_variable = mEncoder.read();
    FDC_PID_INTERRUPT_LOOP;
    mEncoder.write(0);
    
    appPrescaler = 0;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(CFG_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(CFG_BUTTON_LED_PIN, OUTPUT);
  digitalWrite(CFG_BUTTON_LED_PIN, LED_ON);

  mEscControlLer.attach(CFG_ESC_CONTROL_PIN);
  mEscControlLer.writeMicroseconds(CFG_ESC_CONTROL_NEUTRAL_ON_TIME);

  mEncoder.write(0); // Ensure encoder count start at 0
  
  timer2Setup();
  Serial.println("Test started");
}

#define LOG(x) Serial.print(x); Serial.print(" | ");
#define LOG_2(format, ...) sprintf(serialTxBuffer, format, __VA_ARGS__); Serial.print(serialTxBuffer);
#define CFG_HALF_NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW 5
#define NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW (2 * CFG_HALF_NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW)
#define LOWEST_ENCODER_COUNTS_THRESHOLD(average, interval) (average - (CFG_HALF_NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW - 2) * interval)

void loop() {
  static uint32_t counter[NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW] = {0};
  static uint32_t intervalMin = 0;
  static uint32_t intervalMax = 0;
  static uint32_t trackingAverage = 400;
  static uint32_t trackingInterval = 25;
  static uint32_t minimumThreshold = LOWEST_ENCODER_COUNTS_THRESHOLD(trackingAverage, trackingInterval);
  
  static ControlMode_t controlMode = CONTROL_MODE_PID;




  
  /* =================================== PROCESSING SERIAL INPUT =================================== */
  
  processSerialData();

  // Toggle control mode between PID and Forced
  if (mSerialInputType == INPUT_CHANGE_CONTROL_MODE) {
    if (controlMode == CONTROL_MODE_PID) {
      controlMode = CONTROL_MODE_FORCED;
    }
    else {
      controlMode = CONTROL_MODE_PID;
    }
  }

  // Change average value stored in Encoder counts History Table
  if (mSerialInputType == INPUT_AVERAGE) {
    Serial.print("AVG:   "); Serial.println(mSerialInputVal);
    trackingAverage = mSerialInputVal;
    for (int i = 0; i < NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW; i++) {
      counter[i] = 0;
    }
    minimumThreshold = LOWEST_ENCODER_COUNTS_THRESHOLD(trackingAverage, trackingInterval);
  }
  
  // Change interval in Encoder counts History Table
  if (mSerialInputType == INPUT_INTERVAL) {
    Serial.print("INTERVAL: "); Serial.println(mSerialInputVal);
    trackingInterval = mSerialInputVal;
    for (int i = 0; i < NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW; i++) {
      counter[i] = 0;
    }
    minimumThreshold = LOWEST_ENCODER_COUNTS_THRESHOLD(trackingAverage, trackingInterval);
  }

  if (controlMode == CONTROL_MODE_PID) {

    // Command Change Set Point
    if (mSerialInputType == INPUT_SET_POINT) {
      Serial.print("SET POINT: "); Serial.println(mSerialInputVal);
      fdc_pid_set_point = mSerialInputVal;
    }
#   if CFG_PID_TUNING

      // Change K_P
      if (mSerialInputType == INPUT_KP) {
        Serial.print("KP: "); Serial.println(mSerialInputVal);
        FDC_PID_KP = (CFG_PID_VAR_TYPE) mSerialInputVal / 1000;
      }

      // Change K_I
      if (mSerialInputType == INPUT_KI) {
        Serial.print("KI: "); Serial.println(mSerialInputVal);
        FDC_PID_KI = (CFG_PID_VAR_TYPE) mSerialInputVal / 1000;
      }

      // Change K_D
      if (mSerialInputType == INPUT_KD) {
        Serial.print("KD: "); Serial.println(mSerialInputVal);
        FDC_PID_KD = (CFG_PID_VAR_TYPE) mSerialInputVal / 1000;
      }

      // Change Integral term maximum
      if (mSerialInputType == INPUT_I_MAX) {
        Serial.print("FDC_PID_I_TERM_MAX: "); Serial.println(mSerialInputVal);
        FDC_PID_I_TERM_MAX = (CFG_PID_VAR_TYPE) mSerialInputVal;
      }
#   endif // CFG_PID_TUNING
  }
  else {

    // Force ESC control value
    if (mSerialInputType == INPUT_SERVO) {
      Serial.print("ESC: "); Serial.println(mSerialInputVal);
      mEscControlLer.writeMicroseconds(mSerialInputVal);
    }
  }
  mSerialInputType = INPUT_INVALID;

  



  /* =================================== CONTROL THE DAMNED CAR =================================== */
  
  if (mAppTimer2InterruptFlag) {
    BLINK_LED();
    if (controlMode == CONTROL_MODE_PID) {

//      // Display PID variable for tuning
//      LOG(fdc_pid_process_variable);
//      LOG(fdc_pid_control_variable);
//      LOG(fdc_pid_corrected_control_variable);
//      LOG(fdc_pid_error);
//      LOG(fdc_pid_last_error);
//      LOG(fdc_pid_p_term);
//      LOG(fdc_pid_i_term);
//      LOG(fdc_pid_d_term);
//      LOG(FDC_PID_KP);
//      LOG(FDC_PID_KI);
//      LOG(FDC_PID_KD);
//      LOG(FDC_PID_I_TERM_MAX);
//      Serial.println();

      // Adjust ESC based on PID output
      mEscControlLer.writeMicroseconds(fdc_pid_corrected_control_variable);
      
    }
    
    



    /* =================================== DISPLAY ENCODER COUNTS HISTORY =================================== */

    // Display the Encoder count History Table
    Serial.print("\n\n\n");
    
    // First interval, 0 to lowest threshold
    if (fdc_pid_process_variable > 0 && fdc_pid_process_variable < minimumThreshold) {
      counter[0]++;
    }
    LOG(0); LOG_2("%4d | ", 0); LOG_2("%4d | ", minimumThreshold); LOG_2("%4d\n", counter[0]);
    
    // Next interval through second to last interval
    intervalMax = minimumThreshold;
    for (uint32_t i = 1; i < (NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW - 1); i++) {
      intervalMin = intervalMax;
      intervalMax = intervalMin + trackingInterval;
      if (fdc_pid_process_variable >= intervalMin && fdc_pid_process_variable <= intervalMax) {
        counter[i]++;
      }
      LOG(i); LOG_2("%4d | ", intervalMin); LOG_2("%4d | ", intervalMax); LOG_2("%4d\n", counter[i]);
    }

    // Last interval, highest threshold to inf
    if (fdc_pid_process_variable >= intervalMax) {
      counter[NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW - 1]++;
    }
    LOG(NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW - 1); LOG_2("%4d | ", intervalMax); LOG("----"); LOG_2("%4d\n", counter[NUMBER_OF_ENCODER_COUNTS_HISTORY_TABLE_ROW - 1]);





    mAppTimer2InterruptFlag = false;
  }
}

void processSerialData(void) {
  static char rxChar;
  static uint32_t rxValue;
  static SerialInputType_t rxType;
  static SerialState_t serialState = SERIAL_STATE_GET_TYPE;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    if (serialState == SERIAL_STATE_GET_TYPE) {
      if (rxChar == 'f') {
        rxType = INPUT_SERVO;
      }
      else if (rxChar == 's') {
        rxType = INPUT_SET_POINT;
      }
      else if (rxChar == 'a') {
        rxType = INPUT_AVERAGE;
      }
      else if (rxChar == 'r') {
        rxType = INPUT_INTERVAL;
      }
      else if (rxChar == 'p') {
        rxType = INPUT_KP;
      }
      else if (rxChar == 'i') {
        rxType = INPUT_KI;
      }
      else if (rxChar == 'd') {
        rxType = INPUT_KD;
      }
      else if (rxChar == 'm') {
        rxType = INPUT_I_MAX;
      }
      else if (rxChar == 'x') {
        rxType = INPUT_CHANGE_CONTROL_MODE;
      }
      else {
        rxType = INPUT_INVALID;
      }
      serialState = SERIAL_STATE_GET_VALUE;
    }
    else {
      if (rxChar == '\n') {
        mSerialInputType = rxType;
        mSerialInputVal = rxValue;
        serialState = SERIAL_STATE_GET_TYPE;
        rxValue = 0;
      }
      else if ((rxChar >= '0') && (rxChar <= '9')) {
        rxValue *= 10;
        rxValue += rxChar - 0x30;
      }
    }
  }
}


