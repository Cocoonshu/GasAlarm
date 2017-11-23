#ifndef LEDInterpolater_H
#define LEDInterpolater_H
#include <Arduino.h>
#define DIFF_FACTOR 0.05

class LEDInterpolater {
  public:
    LEDInterpolater(unsigned long duration);
    ~LEDInterpolater();

    void   begin();
    double interpolate(unsigned long now);
    void   setDuration(unsigned long duration);
    float  apply(float min, float max);
    
  private:
    unsigned long mDuration;
    unsigned long mStartTime;
    unsigned long mNormDuration;
    double        mCurrentValue;
};

#endif
