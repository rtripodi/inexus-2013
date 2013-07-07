#include "Config.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Robot.h"
#include <Servo.h>
PololuQTRSensorsRC lineSensors = PololuQTRSensorsRC(QTR_SENSOR_PINS_LIST, QTR_NUM_PINS, QTR_TIMEOUT, QTR_EMITTER_PIN);
//alex try.  Here you go Alex.
Motor motors;
Robot robot(&motors);
const int LED_PIN = 13;

void setup()
{  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  motors.setup();
  robot.setup();
}

void loop()
{
  AlexTestingFunction();
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
