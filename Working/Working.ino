#include "Config.h"
#include <SoftwareSerial.h>
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

Motor motors;
LineSensors ls;

Movement mover(&motors, &ls);

IR irs[4] = {
	IR(IR_SHORT_PIN, IR::shortRange),
	IR(IR_MEDIUMR_PIN, IR::mediumRange),
	IR(IR_MEDIUMB_PIN, IR::mediumRange),
	IR(IR_MEDIUML_PIN, IR::mediumRange)
};

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);

GridNav gridNav(&motors, &mover, (IR**)&irs, &claw);

void setup()
{  
	Serial.begin(9600);	
	claw.setup();
	motors.setup();
	gridNav.findBlock();
}

void loop() {}
