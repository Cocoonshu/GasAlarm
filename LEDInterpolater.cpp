#include "LEDInterpolater.h"

LEDInterpolater::LEDInterpolater(unsigned long duration) {
  mStartTime    = 0;
  mDuration     = duration;
  mNormDuration = 1;
}

LEDInterpolater::~LEDInterpolater() {}

void LEDInterpolater::begin() {
  mStartTime = millis();
}

/**
 * F(t) = [cos(2 * pi * t / duration + pi) + 1.0] / 2.0, the unit is s
 */
double LEDInterpolater::interpolate(unsigned long now) {
  double time = (now - mStartTime);
  mCurrentValue = (cos(2.0 * PI * time / (double)mDuration + PI) + 1.0) * 0.5;
  return mCurrentValue;
}

void LEDInterpolater::setDuration(unsigned long duration) {
  mDuration = duration;
}

float LEDInterpolater::apply(float min, float max) {
  return mCurrentValue * (max - min) + min;
}

