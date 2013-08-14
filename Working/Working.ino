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
#define BLOCK_DIST_L (250)
#define BLOCK_DIST_R (250)

#define BLOCK_TOLERANCE (100)
#define BLOCK_STOP (60)

//Front IR senses 275-300mm for block
//Left IR senses 275-300mm for block
//Right IR senses 250-275mm for block

#define TURN_RIGHT (90 - 20) //Tweaking given speed of 80
#define TURN_LEFT (-90 + 20)
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
void followLineTest();

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
bool grabSuccess = true;

//TODO: Change type back to Direction
unsigned char facing = DIR_WEST;
unsigned char prevFacing;

int turn = TURN_RIGHT;

int calibratedBlockR;
int calibratedBlockL;

struct irDir
{
	unsigned char frnt;
	unsigned char bck;
	unsigned char lft;
	unsigned char rght;
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
		return (abs(calibratedBlockL - irInMm.lft) < BLOCK_TOLERANCE);
		//(abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
	else if (dir == REL_RIGHT)
		return (abs(calibratedBlockR - irInMm.rght) < BLOCK_TOLERANCE);
		//(abs(BLOCK_DIST_R - irInMm.rght) < BLOCK_TOLERANCE);
	else
		return false;
}

void moveToFrontPoint()
{
	mover.moveTillPoint(DEFAULT_SPEED);
	mover.moveOffCross(DEFAULT_SPEED);
	currPoint = adjacentPoint(currPoint, REL_FRONT);
}

bool obtainBlock(unsigned char relDir)
{
	tempPoint = adjacentPoint(currPoint, relDir);
	gridMap.setFlag(tempPoint, OCCUPIED);
	prevFacing = facing;
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
	if (mover.onCross() )
	{
		mover.moveOffCross();
		currPoint = adjacentPoint(currPoint, REL_FRONT);
	}
	else
		moveToFrontPoint();
	irInMm.frnt = frontIr.getDist();
	if (irInMm.frnt <= BLOCK_STOP + 40)
	{
//		Serial.print("Picked up block from: (");//DEBUG
//		Serial.print(currPoint.x);//DEBUG
//		Serial.print(", ");//DEBUG
//		Serial.println(currPoint.y);//DEBUG
		
		gridMap.removeFlag(currPoint, OCCUPIED);
		grabSuccess = true;
		return true;
	}
	else
	{
		claw.shut();
		mover.rotateAngle(TURN_BACK, DEFAULT_SPEED);
		facing = findNewFacing(REL_BACK);
		moveToFrontPoint();
		turnInDir(prevFacing);
		grabSuccess = false;
		return false;
	}
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
	turnInDir(newDir);
	facing = newDir;
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
	
	do
	{
		scanAdjacent();
	
		//irInMm.rght = 250;//DEBUG
		if (scanDir.frnt && isBlock(REL_FRONT) )
			haveBlock = obtainBlock(REL_FRONT);
		else if (scanDir.rght && isBlock(REL_RIGHT) )
			haveBlock = obtainBlock(REL_RIGHT);
		else if (scanDir.lft && isBlock(REL_LEFT) )
			haveBlock = obtainBlock(REL_LEFT);
		else
			moveToFrontPoint();
	} while(!grabSuccess);
	grabSuccess = true; //reset
}

void secondTraverse()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	do
	{
		scanAdjacent();
		
		//irInMm.rght = 250;//DEBUG
		if (scanDir.frnt && isBlock(REL_FRONT) )
			haveBlock = obtainBlock(REL_FRONT);
		else if (scanDir.rght && isBlock(REL_RIGHT) )
			haveBlock = obtainBlock(REL_RIGHT);
		else if (scanDir.lft && isBlock(REL_LEFT) )
			haveBlock = obtainBlock(REL_LEFT);
		else
		{
			//move to right point
			mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(REL_RIGHT);
			moveToFrontPoint();
		}
	} while(!grabSuccess);
	grabSuccess = true; //reset
}

void traverseLong()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	tempPoint = adjacentPoint(currPoint, REL_FRONT);
	if (gridMap.contains(tempPoint) )
	{
		do
		{
			scanAdjacent();
		
			//irInMm.lft = 250;//DEBUG
			if (scanDir.frnt && isBlock(REL_FRONT) )
				haveBlock = obtainBlock(REL_FRONT);
			else if (scanDir.rght && isBlock(REL_RIGHT) )
				haveBlock = obtainBlock(REL_RIGHT);
			else if (scanDir.lft && isBlock(REL_LEFT) )
				haveBlock = obtainBlock(REL_LEFT);
			else
				moveToFrontPoint();
		} while(!grabSuccess);
		grabSuccess = true; //reset
	}
	else
	{
		do
		{
			scanAdjacent();
			
			//irInMm.rght = 250;//DEBUG
			
			if (scanDir.frnt && isBlock(REL_FRONT) )
				haveBlock = obtainBlock(REL_FRONT);
			else if (scanDir.rght && isBlock(REL_RIGHT) )
				haveBlock = obtainBlock(REL_RIGHT);
			else if (scanDir.lft && isBlock(REL_LEFT) )
				haveBlock = obtainBlock(REL_LEFT);
			else
			{
				//turn left
				mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
				facing = findNewFacing(REL_LEFT);
				moveToFrontPoint();
			}
		} while(!grabSuccess);
		grabSuccess = true; //reset
	}
}

void traverseShort()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	do
	{
		scanAdjacent();
	
		if (scanDir.frnt && isBlock(REL_FRONT) )
			haveBlock = obtainBlock(REL_FRONT);
		else if (scanDir.rght && isBlock(REL_RIGHT) )
			haveBlock = obtainBlock(REL_RIGHT);
		else if (scanDir.lft && isBlock(REL_LEFT) )
			haveBlock = obtainBlock(REL_LEFT);
		else
		{
			mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			facing = findNewFacing(REL_LEFT);
			moveToFrontPoint();
		}
	} while(!grabSuccess);
	grabSuccess = true; //reset
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

void initTraverseAlt()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	do
	{
		grabSuccess = true; //reset
		scanFrontIr();
	
		//irInMm.rght = 250;//DEBUG
		if (scanDir.frnt && isBlock(REL_FRONT) )
			haveBlock = obtainBlock(REL_FRONT);
		else
		{
			mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(REL_RIGHT);
			moveToFrontPoint();
		}
	} while(!grabSuccess);
}

void traverseLongAlt(unsigned char faceDir)
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	tempPoint = adjacentPoint(currPoint, REL_FRONT);
	if (gridMap.contains(tempPoint) )
	{
		do
		{
			grabSuccess = true; //reset
			scanFrontIr();
		
			//irInMm.lft = 250;//DEBUG
			if (scanDir.frnt && isBlock(REL_FRONT) )
				haveBlock = obtainBlock(REL_FRONT);
			else
				moveToFrontPoint();
		} while(!grabSuccess);
	}
	else
	{
		do
		{
			scanFrontIr();
			
			//irInMm.rght = 250;//DEBUG
			
			if (scanDir.frnt && isBlock(REL_FRONT) )
				haveBlock = obtainBlock(REL_FRONT);
			else
			{
				if (faceDir == NORTH)
				{
					//turn left
					mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
					facing = findNewFacing(REL_LEFT);
				}
				else
				{
					//turn right
					mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
					facing = findNewFacing(REL_RIGHT);
				}
				moveToFrontPoint();
			}
		} while(!grabSuccess);
		grabSuccess = true; //reset
	}
}

void traverseShortAlt()
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	do
	{
		grabSuccess = true; //reset
		scanFrontIr();
	
		if (scanDir.frnt && isBlock(REL_FRONT) )
			haveBlock = obtainBlock(REL_FRONT);
		else
		{
			if (currPoint.x == 2 || currPoint.x == 0)
			{
				mover.rotateAngle(TURN_LEFT, DEFAULT_SPEED);
				facing = findNewFacing(REL_LEFT);
			}
			else
			{
				mover.rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
				facing = findNewFacing(REL_RIGHT);
			}			
			moveToFrontPoint();
		}
	} while(!grabSuccess);
	grabSuccess = true; //reset
}

void findBlockAlt()
{
	//start facing west
	facing = DIR_WEST;
	turn = TURN_RIGHT;

	initTraverseAlt();
	while (!haveBlock)
	{
		if (facing == DIR_NORTH || facing == DIR_SOUTH) // Travel till end
			traverseLongAlt(facing);
		else
			traverseShortAlt();
	}
	dropOff();
}

bool traverseAvoiding(Point nextPoint)
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	scanAdjacent();
	if (scanDir.frnt && isBlock(REL_FRONT) )
	{
		tempPoint = adjacentPoint(currPoint, REL_FRONT);
		gridMap.setFlag(tempPoint, OCCUPIED);
	}
	else if (scanDir.rght && isBlock(REL_RIGHT) )
	{
		tempPoint = adjacentPoint(currPoint, REL_RIGHT);
		gridMap.setFlag(tempPoint, OCCUPIED);
	}
	else if (scanDir.lft && isBlock(REL_LEFT) )
	{
		tempPoint = adjacentPoint(currPoint, REL_LEFT);
		gridMap.setFlag(tempPoint, OCCUPIED);
	}
	
	if (gridMap.isFlagSet(nextPoint, OCCUPIED) )
		return true;
	else
	{
		moveToPoint(nextPoint);
		gridMap.setFlag(currPoint, VISITED);
		return false;
	}
}

void avoidBlocks()
{
	Path path;
	Point startPoint(GRID_MAX_X, 0);
	currPoint = startPoint;
	Point goalPoint(0, GRID_MAX_Y);
	facing = DIR_WEST;
	while (currPoint != goalPoint)
	{
		router.generateRoute(currPoint, goalPoint, (Direction)facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (traverseAvoiding(path.path[ii]) )
				break;
		}
	}
	motors.stop();
	while (currPoint != startPoint)
	{
		router.generateRoute(currPoint, startPoint, (Direction)facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (traverseAvoiding(path.path[ii]) )
				break;
		}
	}
	motors.stop();
}

bool traverseAvoidingAlt(Point nextPoint)
{
	motors.stop();
	gridMap.setFlag(currPoint, VISITED);
	
	scanFrontIr();
	if (scanDir.frnt && isBlock(REL_FRONT) )
	{
		tempPoint = adjacentPoint(currPoint, REL_FRONT);
		gridMap.setFlag(tempPoint, OCCUPIED);
	}
	if (gridMap.isFlagSet(nextPoint, OCCUPIED) )
		return true;
	else
	{
		unsigned char newDir = dirOfPoint(nextPoint);
		turnInDir(newDir);
		facing = newDir;
		scanFrontIr();
		if (scanDir.frnt && isBlock(REL_FRONT) )
		{
			tempPoint = adjacentPoint(currPoint, REL_FRONT);
			gridMap.setFlag(tempPoint, OCCUPIED);
			return true;
		}
		else
		{
			moveToFrontPoint();
			gridMap.setFlag(currPoint, VISITED);
			return false;
		}
	}
}

void avoidBlocksAlt()
{
	Path path;
	Point startPoint(GRID_MAX_X, 0);
	currPoint = startPoint;
	Point goalPoint(0, GRID_MAX_Y);
	facing = DIR_WEST;
	while (currPoint != goalPoint)
	{
		router.generateRoute(currPoint, goalPoint, (Direction)facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (traverseAvoidingAlt(path.path[ii]) )
				break;
		}
	}
	motors.stop();
	while (currPoint != startPoint)
	{
		router.generateRoute(currPoint, startPoint, (Direction)facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (traverseAvoidingAlt(path.path[ii]) )
				break;
		}
	}
	motors.stop();
}

////////////////////////////////// TEST FUNCTIONS //////////////////////////////////

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

void lineSenseCheck()
{
	Point pt = currPoint;
	pt.x = GRID_MAX_X;
	for (int y = 1; y <= GRID_MAX_Y; ++y)
	{
		pt.y = y;
		moveToPoint(pt);
		gridMap.setFlag(pt, VISITED);
	}
	pt.x = 2;
	for (int y = GRID_MAX_Y; y >= 1; --y)
	{
		pt.y = y;
		moveToPoint(pt);
		gridMap.setFlag(pt, VISITED);
	}
	pt.x = 1;
	for (int y = 1; y <= GRID_MAX_Y; ++y)
	{
		pt.y = y;
		moveToPoint(pt);
		gridMap.setFlag(pt, VISITED);
	}
	pt.x = 0;
	for (int y = GRID_MAX_Y; y >= 1; --y)
	{
		pt.y = y;
		moveToPoint(pt);
		gridMap.setFlag(pt, VISITED);
	}
	motors.stop();
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
}

////////////////////////////////// END TEST FUNCTIONS //////////////////////////////////

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
	claw.setup();
	motors.setup();
//	
//	delayTillButton();
//	
//	pinMode(OPENDAY_IR_SWITCH, INPUT);
//	digitalWrite(OPENDAY_IR_SWITCH, HIGH);
//	if (digitalRead(OPENDAY_IR_SWITCH))
//	{
//		calibratedBlockR = BLOCK_DIST_R;
//		calibratedBlockL = BLOCK_DIST_L;
//		Serial.println("PRE");
//	}
//	else
//	{
//		//scan a few IRs before reading calibrated
//		Serial.println("CAL");
//		for (int ii = 0; ii < 10; ++ii)
//		{
//			Serial.print(leftIr.getDist());
//			Serial.print("\t");
//			Serial.println(rightIr.getDist());
//		}
//		calibratedBlockR = rightIr.getDist();
//		calibratedBlockL = leftIr.getDist();
//	}
	claw.shut();
	delayTillButton();

        for(int i=0; i<300; i++)
        {
          ls.calibrate();
        }
	
	delayTillButton();
}
void loop()
{
  Serial.println(backIr.getDist());
  delay(100);
//  LineSensor_ColourValues allWhite[8] = {WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT};
//  int crosscounter=0;
//  while(crosscounter<numCross)
//  {
//    while(!ls.see(allWhite))
//    {
//      followLineTest();
//      delay(60);
//    }
//    motors.both(80);
//    while(ls.see(allWhite))
//    {
//      motors.both(80);
//    }
//    crosscounter+=1;
//  }
//   numCross++;
//   if (numCross>2)
//   {
//     numCross=1;
//   }
//   crosscounter=0;
//   mover.rotateAngle(70,80);
//   motors.both(80);
//   delay(30);
  
////	testSensors();
//	pinMode(OPENDAY_ALT_SWITCH, INPUT);
//	digitalWrite(OPENDAY_ALT_SWITCH, HIGH);
//	if (digitalRead(OPENDAY_ALT_SWITCH))
//	{
//		pinMode(OPENDAY_MODE_SWITCH, INPUT);
//		digitalWrite(OPENDAY_MODE_SWITCH, HIGH);
//		if (digitalRead(OPENDAY_MODE_SWITCH))
//			findBlock();
//		else
//			avoidBlocks();
//	}
//	else
//	{
//		pinMode(OPENDAY_MODE_SWITCH, INPUT);
//		digitalWrite(OPENDAY_MODE_SWITCH, HIGH);
//		if (digitalRead(OPENDAY_MODE_SWITCH))
//			findBlockAlt();
//		else
//			avoidBlocksAlt();
//	}
//	
//	delayTillButton();
//
//	irInMm.frnt = 500;
//	irInMm.bck = 500;
//	irInMm.rght = 500;
//	irInMm.lft = 500;
//	currPoint.x = GRID_MAX_X;
//	currPoint.y = 0;
//	tempPoint.x = 0;
//	tempPoint.y = 0;
//
//	haveBlock = false;
//
//	facing = DIR_WEST;
//	turn = TURN_RIGHT;
//	claw.shut();
//	delay(100);
}

//Note, this is NOT the best way to do it.  Just a quick example of how to use the class.
void lineFollowDemo()
{
	LineSensor_ColourValues leftSlightWhite[8] = {NUL,NUL,WHT,NUL,NUL,NUL,NUL,NUL};
	LineSensor_ColourValues rightSlightWhite[8] = {NUL,NUL,NUL,NUL,NUL,WHT,NUL,NUL};
	LineSensor_ColourValues allBlack[8] = {BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK};
	ls.readCalibrated();
	if(ls.see(leftSlightWhite))
	{
		motors.left(127);
		motors.right(0);
	}
	if(ls.see(rightSlightWhite))
	{
		motors.right(127);
		motors.left(0);
	}
	if(ls.see(allBlack))
		motors.stop();
}

/*
    int linePos = 0;
    int lastLinePos = 0;
    int difference=0;
    float leftspeed=0.0;
    float rightspeed=0.0;
    const int edgeSensitivity = 1000;
    int qtrTotal;
    boolean seenBlack=0;
void followLineTest()
{
  //motors.left(leftspeed);
  //motors.right(rightspeed);

    if (1)//(!ls.see(allWhite))
    {
    lastLinePos = linePos;
    qtrTotal = ls.reading[0]+ls.reading[1]+ls.reading[2]+ls.reading[3]+ls.reading[4]+ls.reading[5]+ls.reading[6]+ls.reading[7];
    if (qtrTotal<7000)
    {
    if (seenBlack)
    {
       linePos = ls.readLine(ls.reading, QTR_EMITTERS_ON, 1);
       while ((abs(linePos-3500))>500)
       {
         motors.left(-60);
         motors.right(-60);
         linePos = ls.readLine(ls.reading, QTR_EMITTERS_ON, 1);
       }
         delay(100);
         linePos = ls.readLine(ls.reading, QTR_EMITTERS_ON, 1);
//       while ((abs(linePos-3500))>3000)
//       {
//         motors.left(-60);
//         motors.right(-60);
//         linePos = ls.readLine(ls.reading, QTR_EMITTERS_ON, 1);
//       }
//       
       if (linePos>3500)
       {
         motors.left(20);
         motors.right(80);
         delay(30);
       }
       else
       {
         motors.left(80);
         motors.right(20);
       }
       seenBlack=false;
       
       
    }
    linePos = ls.readLine(ls.reading, QTR_EMITTERS_ON, 1);
    difference = linePos-lastLinePos;
    if (abs(linePos-3500)<edgeSensitivity)
    {
      if (difference<-30)
      {
         motors.left(80);
         motors.right(80-20);
      }
      if (difference>30)
      {
         motors.right(80);
         motors.left(80-20);
      }
      if (abs(difference)<30)
      {
        motors.both(80);
      }
    }
    else if (linePos>6000)
    {
      motors.left(20);
      motors.right(80);
    }
    else if (linePos<1000)
    {
      motors.left(80);
      motors.right(20);
    }
    else if (linePos-3500>edgeSensitivity)
    {
      motors.right(80);
      motors.left(50);
    }
    else if (linePos-3500<-edgeSensitivity)
    {
      motors.right(50);
      motors.left(80);
    }
    }
    else 
    {
      motors.right(-35);
      motors.left(-35);
      seenBlack=true;
    }
    }
}
*/
