#include "LEDInterpolater.h"
#include "LEDStepper.h"

#define MQ6_ADC_IN           17
#define MUTE_PIN             2
#define TEST_PIN             7
#define LED_R_PIN            5
#define LED_G_PIN            6
#define LED_B_PIN            9
#define ALARM_PIN            14

#define LED_R_MAX            0.4
#define LED_G_MAX            0.3
#define LED_B_MAX            0.15

#define WARNING_VOLT         2.5
#define ALARM_VOLT           3.0
#define WARM_UP_TIMEOUT      1000 * 15
#define SAMPLE_TIMEOUT       1000 * 1
#define COLOR_CHANGE_TIMEOUT 1000

String inputString;
int debugFlag = -1;
#define DISABLE -1
#define WARM    0
#define MONITOR 1
#define WARN    2
#define ALARM   3

struct UI {
  uint32_t color;
  uint16_t duration;
  float    minAlpha;
  float    maxAlpha;

  UI(uint32_t color, uint16_t duration, float minAlpha, float maxAlpha) {
    this->color    = color;
    this->duration = duration;
    this->minAlpha = minAlpha;
    this->maxAlpha = maxAlpha;
  }
};

UI sUI_WarmUp (0xFF3300, 6000, 0.5f, 0.5f); // Gold
UI sUI_Monitor(0xFFFFFF, 6000, 0.1f, 0.8f); // White
UI sUI_Warning(0xFF1800, 1000, 0.1f, 0.8f); // Oriange
UI sUI_Alarm  (0xFF0000,  500, 0.0f, 1.0f); // Red

unsigned long   mWarmingUpTime  = 0;
unsigned long   mLastSampleTime = 0;
LEDInterpolater mLEDInterpolater(sUI_WarmUp.duration);
LEDStepper      mLEDStepper(COLOR_CHANGE_TIMEOUT);

void setup() {
  Serial.begin(115200);
  pinMode(MQ6_ADC_IN, INPUT);
  pinMode(MUTE_PIN,   INPUT_PULLUP);
  pinMode(TEST_PIN,   INPUT_PULLUP);
  pinMode(LED_R_PIN,  OUTPUT);
  pinMode(LED_G_PIN,  OUTPUT);
  pinMode(LED_B_PIN,  OUTPUT);
  pinMode(ALARM_PIN,  OUTPUT);
  analogWrite(LED_R_PIN, 0);
  analogWrite(LED_G_PIN, 0);
  analogWrite(LED_B_PIN, 0);
  analogWrite(ALARM_PIN, 0);
  mLEDInterpolater.begin();

  Serial.println("MQ-6 ready, warming up...");
  mWarmingUpTime = millis();
}

void loop() {
  unsigned long now = millis();
  float volt = adcSampling(now);
  updateRGBLight(now, volt);
  updateAlarm(now, volt);
}

float adcSampling(unsigned long now) {
  if (abs(now - mWarmingUpTime) < WARM_UP_TIMEOUT) {
    return;
  }
  if (abs(now - mLastSampleTime) < SAMPLE_TIMEOUT) {
    return;
  } else {
    mLastSampleTime = now;
  }

  int adc = analogRead(MQ6_ADC_IN);
  float volt = adc * 5.0f / 1024.0f;
  //Serial.print("MQ6: "); Serial.print(volt); Serial.println("v");
  return volt;
}

void updateRGBLight(unsigned long now, float volt) {
  mLEDInterpolater.interpolate(now);
  mLEDStepper.interpolate(now);

  float    currentAlpha = 0;
  uint32_t currentColor = 0;
  uint8_t  currentRed   = 0;
  uint8_t  currentGreen = 0;
  uint8_t  currentBlue  = 0;
  bool     isWarmingUp  = now - mWarmingUpTime < WARM_UP_TIMEOUT;

  if (debugFlag != DISABLE) {
    isWarmingUp = (debugFlag == WARM);
    volt = (debugFlag == MONITOR) ? 1.0 : volt;
    volt = (debugFlag == WARN)    ? 2.7 : volt;
    volt = (debugFlag == ALARM)   ? 3.5 : volt;

    Serial.print("[DEBUG] volt = ");Serial.println(volt);
  }
  
  if (isWarmingUp) {
    mLEDInterpolater.setDuration(sUI_WarmUp.duration);
    currentAlpha = mLEDInterpolater.apply(sUI_WarmUp.minAlpha, sUI_WarmUp.maxAlpha);
    currentColor = sUI_WarmUp.color;
  } else if (volt < WARNING_VOLT) {
    mLEDInterpolater.setDuration(sUI_Monitor.duration);
    currentAlpha = mLEDInterpolater.apply(sUI_Monitor.minAlpha, sUI_Monitor.maxAlpha);
    currentColor = sUI_Monitor.color;
  } else if (volt < ALARM_VOLT) {
    mLEDInterpolater.setDuration(sUI_Warning.duration);
    currentAlpha = mLEDInterpolater.apply(sUI_Warning.minAlpha, sUI_Warning.maxAlpha);
    currentColor = sUI_Warning.color;
  } else {
    mLEDInterpolater.setDuration(sUI_Alarm.duration);
    currentAlpha = mLEDInterpolater.apply(sUI_Alarm.minAlpha, sUI_Alarm.maxAlpha);
    currentColor = sUI_Alarm.color;
  } 

  mLEDStepper.gotoDestination(currentColor);
  currentColor = mLEDStepper.getCurrent();
  currentRed   = ((currentColor & 0xFF0000) >> 16) * currentAlpha * LED_R_MAX;
  currentGreen = ((currentColor & 0x00FF00) >> 8 ) * currentAlpha * LED_G_MAX;
  currentBlue  = ((currentColor & 0x0000FF) >> 0 ) * currentAlpha * LED_B_MAX;
  analogWrite(LED_R_PIN, currentRed);
  analogWrite(LED_G_PIN, currentGreen);
  analogWrite(LED_B_PIN, currentBlue);
}

void updateAlarm(unsigned long now, float volt) {

}

/**
 * Debug interface
 */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      inputString.trim();
      if (inputString.equalsIgnoreCase("warm")) {
        debugFlag = WARM;
      } else if (inputString.equalsIgnoreCase("monitor")) {
        debugFlag = MONITOR;
      } else if (inputString.equalsIgnoreCase("alarm")) {
        debugFlag = ALARM;
      } else if (inputString.equalsIgnoreCase("warn")) {
        debugFlag = WARN;
      } else {
        debugFlag = DISABLE;
      }
      Serial.println(inputString);
      inputString = "";
    }
  }
}
