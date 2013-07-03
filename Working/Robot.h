/*
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
#include "Motor.h"

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
    Robot(Motor * inMotors);
    void setup();
    void sdebug();
/*****************************************************************************/
/*****************************************************************************/	
    
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
    Motor *motors;
};

#endif
