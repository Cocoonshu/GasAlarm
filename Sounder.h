#ifndef Sounder_H
#define Sounder_H
#include <Arduino.h>

struct Alarm {
  uint32_t duration;
  uint32_t interval;
  Alarm(uint32_t duration, uint32_t interval) {
    this->duration = duration;
    this->interval = interval;
  }
};

enum AlarmType {
  ALARM_TYPE_SIN,
  ALARM_TYPE_TRIANGLE,
  ALARM_TYPE_RECTRANGLE
};

class Sounder {
  public:
    Sounder(int pin);
    ~Sounder();

    void setType(AlarmType alarmType);
    void begin();
    void stop();
    uint8_t interpolate(unsigned long now);

  protected:
    uint8_t soundSin(unsigned long now);
    uint8_t soundTriangle(unsigned long now);
    uint8_t soundRectangle(unsigned long now);

  private:
    AlarmType mAlarmType;
    int mSoundPin;
    bool mEnableSound;
    bool mIsSounding;
    unsigned long mStartTime;
};

#endif
