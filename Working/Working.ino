#include "Config.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Claw.h"
#include "IR.h"
#include "LineSensors.h"
#include <Servo.h>
#include "Movement.h"
#include "Motor.h"
#include "MazeImports.h"
#include "GridMap.h"
#include "Routing.h"
#include "GridNav.h"
#include "Colour.h"

Colour colour;

void setup()
{  
	Serial.begin(9600);	
	colour.setup();
}

void loop() {}
