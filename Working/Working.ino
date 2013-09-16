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
#include "MazeMap.h"
#include "Colour.h"
#include "ColourSoftware.h"

#define WALL_MIN (175)
#define WALL_TOLERANCE (25)

Motor motors;
LineSensors ls;
Movement mover(&motors, &ls);

MazeMap mazeMap;

typedef struct {
	int frnt;
	int rght;
	int bck;
	int lft;
} IrValues;

IrValues irInMm;

IR frntIr = IR(IR_FRONT_PIN, IR::shortRange);
IR rghtIr = IR(IR_RIGHT_PIN, IR::mediumRange);
IR bckIr = IR(IR_BACK_PIN, IR::mediumRange);
IR lftIr = IR(IR_LEFT_PIN, IR::mediumRange);
/*
IrSensors irs = {
	&IR(IR_FRONT_PIN, IR::shortRange),
	&IR(IR_RIGHT_PIN, IR::mediumRange),
	&IR(IR_BACK_PIN, IR::mediumRange),
	&IR(IR_LEFT_PIN, IR::mediumRange)
};*/

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);

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

void setup()
{  
	Serial.begin(9600);	
	claw.setup();
	motors.setup();
	claw.shut();
}

void loop()
{
	delayTillButton();
	firstNavigate();
}

//Navigate and map maze from starting point
void firstNavigate()
{
	RelDir turn;
	
	searchInitEntrance();
	while (1) //DEBUG: Need exit condition
	{
		searchNextMaze();
		scanWalls();
		turn = alwaysLeft();
		mazeMap.updateMap(turn);
	}
}

//Returns true if a wall is in given relative direction based on IR values
bool isWall(RelDir relDir)
{
	if (relDir == FRONT)
		return (irInMm.frnt < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == RIGHT)
		return (irInMm.rght < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == BACK)
		return (irInMm.bck < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == LEFT)
		return (irInMm.lft < WALL_MIN + WALL_TOLERANCE);
}

//Updates the IR values
void scanWalls()
{
	irInMm.frnt = frntIr.getDist();
	irInMm.rght = rghtIr.getDist();
	irInMm.bck = bckIr.getDist();
	irInMm.lft = lftIr.getDist();
}

//From starting point, search for entrance
void searchInitEntrance()
{
	scanWalls();
	if (isWall(FRONT))
	{
		mover.rotateDirection(RIGHT, DEFAULT_SPEED);
		while (isWall(LEFT))
		{
			mover.moveForward(DEFAULT_SPEED);
			scanWalls();
		}
		mover.moveLength(170, DEFAULT_SPEED);
		motors.stop();
		mover.rotateDirection(LEFT, DEFAULT_SPEED);
	}
}

void searchNextMaze()
{
	mover.moveLength(170, DEFAULT_SPEED);
	motors.stop();
	scanWalls();
	while (!isWall(FRONT) && isWall(RIGHT) && isWall(LEFT))
	{
		mover.moveForward(DEFAULT_SPEED);
	}
	if (!isWall(FRONT))
	{
		mover.moveLength(170, DEFAULT_SPEED);
		motors.stop();
	}
}

//Separated to allow easier change to another turning scheme
RelDir alwaysLeft()
{
	if (!isWall(LEFT))
		return LEFT;
	else if (!isWall(FRONT))
		return FRONT;
	else if (!isWall(RIGHT))
		return RIGHT;
	else
		return BACK;
}
