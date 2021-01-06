#ifndef __TIMEPOINT_H__
#define __TIMEPOINT_H__

class TimePoint
{
  public:
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    TimePoint(uint8_t h, uint8_t m, uint8_t s) : hour(h), minute(m), second(s)
    { }
};

#endif