/*
 * TODO: Fix random moveticks issue.
 * The Robot class is here to represent what a robot can do.
 * The robot code is based on two wheels/encoders and a motor controller.
 */
 
#ifndef Robot_h
#define Robot_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <PololuWheelEncoders.h>

#define MotorControl Serial1
/*
#define TURN_SPEED  40  
#define STRAIGHT_SPEED 30  
#define APPROACHSPEED 30  
*/

class Robot
{
  public:
/*****************************************************************************/
/*  Initialisation                                                                   */
    Robot();
    void setupSerialComms();
    void sdebug();
/*****************************************************************************/
/*****************************************************************************/	
    
  /* Direct motor control */
    void bothMotorSpd(int motorSpeed, int error);//Adjust the speed of both motors, positive speed goes forwards, negative goes backwards
    void leftMotorSpd(int motorSpeed);//Positive for forward, negative for backwards.
    void rightMotorSpd(int motorSpeed);//Positive for forward, negative for backwards.
    void motorStop();
    
  /* Wheel encoders */
    void resetEncoders();
    int getLeftEncoderCounts();
    int getRightEncoderCounts();
    int getTicks(); //returns the average of the ticks on both wheels
    void moveTicks(int ticks, int motorSpeed);// Move the number of ticks. We do not stop when we have hit the ticks.
    void rotateTicks(int ticks, int motorSpeed);// rotate the number of ticks.  It will stop when the ticks are reached.
    
  /* Error adjustment */
    int errorAdjustment(bool onlyTick=false);//Uses tick and sensor error to provide an error for following lines. Between -20 and 20.
    int errorTickAdjustment();//Diference in ticks between wheels, can be positive or negative.  Constrained -5 to 5.
/*****************************************************************************/	
  private:
    int lastError;
    int errorSum;
    
    PololuWheelEncoders wheelEnc;
    
/* Serial comms supporting functions */
    int limit_0_to_127(int val);// Limits val between 0 and 127.
    
  protected:
};

#endif
