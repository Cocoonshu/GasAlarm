#ifndef LEDStepper_H
#define LEDStepper_H
#include <Arduino.h>

class LEDStepper {
  public:
    LEDStepper(long duration);
    ~LEDStepper();

    void setDuration(long duration);
    void gotoDestination(uint32_t color);
    uint32_t getCurrent();
    float interpolate(unsigned long now);

  private:
    inline uint32_t getColorRed(uint32_t color) {
      return (color & 0xFF0000) >> 16;
    }
    
    inline uint32_t getColorGreen(uint32_t color) {
      return (color & 0x00FF00) >> 8;
    }

    inline uint32_t getColorBlue(uint32_t color) {
      return (color & 0x0000FF) >> 0;
    }

  private:
    long mStartTime;
    long mDuration;
    uint32_t mStartColor;
    uint32_t mEndColor;
    uint32_t mCurrentColor;
};

#endif
