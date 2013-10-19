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
#include "GridNav.h"
#include "Routing.h"
#include "MazeNav.h"
#include "Colour.h"
#include "ColourSoftware.h"

Motor motors;
LineSensors ls;
Movement mover(&motors, &ls);

MazeMap mazeMap;

IrValues irInMm;

IR frntIr = IR(IR_FRONT_PIN, IR::front);
IR rghtIr = IR(IR_RIGHT_PIN, IR::right);
IR bckIr = IR(IR_BACK_PIN, IR::back);
IR lftIr = IR(IR_LEFT_PIN, IR::left);

IrSensors irs = {
	&frntIr,
	&rghtIr,
	&bckIr,
	&lftIr
};

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);

GridNav gridNav(&motors, &mover, &irs, &claw);
MazeNav mazeNav(&motors, &mover, &irs);

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

void gridTest()
{
	Serial.println("\t\t\t\t\t............");
	Serial.println("\t\t\t\t\tFIRST BLOCK");
	Serial.println("\t\t\t\t\t............");
	gridNav.findBlock();
//	delayTillButton();
	Serial.println("\t\t\t\t\t............");
	Serial.println("\t\t\t\t\tSECOND BLOCK");
	Serial.println("\t\t\t\t\t............");
	gridNav.findBlock();
//	delayTillButton();
	Serial.println("\t\t\t\t\t............");
	Serial.println("\t\t\t\t\tTHIRD BLOCK");
	Serial.println("\t\t\t\t\t............");
	gridNav.findBlock();
}

void setup()
{  
	Serial.begin(9600);
/*	claw.setup();
	motors.setup();
	claw.shut();
	delayTillButton();
	for (int ii = 0; ii <= 100; ii++)
	{
		ls.calibrate();
		delay(5);
	}
	delayTillButton();
	gridTest();*/
	mazeNav.firstNavigate();
}

void loop() {}

float diffs=0;
float prevPos = 0.5;
void wallsAreScary()
{
  delay(100);
/*
get left and right IR readings
if it's too close to the left wall and it's not already moving in the direction of the right
slow the right motor

if it's too close to the right wall and it's not already moving in the direction of the left
slow the left motor
*/
int leftDist = lftIr.getDist();
int rightDist = rghtIr.getDist();
int speed = 80;
float posBetweenWalls = (float)leftDist/(float)(leftDist+rightDist);//value between 0 and 1, 0 being the left wall 1 being the right
diffs = posBetweenWalls - prevPos; //+ve value means moving towards right, it's actually in mm/readingtime
prevPos = posBetweenWalls;
		//If the line is within a margin of EDGE_SENSITIVITY
		if (abs(posBetweenWalls-0.5)<0.05)
		{
			if (diffs<-0.1)
			{
				motors.left(speed);
				motors.right(speed-speed/4);
			}
			if (diffs>0.1)
			{
				motors.right(speed);
				motors.left(speed-speed/4);
			}
			if (abs(diffs)<0.1)
			{
				motors.both(speed);
			}
		}
		//Otherwise, if it's right on the edge of the sensors
		else if (posBetweenWalls>0.6)
		{
			motors.left(speed/4);
			motors.right(speed);
		}
		else if (posBetweenWalls<0.4)
		{
			motors.left(speed);
			motors.right(speed/4);
		}
		//Else if it's off center but not too bad
		else if (posBetweenWalls-0.5>0.1)
		{
			motors.right(speed);
			motors.left(speed/2);
		}
		else if (posBetweenWalls-0.5<-0.1)
		{
			motors.right(speed/2);
			motors.left(speed);
		}
}
