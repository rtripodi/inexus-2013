//Pins used for input from QTR sensors
#define QTR_SENSOR_PINS_LIST ((unsigned char[]) {22,23,24,25,26,27,28,29})

//Number of input pins for QTR sensors
#define QTR_NUM_PINS (8)

//At 16MHz you divide this number by 2 to get the number of microseconds.
//if QTR_TIMEOUT = 4000 then it takes a maximum of 2000us for the class
//to read the sensors (Advice is to use between 1000-3000us.)
#define QTR_TIMEOUT (4000)

//Pin to control infrared emitter on sensors
#define QTR_EMITTER_PIN (39)

#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Robot.h"
#include <Servo.h>
PololuQTRSensorsRC lineSensors = PololuQTRSensorsRC(QTR_SENSOR_PINS_LIST, QTR_NUM_PINS, QTR_TIMEOUT, QTR_EMITTER_PIN);
//alex try
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
