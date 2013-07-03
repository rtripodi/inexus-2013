/*
  Motor.h - Library for controlling the Polou Motor Controller.
  Created by Alex Ahern, May 7, 2013.
*/

#ifndef Motor_h

#include "Arduino.h"

class Motor
{
  public:
	 Motor();
    void leftDrive(int i);
    void rightDrive(int i);
    void bothDrive(int i);
};

#endif

