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
IR rghtIr = IR(IR_RIGHT_PIN, IR::left);
IR bckIr = IR(IR_BACK_PIN, IR::back);
IR lftIr = IR(IR_LEFT_PIN, IR::left);
/*
IrSensors irs = {
	&IR(IR_FRONT_PIN, IR::shortRange),
	&IR(IR_RIGHT_PIN, IR::mediumRange),
	&IR(IR_BACK_PIN, IR::mediumRange),
	&IR(IR_LEFT_PIN, IR::mediumRange)
};*/

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
	Serial.println("\t\t\t\t\t............");
	Serial.println("\t\t\t\t\tSECOND BLOCK");
	Serial.println("\t\t\t\t\t............");
	gridNav.findBlock();
	Serial.println("\t\t\t\t\t............");
	Serial.println("\t\t\t\t\tTHIRD BLOCK");
	Serial.println("\t\t\t\t\t............");
	gridNav.findBlock();
}

void setup()
{  
	Serial.begin(9600);
	gridTest();
/*	claw.setup();
	motors.setup();
	claw.shut();
        delayTillButton();*/
}

void loop()
{
//	scanWalls();
  /*Serial.print(irInMm.frnt);
  Serial.print("\t");
  Serial.print(irInMm.rght);
  Serial.print("\t");
  Serial.print(irInMm.bck);
  Serial.print("\t");
  Serial.println(irInMm.lft);*/
//  Serial.print(analogRead(IR_FRONT_PIN));
//  Serial.print("\t");
//  Serial.print(analogRead(IR_RIGHT_PIN));
//  Serial.print("\t");
//  Serial.print(analogRead(IR_BACK_PIN));
//  Serial.print("\t");
//  Serial.println(analogRead(IR_LEFT_PIN));
//	wallsAreScary();

}
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






/*
	Maze Navigation Class
*/

/*	|       |
	|       '-------.
	|               |
	|       |<----->| = 350mm wall minimum (full)
	|               |
	|       .-------'
	|       |
	
	|       |
	|       |
	|   |<->| = ~175mm wall minimum (half)
	|       |
	|       |
	
	|       |
	|       '-------.
	|               |
	|   |<--------->| ~175mm + 350mm = ~525mm mmax wall
	|               |
	|       .-------'
	|       |
*/

#define WALL_MIN_FULL (350)
#define WALL_MIN_HALF (WALL_MIN_FULL / 2)
#define WALL_TOLERANCE (15)

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
		return (irInMm.frnt < WALL_MIN_FULL + WALL_TOLERANCE);
	else if (relDir == BACK)
		return (irInMm.bck < WALL_MIN_HALF + WALL_TOLERANCE);
	else if (irInMm.rght + irInMm.lft > 2 * WALL_MIN_HALF)
	{
		if (relDir == LEFT)
			return (irInMm.lft < WALL_MIN_HALF + WALL_TOLERANCE);
		else if (relDir == RIGHT)
			return (irInMm.rght < WALL_MIN_HALF + WALL_TOLERANCE);
	}
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
		mover.moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
		motors.stop();
		mover.rotateDirection(LEFT, DEFAULT_SPEED);
	}
}

void searchNextMaze()
{
	mover.moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
	motors.stop();
	scanWalls();
	while (!isWall(FRONT) && isWall(RIGHT) && isWall(LEFT))
	{
		mover.moveForward(DEFAULT_SPEED);
	}
	if (!isWall(FRONT))
	{
		mover.moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
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
