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
	claw.open();
	delayTillButton();
	for (int ii = 0; ii <= 100; ii++)
	{
		ls.calibrate();
		delay(5);
	}
	delayTillButton();*/
	gridTest();
}

void loop() {}

float diffs=0;
float prevPos = 0;
void wallsAreScary()
{
/*
get left and right IR readings
if it's too close to the left wall and it's not already moving in the direction of the right
slow the right motor

if it's too close to the right wall and it's not already moving in the direction of the left
slow the left motor
*/
int leftDist = lftIr.getDist();
int rightDist = rghtIr.getDist();
float posBetweenWalls = (float)leftDist/(float)(leftDist+rightDist);//value between 0 and 1, 0 being the left wall 1 being the right
diffs = posBetweenWalls - prevPos; //+ve value means moving towards right, it's actually in mm/readingtime
prevPos = posBetweenWalls;
if ((posBetweenWalls>0.5)&&(diffs>0))
  {
  motors.left(40);
  motors.right(80);
  }
if ((posBetweenWalls<0.5)&&(diffs<0))
  {
  motors.left(80);
  motors.right(40);
  }
if ((posBetweenWalls>0.5)&&(diffs<0))
  {
  motors.left(60);
  motors.right(80);
  }
if ((posBetweenWalls<0.5)&&(diffs>0))
  {
  motors.left(80);
  motors.right(60);
  }
if (diffs>0.2||diffs<-0.2)
{
  motors.left(80);
  motors.right(80);
}
if (posBetweenWalls<0.4)
  {
  motors.left(40);
  motors.right(0);
  }
if (posBetweenWalls>0.6)
  {
  motors.left(0);
  motors.right(40);
  }
}