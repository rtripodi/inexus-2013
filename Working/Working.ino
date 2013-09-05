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
#include "MazeImports.h"
#include "GridMap.h"
#include "Routing.h"
#include "GridNav.h"
#include "Colour.h"
#include "ColourSoftware.h"

ColourSoftware colour;

void setup()
{  
	Serial.begin(9600);	
	colour.setup();
	Serial.println("Calibrating black in 2 secs.");
	delay(2000);
	colour.calibrateBlack();
	Serial.println("Calibrating white in 2 secs.");
	delay(2000);
	colour.calibrateWhite();
	Serial.println("D");
}

void loop()
{
	colour.senseColour();
}
