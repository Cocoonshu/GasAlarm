#include "Sounder.h"

Alarm AlarmSin(1000, 0);
Alarm AlarmTriangle(1000, 1000);
Alarm AlarmRectangle(1000, 500);

Sounder::Sounder(int pin) {
  mSoundPin = pin;
  pinMode(mSoundPin, OUTPUT);
  mAlarmType = ALARM_TYPE_SIN;
}

Sounder::~Sounder() {
  analogWrite(mSoundPin, 0);
  mSoundPin = 0;
}

void Sounder::setType(AlarmType alarmType) {
  mAlarmType = alarmType;
}

void Sounder::begin() {
  if (!mEnableSound) {
    mEnableSound = true;
    mIsSounding = true;
    mStartTime = millis();
  }
}

void Sounder::stop() {
  mEnableSound = false;
}

uint8_t Sounder::interpolate(unsigned long now) {
  uint8_t pwm = 0;
  if (!mEnableSound) {
    pwm = 0;
  } else {
    switch (mAlarmType) {
      case ALARM_TYPE_SIN:        pwm = soundSin(now);      break;
      case ALARM_TYPE_TRIANGLE:   pwm = soundTriangle(now); break;
      case ALARM_TYPE_RECTRANGLE: pwm = soundRectangle(now);break;
      default:
        pwm = 0;
        break;
    }
  }

  analogWrite(mSoundPin, pwm);
  return pwm;
}

uint8_t Sounder::soundSin(unsigned long now) {
  /**
     F(t) = [cos(2 * pi * t / duration + pi) + 1.0] / 2.0, the unit is s
     Range in [0.0 1.0]
  */
  double duration = now - mStartTime;
  double value = (cos(2.0 * PI * duration / (double)AlarmSin.duration + PI) + 1.0) * 0.5;
  uint8_t pwm = value * 255;
  return pwm;
}

uint8_t Sounder::soundTriangle(unsigned long now) {
  unsigned long duration = now - mStartTime;
  return 0;
}

uint8_t Sounder::soundRectangle(unsigned long now) {
  unsigned long duration = now - mStartTime;
  return 0;
}


