#include <PololuWheelEncoders.h>
#include "Robot.h"

// TESTING

Motor motors;
Robot robot(&motors);
const int LED_PIN = 13;
//int foreward[5] = {1,2,3,4,5};

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  motors.setup();
  robot.setup();
}
//A test comment

void loop()
{
    Serial.print("Average Ticks");
    Serial.print("\t");
    Serial.print("Wheel Error");
    Serial.print("\t");    
    Serial.print("Left Wheel");
    Serial.print("\t");
    Serial.print("Right Wheel");
    Serial.print("\t");
    Serial.print("Difference");
  Serial.println();
  Serial.println(robot.errorTickAdjustment());
  
  
  
//  robot.sdebug();
//  motors.both(80,0);
//  delay(500);
//  motorstop();
/*  robot.leftMotorSpd(80);
  delay(500);
  motorstop();
  robot.rightMotorSpd(80);
  delay(500);
  motorstop();
  robot.rightMotorSpd(-80);
  delay(500);
  motorstop();
  robot.leftMotorSpd(-80);
  delay(500);
  motorstop();
  motors.both(-80,0);
  delay(500);
  motorstop();
*/
//  motors.both(0,0);
  robot.resetEncoders();
//  robot.sdebug();
  delay(1000);



  robot.moveTicks(120,50);
//  robot.sdebug();
  delay(1000);
  motors.both(0,0);
//  robot.sdebug();
  delay(1000);


  robot.resetEncoders();
//  robot.sdebug();
  delay(1000);



  robot.moveTicks(-120,50);
//  robot.sdebug();
  delay(1000);
  motors.both(0,0);
//  robot.sdebug();  
  delay(1000);



  robot.resetEncoders();
  robot.sdebug();
  delay(1000);


/*
  robot.moveTicks(10,-50);
  robot.sdebug();
  delay(1000);
  motors.both(0,0);
  robot.sdebug();  
  delay(1000);



  robot.resetEncoders();
  robot.sdebug();
  delay(1000);



  robot.moveTicks(-10,-50);
//  robot.sdebug();
  delay(1000);
  motors.both(0,0);
//  robot.sdebug();  
  delay(1000);

/*

  robot.rotateTicks(90,80);
  delay(500);
  motors.both(0,0); 
  delay(1000);
 
 
  robot.rotateTicks(90,-80);
  delay(500);
  motors.both(0,0); 
  delay(1000);
 
 
  robot.rotateTicks(-90,80);
  delay(500);
  motors.both(0,0); 
  delay(1000);
 
 
  robot.rotateTicks(-90,-80);
  delay(500);
  motors.both(0,0);
  delay(1000);
  
*/  
  motors.both(0,0);
}


//Sends back details from the encoders in the form:
//   average     adjustment    left motor   right motor
/*void robot.sdebug()
{
  foreward[0] = robot.getTicks();
  foreward[1] = robot.errorTickAdjustment();
  foreward[2] = robot.getLeftEncoderCounts();
  foreward[3] = robot.getRightEncoderCounts();
  foreward[4] = abs(robot.getLeftEncoderCounts() - robot.getRightEncoderCounts());
    for (int i = 0; i < 5; i++)
  {
    Serial.print(foreward[i]);
    Serial.print("\t");
    Serial.print("\t");
  }
  Serial.println();
}
*/
