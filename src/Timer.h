#pragma once

class Timer
{
private:
  unsigned long time;
  unsigned long activationTime;

public:
  bool isActivated;

  Timer(unsigned long);

  bool read();

  void reset();

  void setTime(unsigned long);
};