//#include <SoftwareSerial.h>  //For colour sensor
#include "Config.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Claw.h"
#include "IR.h"
#include "LineSensors.h"
//#include "Colour.h"
#include <Servo.h>
#include "Movement.h"
#include "Motor.h"
#include "MazeImports.h"
#include "GridMap.h"
#include "Routing.h"

Motor motor;
LineSensors ls;
Movement mover(&motor, &ls); 

void setup()
{
  Serial.begin(9600);
  motor.setup(); 
}

void loop()
{

motor.stop();
mover.moveLength(270,80);
motor.stop();

delay(500);

motor.stop();
mover.rotateAngle(90,80);
motor.stop();

delay(500);
}
