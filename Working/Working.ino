#include "Config.h"
#include "LineSensors.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Robot.h"
#include <Servo.h>

Motor motors;
Robot robot(&motors);
const int LED_PIN = 13;

LineSensors ls;
void lineFollowDemoSetup();
void lineFollowDemo();

void setup()
{  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  motors.setup();
  robot.setup();
  lineFollowDemoSetup();
}

void loop()
{
  lineFollowDemo();
}

void lineFollowDemoSetup()
{
  ls.calibrate();
  delay(4000);
  ls.calibrate();
}

//Note, this is NOT the best way to do it.  Just a quick example of how to use the class.
void lineFollowDemo()
{
  LineSensor_ColourValues leftWhite[8] = {NUL,NUL,WHT,NUL,NUL,NUL,NUL,NUL};
  LineSensor_ColourValues rightWhite[8] = {NUL,NUL,NUL,NUL,NUL,WHT,NUL,NUL};
  LineSensor_ColourValues allBlack[8] = {BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK};
  if(ls.see(leftWhite)){
    motors.left(30);
    motors.right(0);
  }
  if(ls.see(rightWhite)){
    motors.right(30);
    motors.left(0);
  }
  if(ls.see(allBlack)){
    motors.stop();
  }
}

void AlexTestingFunction()
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
  robot.resetEncoders();
  delay(1000);
  robot.moveTicks(120,50);
  motors.both(0,0);//motors.stop()
  delay(1000);
  robot.resetEncoders();
  delay(1000);
  robot.moveTicks(-120,50);
  motors.both(0,0);//motors.stop()
  delay(1000);
  robot.resetEncoders();
  robot.sdebug();
  delay(1000);
}
