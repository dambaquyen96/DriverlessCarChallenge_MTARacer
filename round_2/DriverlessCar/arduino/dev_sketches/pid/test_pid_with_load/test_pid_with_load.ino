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


static Encoder mEncoder(CFG_ENCODER_PIN_B, CFG_ENCODER_PIN_A);
static Servo mEscControlLer;
static volatile bool mAppTimer2InterruptFlag = false;
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

void loop() {
  processSerialData();

  if (fdc_pid_set_point != mSerialInputVal) {
    fdc_pid_set_point = mSerialInputVal;
    Serial.print("s"); Serial.println(mSerialInputVal);
  }
  
  if (mAppTimer2InterruptFlag) {
    BLINK_LED();

    // // Display PID variable for tuning
    // LOG(fdc_pid_process_variable);
    // LOG(fdc_pid_control_variable);
    // LOG(fdc_pid_corrected_control_variable);
    // LOG(fdc_pid_error);
    // LOG(fdc_pid_last_error);
    // LOG(fdc_pid_p_term);
    // LOG(fdc_pid_i_term);
    // LOG(fdc_pid_d_term);
    // LOG(FDC_PID_KP);
    // LOG(FDC_PID_KI);
    // LOG(FDC_PID_KD);
    // LOG(FDC_PID_I_TERM_MAX);
    // Serial.println();

    if (fdc_pid_process_variable > 1.0) {
      Serial.print("e"); Serial.print(fdc_pid_process_variable); Serial.print(" | ");
      Serial.print("p"); Serial.print(fdc_pid_p_term); Serial.print(" | ");
      Serial.print("i"); Serial.print(fdc_pid_i_term); Serial.print(" | ");
      Serial.print("d"); Serial.print(fdc_pid_d_term); Serial.print(" | ");
      Serial.print("c"); Serial.print(fdc_pid_corrected_control_variable); Serial.println();
    }
    mEscControlLer.writeMicroseconds(fdc_pid_corrected_control_variable);
    
    mAppTimer2InterruptFlag = false;
  }
}

void processSerialData(void) {
  static char rxChar;
  static uint32_t rxValue;
  
  while(Serial.available()) {
    rxChar = (char) Serial.read();
    
    if (rxChar == '\n') {
      mSerialInputVal = rxValue;
      rxValue = 0;
    }
    else if ((rxChar >= '0') && (rxChar <= '9')) {
      rxValue *= 10;
      rxValue += rxChar - 0x30;
    }
  }
}


