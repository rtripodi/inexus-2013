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
#include "MazeNav.h"
#include "Colour.h"
#include "ColourSoftware.h"

Motor motors;
LineSensors ls;
Movement mover(&motors, &ls);

void delayTillButton()
{
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN,HIGH);
	pinMode(PUSH_PIN, INPUT);
	digitalWrite(PUSH_PIN, HIGH);
	while(digitalRead(PUSH_PIN) == HIGH)
	{
		delay(500); 
	}
	digitalWrite(LED_PIN, LOW);
}

/*IR frntIr = IR(IR_FRONT_PIN, IR::shortRange);
IR rghtIr = IR(IR_RIGHT_PIN, IR::mediumRange);
IR bckIr = IR(IR_BACK_PIN, IR::mediumRange);
IR lftIr = IR(IR_LEFT_PIN, IR::mediumRange);*/

IrSensors irs = {
	&IR(IR_FRONT_PIN, IR::shortRange),
	&IR(IR_RIGHT_PIN, IR::mediumRange),
	&IR(IR_BACK_PIN, IR::mediumRange),
	&IR(IR_LEFT_PIN, IR::mediumRange)
};

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);

GridNav gridNav(&motors, &mover, &irs, &claw);

void setup()
{  
	Serial.begin(9600);	
	claw.setup();
	motors.setup();
	claw.shut();
	delayTillButton();
	for (int ii = 0; ii < 300 ; ++ii)
	{
		ls.calibrate();
	}
}

void loop()
{
	delayTillButton();
	gridNav.findBlock();
}
