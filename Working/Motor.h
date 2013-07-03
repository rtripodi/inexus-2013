/*
  Motor.h - Library for controlling the Polou Motor Controller.
  Created by Alex Ahern, May 7, 2013.
*/

#ifndef Motor_h

#include "Arduino.h"
#define MotorControl Serial1
//quick test.

class Motor
{
  public:
  /* Direct motor control */
    Motor();
    void setup();
    void both(int motorSpeed, int error);//Adjust the speed of both motors, positive speed goes forwards, negative goes backwards
    void left(int motorSpeed);//Positive for forward, negative for backwards.
    void right(int motorSpeed);//Positive for forward, negative for backwards.
    void stop();
  private:
  /* Serial comms supporting functions */
    int limit_0_to_127(int val);// Limits val between 0 and 127.
};

#endif

