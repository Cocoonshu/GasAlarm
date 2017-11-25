#include "LEDInterpolater.h"

LEDInterpolater::LEDInterpolater(unsigned long duration) {
  mLastTime     = 0;
  mDuration     = duration;
  mNormDuration = 1;
  mSpeed        = 0;
  mPhase        = 0;
}

LEDInterpolater::~LEDInterpolater() {}

void LEDInterpolater::begin() {
  mLastTime = millis();
}

/**
 * F(t) = [cos(2 * pi * t / duration + pi) + 1.0] / 2.0, the unit is s
 */
double LEDInterpolater::interpolate(unsigned long now) {
  double deltaTime = (now - mLastTime);
  double deltaPhase = mSpeed * deltaTime;
  mPhase += deltaPhase;
  mCurrentValue = (cos(mPhase) + 1.0) * 0.5;
  mLastTime = now;
  return mCurrentValue;
}

void LEDInterpolater::setDuration(unsigned long duration) {
  mDuration = duration;
  mSpeed = 2 * PI / (double) duration;
}

float LEDInterpolater::apply(float min, float max) {
  return mCurrentValue * (max - min) + min;
}

