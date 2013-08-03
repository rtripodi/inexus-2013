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

#define CENTRE_DIST (300)
#define BLOCK_ACT_DIST (250)
#define BLOCK_DIST_F (300)
#define BLOCK_DIST_L (281)
#define BLOCK_DIST_R (277)

#define BLOCK_TOLERANCE (50)
#define BLOCK_STOP (60)

//Front IR senses 275-300mm for block
//Left IR senses 275-300mm for block
//Right IR senses 250-275mm for block

#define TURN_RIGHT 90
#define TURN_LEFT -90
#define TURN_FRONT 0
#define TURN_BACK 180

//F=0, R=1, B=2, L=3
//enum RelativeDir {RelFront, RelRight, RelBack, RelLeft};
#define REL_RIGHT 1
#define REL_LEFT 3
#define REL_FRONT 0
#define REL_BACK 2

#define DIR_NORTH 1
#define DIR_EAST 2
#define DIR_SOUTH 3
#define DIR_WEST 4

Motor motors;
LineSensors ls;
void lineFollowDemoSetup();
void lineFollowDemo();

Movement mover(&motors, &ls);

IR frontIr(IR_SHORT_PIN, IR::shortRange);
IR backIr(IR_MEDIUMB_PIN, IR::mediumRange);
IR leftIr(IR_MEDIUML_PIN, IR::mediumRange);
IR rightIr(IR_MEDIUMR_PIN, IR::mediumRange);

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);

GridMap gridMap;

Routing router(&gridMap);

Path path;

struct irValues
{
	int frnt;
	int bck;
	int lft;
	int rght;
};

//stores distance in mm read from each ir sensor
irValues irInMm = {500, 500, 500, 500};

//start at (GRID_MAX_X, 0)
Point currPoint(GRID_MAX_X, 0);

//used to use points as function parameters
Point tempPoint(0, 0);

bool haveBlock = false;

//TODO: Change type back to Direction
unsigned char facing = DIR_WEST;
int turn = TURN_RIGHT;

struct irDir
{
	int frnt;
	int bck;
	int lft;
	int rght;
};

irDir scanDir = {1, 0, 0, 1};

/////////////////////// DEBUG PRINTING FUNCTIONS ///////////////////////
void printDirection(unsigned char dir)
{
	switch(dir)
	{
		case DIR_NORTH:
			Serial.print("North");
			break;
		case DIR_SOUTH:
			Serial.print("South");
			break;
		case DIR_EAST:
			Serial.print("East");
			break;
		case DIR_WEST:
			Serial.print("West");
			break;
	}
}

void printRelDirection(unsigned char dir)
{
	switch(dir)
	{
		case REL_FRONT:
			Serial.print("Front");
			break;
		case REL_BACK:
			Serial.print("Back");
			break;
		case REL_RIGHT:
			Serial.print("Right");
			break;
		case REL_LEFT:
			Serial.print("Left");
			break;
	}
}

void printGrid()
{
	Serial.println();
	char buffer[50];
	for(int x = 0; x < GRID_MAX_X + 1 ; ++x)
	{
		for(int y = 0; y < GRID_MAX_Y + 1; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			sprintf(buffer, "[%x]", gridMap.getFlags(tempPoint) );
			Serial.print(buffer);
		}
		Serial.println();
	}
	Serial.println();
	Serial.print("Current: (");
	Serial.print(currPoint.x);
	Serial.print(", ");
	Serial.print(currPoint.y);
	Serial.print(")\tFacing: ");
	printDirection(facing);
	Serial.println();
	Serial.println();
}
/////////////////////// END DEBUG PRINTING FUNCTIONS ///////////////////////

void dirToCheck()
{
	if ( (facing == DIR_NORTH && currPoint.x == GRID_MAX_X) || (facing == DIR_WEST && currPoint.y == GRID_MAX_Y) || (facing == DIR_SOUTH && currPoint.x == 0) || (facing == DIR_EAST && currPoint.y == 0) )
	{//front and left
		scanDir.frnt = 1;
		scanDir.bck = 0;
		scanDir.lft = 1;
		scanDir.rght = 0;
	}
	else if ( (facing == DIR_NORTH && currPoint.x == 0) || (facing == DIR_WEST && currPoint.y == 0) || (facing == DIR_SOUTH && currPoint.x == GRID_MAX_X) || (facing == DIR_EAST && currPoint.y == GRID_MAX_Y) )
	{//front and right
		scanDir.frnt = 1;
		scanDir.bck = 0;
		scanDir.lft = 0;
		scanDir.rght = 1;
	}
	else
	{//front, left, right
		scanDir.frnt = 1;
		scanDir.bck = 0;
		scanDir.lft = 1;
		scanDir.rght = 1;
	}
}

unsigned char findNewFacing(unsigned char relativeTurn)
{
	if( (facing == DIR_NORTH && relativeTurn == REL_FRONT) || (facing == DIR_EAST && relativeTurn == REL_LEFT) || (facing == DIR_SOUTH && relativeTurn == REL_BACK) || (facing == DIR_WEST && relativeTurn == REL_RIGHT) )
		return DIR_NORTH;
	else if( (facing == DIR_NORTH && relativeTurn == REL_RIGHT) || (facing == DIR_EAST && relativeTurn == REL_FRONT) || (facing == DIR_SOUTH && relativeTurn == REL_LEFT) || (facing == DIR_WEST && relativeTurn == REL_BACK) )
		return DIR_EAST;
	else if( (facing == DIR_NORTH && relativeTurn == REL_BACK) || (facing == DIR_EAST && relativeTurn == REL_RIGHT) || (facing == DIR_SOUTH && relativeTurn == REL_FRONT) || (facing == DIR_WEST && relativeTurn == REL_LEFT) )
		return DIR_SOUTH;
	else if( (facing == DIR_NORTH && relativeTurn == REL_LEFT) || (facing == DIR_EAST && relativeTurn == REL_BACK) || (facing == DIR_SOUTH && relativeTurn == REL_RIGHT) || (facing == DIR_WEST && relativeTurn == REL_FRONT) )
		return DIR_WEST;
	else
		return DIR_NORTH; //Explode
}

Point adjacentPoint(Point pt, unsigned char relativeTurn)
{
	unsigned char dir = findNewFacing(relativeTurn);
		
	switch(dir)
	{
		case DIR_NORTH:
			pt.y += 1;
			break;
		case DIR_SOUTH:
			pt.y -= 1;
			break;
		case DIR_EAST:
			pt.x += 1;
			break;
		case DIR_WEST:
			pt.x -= 1;
			break;
	}
	return pt;
}

bool isBlock(unsigned char dir)
{
	if (dir == REL_FRONT)
		return (irInMm.frnt < BLOCK_DIST_F);
	else if (dir == REL_LEFT)
		return (abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
	else if (dir == REL_RIGHT)
		return (abs(BLOCK_DIST_R - irInMm.rght) < BLOCK_TOLERANCE);
	else
		return false;
}

bool obtainBlock(unsigned char relDir)
{
	tempPoint = adjacentPoint(currPoint, relDir);
	gridMap.setFlag(tempPoint, OCCUPIED);
	switch (relDir)
	{
		case REL_FRONT:
			mover.rotateAngle(TURN_FRONT, DEFAULT_SPEED);
			break;
		case REL_BACK:
			mover.rotateAngle(TURN_BACK, DEFAULT_SPEED);
			break;
		case REL_LEFT:
			mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			break;
		case REL_RIGHT:
			mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			break;
	}
	facing = findNewFacing(relDir);
	claw.open();
	irInMm.frnt = frontIr.getDist();
	while (irInMm.frnt > BLOCK_STOP && !mover.onCross() )
	{
		mover.moveForward(DEFAULT_SPEED);
		irInMm.frnt = frontIr.getDist();
	}
	motors.stop();
	claw.close();
	moveToFrontPoint();
	irInMm.frnt = frontIr.getDist();
	if (irInMm.frnt <= BLOCK_STOP + 20) //DEBUG: assume true for now
	{
//		Serial.print("Picked up block from: (");//DEBUG
//		Serial.print(currPoint.x);//DEBUG
//		Serial.print(", ");//DEBUG
//		Serial.println(currPoint.y);//DEBUG
		
		gridMap.removeFlag(currPoint, OCCUPIED);
		return true;
	}
	else
		return false;
}

unsigned char dirOfPoint(Point pt)
{
	if (pt.y == currPoint.y + 1 && pt.x == currPoint.x)
		return DIR_NORTH;
	else if (pt.y == currPoint.y - 1 && pt.x == currPoint.x)
		return DIR_SOUTH;
	else if (pt.y == currPoint.y && pt.x == currPoint.x + 1)
		return DIR_EAST;
	else if (pt.y == currPoint.y && pt.x == currPoint.x - 1)
		return DIR_WEST;
	//TODO: Add NWEST etc.
}

void moveToFrontPoint()
{
	mover.moveTillPoint(DEFAULT_SPEED);
	mover.moveOffCross(DEFAULT_SPEED);
	currPoint = adjacentPoint(currPoint, REL_FRONT);
}

void turnInDir(unsigned char newDir)
{
/*	signed char delta = newDir - facing; DEBUG: Haven't tested, using below if statement instead
	if (delta < 0)
		delta += 4;
	turn = delta;*/
	if (facing == newDir)
		turn = REL_FRONT;
	else if ( (facing == DIR_NORTH && newDir == DIR_SOUTH) || (facing == DIR_EAST && newDir == DIR_WEST) || (facing == DIR_SOUTH && newDir == DIR_NORTH) || (facing == DIR_WEST && newDir == DIR_EAST) )
		turn = REL_BACK;
	else if ( (facing == DIR_NORTH && newDir == DIR_WEST) || (facing == DIR_EAST && newDir == DIR_NORTH) || (facing == DIR_SOUTH && newDir == DIR_EAST) || (facing == DIR_WEST && newDir == DIR_SOUTH) )
		turn = REL_LEFT;
	else if ( (facing == DIR_NORTH && newDir == DIR_EAST) || (facing == DIR_EAST && newDir == DIR_SOUTH) || (facing == DIR_SOUTH && newDir == DIR_WEST) || (facing == DIR_WEST && newDir == DIR_NORTH) )
		turn = REL_RIGHT;
	switch (turn)
	{
		case REL_FRONT:
			mover.rotateAngle(TURN_FRONT, DEFAULT_SPEED);
			break;
		case REL_BACK:
			mover.rotateAngle(TURN_BACK, DEFAULT_SPEED);
			break;
		case REL_LEFT:
			mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			break;
		case REL_RIGHT:
			mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			break;
	}
}

void moveToPoint(Point pt)
{
	unsigned char newDir = dirOfPoint(pt);
	
	Serial.print("Direction: "); //DEBUG
	printDirection(newDir);//DEBUG
		
	turnInDir(newDir);
	
	Serial.print("\tTurn: ");//DEBUG
	printRelDirection(turn);//DEBUG
	
	facing = newDir;
	
	Serial.print("\tNow Facing: ");//DEBUG
	printDirection(newDir);//DEBUG
	
	moveToFrontPoint();
}

void dropOff()
{
	Point entrPoint(GRID_MAX_X, 0);
	router.generateRoute(currPoint, entrPoint, (Direction)facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToPoint(path.path[ii]);
	}
	motors.stop();
	claw.open();
}

//scan front point and set seen flag
void scanFrontIr()
{
	irInMm.frnt = frontIr.getDist();
	tempPoint = adjacentPoint(currPoint, REL_FRONT);
	gridMap.setFlag(tempPoint, SEEN);
}
	
//scan right point and set seen flag
void scanRightIr()
{
	irInMm.rght = rightIr.getDist();
	tempPoint = adjacentPoint(currPoint, REL_RIGHT);
	gridMap.setFlag(tempPoint, SEEN);
}

//scan left point and set seen flag
void scanLeftIr()
{
	irInMm.lft = leftIr.getDist();
	tempPoint = adjacentPoint(currPoint, REL_LEFT);
	gridMap.setFlag(tempPoint, SEEN);
}

void scanAdjacent()
{
	if(scanDir.frnt)
		scanFrontIr();
	if(scanDir.rght)
		scanRightIr();
	if(scanDir.lft)
		scanLeftIr();
}

//Base case for traversal
//At start: scan front and right, if no blocks move to front point
//At next point: scan front and right, if no blocks turn right and move
void initTraverse()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
		
	scanFrontIr();
	scanRightIr();
	
	//irInMm.rght = 250;//DEBUG
	
	if (isBlock(REL_FRONT) )
	{
		haveBlock = obtainBlock(REL_FRONT);
	}
	else if (isBlock(REL_RIGHT) )
	{
		haveBlock = obtainBlock(REL_RIGHT);
	}
	else
	{
		moveToFrontPoint();
	}
}

void secondTraverse()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	scanFrontIr();
	scanRightIr();
	
	//irInMm.rght = 250;//DEBUG
	
	if (isBlock(REL_FRONT) )
	{
		haveBlock = obtainBlock(REL_FRONT);
	}
	else if (isBlock(REL_RIGHT) )
	{
		haveBlock = obtainBlock(REL_RIGHT);
	}
	else
	{
		//move to right point
		mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
		facing = findNewFacing(REL_RIGHT);
		moveToFrontPoint();
	}
}

void traverseLong()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	tempPoint = adjacentPoint(currPoint, REL_FRONT);
	if (gridMap.contains(tempPoint) )
	{
		scanFrontIr();
		scanRightIr();
		scanLeftIr();
		
		//irInMm.lft = 250;//DEBUG
		
		if (isBlock(REL_FRONT) )
		{
			haveBlock = obtainBlock(REL_FRONT);
		}
		else if (isBlock(REL_RIGHT) )
		{
			haveBlock = obtainBlock(REL_RIGHT);
		}
		else if (isBlock(REL_LEFT) )
		{
			haveBlock = obtainBlock(REL_LEFT);
		}	
		else
		{
			moveToFrontPoint();
		}
	}
	else
	{
		scanRightIr();
		scanLeftIr();
		
		//irInMm.rght = 250;//DEBUG
		
		if (isBlock(REL_RIGHT) )
		{
			haveBlock = obtainBlock(REL_RIGHT);
		}
		else if (isBlock(REL_LEFT) )
		{
			haveBlock = obtainBlock(REL_LEFT);
		}
		else
		{
			//turn left
			mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			facing = findNewFacing(REL_LEFT);
			moveToFrontPoint();
		}	
	}
}

void traverseShort()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	scanFrontIr();
	scanLeftIr();
	
	if (isBlock(REL_FRONT) )
	{
		haveBlock = obtainBlock(REL_FRONT);
	}
	else if (isBlock(REL_LEFT) )
	{
		haveBlock = obtainBlock(REL_LEFT);
	}	
	else
	{
		mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
		facing = findNewFacing(REL_LEFT);
		moveToFrontPoint();
	}
}

void findBlock()
{
	//start facing west
	facing = DIR_WEST;
	turn = TURN_RIGHT;

	initTraverse();
	if (!haveBlock)
	{
		secondTraverse();
		while (!haveBlock)
		{
			if (facing == DIR_NORTH || facing == DIR_SOUTH) // Travel till end
				traverseLong();
			else
				traverseShort();
		}
	}
	dropOff();
}

void avoidBlocks()
{
	Path path;
	Point startPoint(GRID_MAX_X, 0);
	currPoint = startPoint;
	Point goalPoint(0, GRID_MAX_Y);
	facing = DIR_WEST;
	router.generateRoute(startPoint, goalPoint, (Direction)facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToPoint(path.path[ii]);
		gridMap.setFlag(currPoint, VISITED);
		printGrid();
	}	
}

void virtualfindBlock()
{
	//start facing west
	facing = DIR_WEST;
	turn = TURN_RIGHT;

	initTraverse();
	printGrid();
	if (!haveBlock)
	{
		secondTraverse();
		printGrid();
		while (!haveBlock)
		{
			if (facing == DIR_NORTH || facing == DIR_SOUTH) // Travel till end
				traverseLong();
			else
				traverseShort();
			printGrid();
		}
	}
	printGrid();
//	dropOff();
	delay(100000);
}

void virtualPointTest()
{
	Point pt[5];
	pt[0] = currPoint;
	pt[1].x = currPoint.x - 1;
	pt[1].y = currPoint.y;
	pt[2].x = currPoint.x - 1;
	pt[2].y = currPoint.y + 1;
	pt[3].x = currPoint.x - 2;
	pt[3].y = currPoint.y + 1;
	pt[4].x = currPoint.x - 2;
	pt[4].y = currPoint.y + 2;
	gridMap.setFlag(currPoint, VISITED);
	printGrid();
	for (int ii = 1; ii < 5; ++ii)
	{
		moveToPoint(pt[ii]);
		gridMap.setFlag(currPoint, VISITED);
		printGrid();
	}
}

void virtualRouting()
{
	Path path;
	Point startPoint(GRID_MAX_X, 0);
	currPoint = startPoint;
	Point goalPoint(0, GRID_MAX_Y);
	facing = DIR_WEST;
	router.generateRoute(startPoint, goalPoint, (Direction)facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToPoint(path.path[ii]);
		gridMap.setFlag(currPoint, VISITED);
		printGrid();
	}
}

void testSensors()
{	
	unsigned char blockDir = 99;
	irInMm.frnt = frontIr.getDist();
	irInMm.lft = leftIr.getDist();
	irInMm.rght = rightIr.getDist();
	Serial.print(irInMm.lft);
	Serial.print("\t");
	Serial.print(irInMm.frnt);
	Serial.print("\t");
	Serial.println(irInMm.rght);
	/*
	while (blockDir != REL_FRONT && blockDir != REL_LEFT && blockDir != REL_RIGHT)
	{
		if (irInMm.frnt > BLOCK_DIST - BLOCK_TOLERANCE)
			blockDir = REL_FRONT;
		else if (irInMm.lft > BLOCK_DIST - BLOCK_TOLERANCE)
			blockDir = REL_LEFT;
		else if (irInMm.rght > BLOCK_DIST - BLOCK_TOLERANCE)
			blockDir = REL_RIGHT;
		else
		{
			irInMm.frnt = frontIr.getDist();
			irInMm.lft = leftIr.getDist();
			irInMm.rght = rightIr.getDist();
			Serial.print(irInMm.lft);
			Serial.print("\t");
			Serial.print(irInMm.frnt);
			Serial.print("\t");
			Serial.println(irInMm.rght);
		}
	}
	switch (blockDir)
	{
		case REL_LEFT:
			mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			break;
		case REL_RIGHT:
			mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			break;
	}
	claw.open();
	irInMm.frnt = frontIr.getDist();
	while (irInMm.frnt > BLOCK_STOP)
	{
		mover.moveForward(DEFAULT_SPEED);
		irInMm.frnt = frontIr.getDist();
	}
	motors.stop();
	claw.close();
	mover.moveTillPoint(DEFAULT_SPEED);
	mover.moveOffCross(DEFAULT_SPEED);
	motors.stop();
	
	claw.open();

	delay(100000);*/
}

void lineFollowDemoSetup()
{
	ls.calibrate();
	delay(3000);
	ls.calibrate();
}

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
/*	claw.setup();
	motors.setup();
	
	delayTillButton();
	
	claw.shut();
	lineFollowDemoSetup();
	
	delayTillButton();*/
}

void loop()
{
	testSensors();
/*	findBlock();
	delayTillButton();

	irInMm.frnt = 500;
	irInMm.bck = 500;
	irInMm.rght = 500;
	irInMm.lft = 500;
	currPoint.x = GRID_MAX_X;
	currPoint.y = 0;
	tempPoint.x = 0;
	tempPoint.y = 0;

	haveBlock = false;

	facing = DIR_WEST;
	turn = TURN_RIGHT;*/
}

//Note, this is NOT the best way to do it.  Just a quick example of how to use the class.
void lineFollowDemo()
{
	LineSensor_ColourValues leftWhite[8] = {NUL,NUL,WHT,NUL,NUL,NUL,NUL,NUL};
	LineSensor_ColourValues rightWhite[8] = {NUL,NUL,NUL,NUL,NUL,WHT,NUL,NUL};
	LineSensor_ColourValues allBlack[8] = {BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK};
	ls.readCalibrated();
	if(ls.see(leftWhite))
	{
		motors.left(127);
		motors.right(0);
	}
	if(ls.see(rightWhite))
	{
		motors.right(127);
		motors.left(0);
	}
	if(ls.see(allBlack))
		motors.stop();
}
