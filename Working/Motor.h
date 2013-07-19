/*
  Motor.h - Library for controlling the Polou Motor Controller.
  Created by Alex Ahern, May 7, 2013.
  Updated by Toby Scantlebury, July 3, 2013.
*/

#ifndef Motor_h
#define Motor_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Config.h"

class Motor
{
  public:
    Motor();
	
    //Init connection to motor controller.
    //Run in setup() function.
    void setup();
	
    //Adjust speed of both motors.
    //Positive for forward, negative for backwards. 
    //motorSpeed will be constrained between 0 and 127.
    void both(int motorSpeed, int error = 0);
	
    //Positive for forward, negative for backwards. 
    //motorSpeed will be constrained between 0 and 127.
    void left(int motorSpeed);
	
    //Positive for forward, negative for backwards. 
    //motorSpeed will be constrained between 0 and 127.
    void right(int motorSpeed);
	
    //Stops both motors.
    void stop();
	
  private:
    //Returns val if it is between 0 and 127.
    //If less than 0, it returns 0.
    //If greater than 127, it returns 127.
    int limit_0_to_127(int val);
};

#endif

