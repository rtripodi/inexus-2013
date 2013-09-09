#include "Config.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include <OldSoftwareSerial.h>
#include "Claw.h"
#include "IR.h"
#include "LineSensors.h"
#include <Servo.h>
#include "Movement.h"
#include "Motor.h"
#include "GridImports.h"
#include "GridMap.h"
#include "Routing.h"
#include "GridNav.h"
#include "Colour.h"
#include "ColourSoftware.h"

ColourSoftware colour;

MazeNav mazeNav(&motors, &mover, (IR**)&irs);

void setup()
{  
	Serial.begin(9600);	
	claw.setup();
	motors.setup();
	claw.shut();
	gridNav.findBlock();
}

void loop()
{
	colour.senseColour();
}
