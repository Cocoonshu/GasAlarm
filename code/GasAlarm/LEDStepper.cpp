#include "LEDStepper.h"

LEDStepper::LEDStepper(long duration) {
  mStartTime = 0;
  mDuration = duration;
  mStartColor = mEndColor = mCurrentColor = 0;
}

LEDStepper::~LEDStepper() {

}

void LEDStepper::setDuration(long duration) {
  mDuration = duration;
}

void LEDStepper::gotoDestination(uint32_t color) {
  if (mEndColor != color) {
    mStartTime = millis();
    mStartColor = mCurrentColor;
    mEndColor = color;
  }
}

uint32_t LEDStepper::getCurrent() {
  return mCurrentColor;
}

float LEDStepper::interpolate(unsigned long now) {
  long duration         = now - mStartTime;
  int  destinationRed   = getColorRed(mEndColor);
  int  destinationGreen = getColorGreen(mEndColor);
  int  destinationBlue  = getColorBlue(mEndColor);
  int  startRed         = getColorRed(mStartColor);
  int  startGreen       = getColorGreen(mStartColor);
  int  startBlue        = getColorBlue(mStartColor);

  if (duration <= 0) {
    mCurrentColor = mStartColor;
    return 0;
  } else if (duration >= mDuration) {
    mCurrentColor = mEndColor;
    return 1;
  } else {
    float    progress     = duration / (float) mDuration;
    uint32_t currentRed   = progress * (destinationRed - startRed) + startRed;
    uint32_t currentGreen = progress * (destinationGreen - startGreen) + startGreen;
    uint32_t currentBlue  = progress * (destinationBlue - startBlue) + startBlue;
    mCurrentColor = ((currentBlue  <<  0) & 0x0000FF) |
                    ((currentGreen <<  8) & 0x00FF00) |
                    ((currentRed   << 16) & 0xFF0000);
    return progress;
  }
}

