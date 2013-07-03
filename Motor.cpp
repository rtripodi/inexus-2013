/*
  Motor.h - Library for controlling the Polou Motor Controller.
  Created by Alex Ahern, May 7, 2013.
*/

#include "Arduino.h"
#include "Motor.h"

 Motor::Motor()
{
Serial1.begin(38400);
  Serial1.write(0xAA);
}
  
/* MOTOR DRIVE CONFIG */
void Motor::leftDrive(int i)
{
  if ( i >= 0 ){
    if ( i > 127 ){
      i = 127;  
    }
    Serial1.write(0x88);
    Serial1.write(i);
  }
  else if (i < 0){
    if ( i < -127 ){
      i = -127;  
    }
    i = i * (-1);
    Serial1.write(0x8A);
    Serial1.write(i);
  }
}

void Motor::rightDrive(int i)
{
  if ( i >= 0 ){
    if ( i > 127 ){
      i = 127;  
    }
    Serial1.write(0x8C);
    Serial1.write(i);
  }
  else if (i < 0){
    if ( i < -127 ){
      i = -127;  
    }
    i = i * (-1);
    Serial1.write(0x8E);
    Serial1.write(i);
  }
}

void Motor::bothDrive(int i)
{
  if ( i >= 0 ){
    if ( i > 127 ){
      i = 127;  
    }
    Serial1.write(0x88);
    Serial1.write(i);
    Serial1.write(0x8C);
    Serial1.write(i);
  }
  else if (i < 0){
    if ( i < -127 ){
      i = -127;  
    }
    i = i * (-1);
    Serial1.write(0x8A);
    Serial1.write(i);
    Serial1.write(0x8E);
    Serial1.write(i);
  }
}

