#include "Timer.h"
#include <Arduino.h>

Timer::Timer(unsigned long time) : time(time), isActivated(false), activationTime(0)
{
}

bool Timer::read()
{
  if (isActivated) {
    if ((millis() - activationTime) > time) {
      return true;
    }
  }
  else {
    isActivated = true;
    activationTime = millis();
  }
  return false;
}

void Timer::reset()
{
  isActivated = false;
}

void Timer::setTime(unsigned long time)
{
  this->time = time;
}
