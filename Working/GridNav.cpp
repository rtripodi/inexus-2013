/*
	Changes to be made:
		- Consitently list cardinal directions in the order: North, East, South, West
		- Consitently list relative directions  in the order: Front, Right, Back, Left
		- Decide action for already seen points
		- Move unrelated constants, variables and functions
*/

#include "GridNav.h"

GridNav::GridNav(Motor *inMotor, Movement *inMovement, IR *inIrs[4], Claw *inClaw)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	claw = inClaw;
	GridMap gridMap;
	Routing router(&gridMap);
	Path path;
}

//Returns the value is degrees needed to excute a turn in the given relative direction
int GridNav::findAngle(unsigned char relDir)
{
	if (relDir == REL_FRONT)
		return TURN_FRONT;
	else if (relDir ==  REL_RIGHT)
		return TURN_RIGHT;
	else if (relDir ==  REL_BACK)
		return TURN_BACK;
	else if (relDir ==  REL_LEFT)
		return TURN_LEFT;
}

//Returns the new facing cardinal direction given a turn in the relative direction
//Unexpected return for inputs other then: Front, Right, Back, Left
unsigned char GridNav::findNewFacing(unsigned char inFacing, unsigned char relativeTurn)
{
	if ((inFacing == DIR_NORTH && relativeTurn == REL_FRONT) || (inFacing == DIR_EAST && relativeTurn == REL_LEFT) || (inFacing == DIR_SOUTH && relativeTurn == REL_BACK) || (inFacing == DIR_WEST && relativeTurn == REL_RIGHT))
		return DIR_NORTH;
	else if ((inFacing == DIR_NORTH && relativeTurn == REL_RIGHT) || (inFacing == DIR_EAST && relativeTurn == REL_FRONT) || (inFacing == DIR_SOUTH && relativeTurn == REL_LEFT) || (inFacing == DIR_WEST && relativeTurn == REL_BACK))
		return DIR_EAST;
	else if ((inFacing == DIR_NORTH && relativeTurn == REL_BACK) || (inFacing == DIR_EAST && relativeTurn == REL_RIGHT) || (inFacing == DIR_SOUTH && relativeTurn == REL_FRONT) || (inFacing == DIR_WEST && relativeTurn == REL_LEFT))
		return DIR_SOUTH;
	else if ((inFacing == DIR_NORTH && relativeTurn == REL_LEFT) || (inFacing == DIR_EAST && relativeTurn == REL_BACK) || (inFacing == DIR_SOUTH && relativeTurn == REL_RIGHT) || (inFacing == DIR_WEST && relativeTurn == REL_FRONT))
		return DIR_WEST;
}

//Returns the closest point in the inputted relative direction
//Unexpected return for a relative direction causing a point off the grid
Point GridNav::adjacentPoint(Point pt, unsigned char inFacing, unsigned char relativeTurn)
{
	unsigned char dir = findNewFacing(inFacing, relativeTurn);
		
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
	if (gridMap.contains(pt) )
		return pt;
}

//Checks whether there is a block within a specified range of the inputted relative direction
//Returns false by default for a relative direction causing a point off the grid
bool GridNav::isBlock(unsigned char dir)
{
	if (dir == REL_FRONT)
		return (irInMm.frnt < BLOCK_DIST_F);
	else if (dir == REL_LEFT)
		return (abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
		//(abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
	else if (dir == REL_RIGHT)
		return (abs(BLOCK_DIST_R - irInMm.rght) < BLOCK_TOLERANCE);
		//(abs(BLOCK_DIST_R - irInMm.rght) < BLOCK_TOLERANCE);
	else
		return false;
}

//TODO: Return point moved to, then currPoint will be made equal to this 
void GridNav::moveToFrontPoint()
{
/*VIRTUAL	mover->moveTillPoint(DEFAULT_SPEED);
	mover->moveOffCross(DEFAULT_SPEED);*/
	currPoint = adjacentPoint(currPoint, facing, REL_FRONT);
}

//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
//If the grab is unsuccessful, the robot will return to the point at which it sensed it
bool GridNav::obtainBlock(unsigned char relDir)
{
	unsigned char prevFacing = facing;
	Point tempPoint;
	tempPoint = adjacentPoint(currPoint, facing, relDir);
	gridMap.setFlag(tempPoint, OCCUPIED);
//VIRTUAL	mover->rotateAngle(findAngle(relDir), DEFAULT_SPEED);
	facing = findNewFacing(facing, relDir);
	claw->open();
	irInMm.frnt = irs[0]->getDist();
	while (irInMm.frnt > BLOCK_STOP && !mover->onCross() )
	{
//VIRTUAL		mover->moveForward(DEFAULT_SPEED);
		irInMm.frnt = irs[0]->getDist();
	}
//VIRTUAL	motors->stop();
//VIRTUAL	claw->close();
/*VIRTUAL	if (mover->onCross() )
	{
		mover->moveOffCross();
		currPoint = adjacentPoint(currPoint, facing, REL_FRONT);
	}
	else
		moveToFrontPoint();*/
	irInMm.frnt = irs[0]->getDist();
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
		claw->shut();
//VIRTUAL		mover->rotateAngle(TURN_BACK, DEFAULT_SPEED);
		facing = findNewFacing(facing, REL_BACK);
		moveToFrontPoint();
//VIRTUAL		mover->rotateAngle( findAngle( dirCardToRel(prevFacing)), DEFAULT_SPEED);
		grabSuccess = false;
		return false;
	}
}

//Returns the cardinal direction of a point given the inputted relative direction
//
unsigned char GridNav::dirOfPoint(Point pt)
{
	if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x))
		return DIR_NORTH;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x))
		return DIR_SOUTH;
	else if ((pt.y == currPoint.y) && (pt.x == currPoint.x + 1))
		return DIR_EAST;
	else if ((pt.y == currPoint.y) && (pt.x == currPoint.x - 1))
		return DIR_WEST;
/*	else if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x + 1))
		return DIR_NEAST;
	else if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x - 1))
		return DIR_NWEST;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x + 1))
		return DIR_SEAST;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x - 1))
		return DIR_SWEST;*/
}

//Turns the robot in the desired cardinal direction
//Unexpected results for inputs other then: North, East, South, West
unsigned char GridNav::dirCardToRel(unsigned char newCardDir)
{
/*	DEBUG: Haven't tested, using below if statement instead
	Assumes N=1, E=2, S=3, W=4, F=0, R=1, B=2, L=3
	signed char delta = newCardDir - facing;
	if (delta < 0)
		delta += 4;
	turn = delta;*/
	if (facing == newCardDir)
		return REL_FRONT;
	else if ((facing == DIR_NORTH && newCardDir == DIR_SOUTH) || (facing == DIR_EAST && newCardDir == DIR_WEST) || (facing == DIR_SOUTH && newCardDir == DIR_NORTH) || (facing == DIR_WEST && newCardDir == DIR_EAST))
		return REL_BACK;
	else if ((facing == DIR_NORTH && newCardDir == DIR_WEST) || (facing == DIR_EAST && newCardDir == DIR_NORTH) || (facing == DIR_SOUTH && newCardDir == DIR_EAST) || (facing == DIR_WEST && newCardDir == DIR_SOUTH))
		return REL_LEFT;
	else if ((facing == DIR_NORTH && newCardDir == DIR_EAST) || (facing == DIR_EAST && newCardDir == DIR_SOUTH) || (facing == DIR_SOUTH && newCardDir == DIR_WEST) || (facing == DIR_WEST && newCardDir == DIR_NORTH))
		return REL_RIGHT;
}

//Moves to given point
void GridNav::moveToPoint(Point pt)
{
	unsigned char newDir = dirOfPoint(pt);
//VIRTUAL	mover->rotateAngle(findAngle( dirCardToRel(newDir)), DEFAULT_SPEED);
	facing = newDir;
	moveToFrontPoint();
}

//Travels the current least expensive route to the entrance
void GridNav::returnToEntrance()
{
	Point entrPoint(GRID_MAX_X, 0);
	router.generateRoute(currPoint, entrPoint, (Direction)facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToPoint(path.path[ii]);
	}
/*VIRTUAL	motors->stop();
	claw->open();*/
}

//Scan front point and set seen flag
void GridNav::scanFrontIr()
{
	Point tempPoint;
	irInMm.frnt = 1000;//VIRTUALirs[0]->getDist();
	tempPoint = adjacentPoint(currPoint, facing, REL_FRONT);
	gridMap.setFlag(tempPoint, SEEN);
}
	
//Scan right point and set seen flag
void GridNav::scanRightIr()
{
	Point tempPoint;
	irInMm.rght = 1000;//VIRTUALirs[1]->getDist();
	tempPoint = adjacentPoint(currPoint, facing, REL_RIGHT);
	gridMap.setFlag(tempPoint, SEEN);
}

//Scan left point and set seen flag
void GridNav::scanLeftIr()
{
	Point tempPoint;
	irInMm.lft = 1000;//VIRTUALirs[3]->getDist();
	tempPoint = adjacentPoint(currPoint, facing, REL_LEFT);
	gridMap.setFlag(tempPoint, SEEN);
}

int GridNav::findPathProfit(unsigned char relDir)
{
	Serial.println();//DEBUG
	Serial.print("Path: ");//DEBUG
	printRelDirection(relDir);//DEBUG
	Serial.println();//DEBUG
	unsigned char tempFacing = findNewFacing(facing, relDir);
	int totalProfit = 0;
	if (facing == tempFacing)
		totalProfit += 5;
	Point tempPoint = adjacentPoint(currPoint, facing, relDir);
	Point rightPoint, leftPoint;
	while ( gridMap.contains(tempPoint))
	{
		Serial.print("F(");//DEBUG
		Serial.print(tempPoint.x);//DEBUG
		Serial.print(", ");//DEBUG
		Serial.print(tempPoint.y);//DEBUG
		Serial.print(")");//DEBUG
		//Profit of visited
		if (gridMap.isFlagSet(tempPoint, OCCUPIED))
			totalProfit += 0;
		else if(gridMap.isFlagSet(tempPoint, VISITED))
			totalProfit += 5;
		else if(gridMap.isFlagSet(tempPoint, SEEN))
			totalProfit += 10;
		else
			totalProfit += 15;
		
		//Profit of seen left and right points
		rightPoint = adjacentPoint(tempPoint, tempFacing, REL_RIGHT);
		leftPoint = adjacentPoint(tempPoint, tempFacing, REL_LEFT);
		if (gridMap.contains(rightPoint))
		{
			Serial.print(" R(");//DEBUG
			Serial.print(rightPoint.x);//DEBUG
			Serial.print(", ");//DEBUG
			Serial.print(rightPoint.y);//DEBUG
			Serial.print(")");//DEBUG
			if (gridMap.isFlagSet(rightPoint, OCCUPIED))
			totalProfit += 0;
			else if (gridMap.isFlagSet(rightPoint, VISITED))
				totalProfit += 5;
			else if (gridMap.isFlagSet(rightPoint, SEEN))
				totalProfit += 10;
			else
				totalProfit += 15;
		}
		if (gridMap.contains(leftPoint))
		{
			Serial.print(" L(");//DEBUG
			Serial.print(leftPoint.x);//DEBUG
			Serial.print(", ");//DEBUG
			Serial.print(leftPoint.y);//DEBUG
			Serial.print(")");//DEBUG
			if (gridMap.isFlagSet(leftPoint, OCCUPIED))
				totalProfit += 0;
			else if (gridMap.isFlagSet(leftPoint, VISITED))
				totalProfit += 5;
			else if (gridMap.isFlagSet(leftPoint, SEEN))
				totalProfit += 10;
			else
				totalProfit += 15;
		}
		tempPoint = adjacentPoint(tempPoint, tempFacing, REL_FRONT);
		Serial.println();
	}
	return totalProfit;
}

//TODO:	If path doesn't contain unknown, find closest unknown and use routing until gridmap
//		is filled with SEEN at minimum
//		Needs visited and seen counts
void GridNav::chooseNextPath()
{
	Point pastFront = adjacentPoint( adjacentPoint(currPoint, facing, REL_FRONT), facing, REL_FRONT);
	if (!gridMap.contains(pastFront) || gridMap.isFlagSet(pastFront, SEEN))
	{
		//Choose new path based on maximum profit
		int leftPathProfit = findPathProfit(REL_LEFT);
		int rightPathProfit = findPathProfit(REL_RIGHT);
		if (leftPathProfit > rightPathProfit)
		{
//VIRTUAL			mover->rotateAngle(TURN_LEFT, DEFAULT_SPEED);
			facing = findNewFacing(facing, REL_LEFT);
			moveToFrontPoint();
		}
		else
		{
//VIRTUAL			mover->rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(facing, REL_RIGHT);
			moveToFrontPoint();
		}
	}
	else
		moveToFrontPoint();
}

//Checks for blocks on adjacent points
void GridNav::checkForBlocks()
{
	gridMap.setFlag(currPoint, VISITED);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_FRONT)))
		scanFrontIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_RIGHT)))
		scanRightIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_LEFT)))
		scanLeftIr();

	if (gridMap.contains( adjacentPoint(currPoint, facing, REL_FRONT)) && isBlock(REL_FRONT) )
		haveBlock = obtainBlock(REL_FRONT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, REL_RIGHT)) && isBlock(REL_RIGHT) )
		haveBlock = obtainBlock(REL_RIGHT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, REL_LEFT)) && isBlock(REL_LEFT) )
		haveBlock = obtainBlock(REL_LEFT);
	else
	{
		chooseNextPath();
	}
}

void GridNav::initCheckForBlocks()
{
	gridMap.setFlag(currPoint, VISITED);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_FRONT)))
		scanFrontIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_RIGHT)))
		scanRightIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, REL_LEFT)))
		scanLeftIr();
	if (gridMap.contains( adjacentPoint(currPoint, facing, REL_FRONT)) && isBlock(REL_FRONT) )
		haveBlock = obtainBlock(REL_FRONT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, REL_RIGHT)) && isBlock(REL_RIGHT) )
		haveBlock = obtainBlock(REL_RIGHT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, REL_LEFT)) && isBlock(REL_LEFT) )
		haveBlock = obtainBlock(REL_LEFT);
	else
	{
		//Choose new path based on maximum profit
		int frontPathProfit = findPathProfit(REL_FRONT);
		int rightPathProfit = findPathProfit(REL_RIGHT);
		if (frontPathProfit > rightPathProfit)
		{
			moveToFrontPoint();
		}
		else
		{
//VIRTUAL			mover->rotateAngle(TURN_RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(facing, REL_RIGHT);
			moveToFrontPoint();
		}
	}
}

void GridNav::findBlock()
{
	irInMm.frnt = 1000;
	irInMm.rght = 1000;
	irInMm.bck = 1000;
	irInMm.lft = 1000;

	currPoint = Point(GRID_MAX_X, 0);

	haveBlock = false;
	grabSuccess = true;
	facing = DIR_WEST;

	initCheckForBlocks();
	printGrid();
	while (!haveBlock)
	{
		checkForBlocks();
		printGrid();
	}
	returnToEntrance();
}

/////////////////////// DEBUG PRINTING FUNCTIONS ///////////////////////
void GridNav::printDirection(unsigned char dir)
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

void GridNav::printRelDirection(unsigned char dir)
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

void GridNav::printGrid()
{
	Point tempPoint(0, 0);
        Serial.println();
	char buffer[50];
	for(int x = 0; x < GRID_MAX_X + 1 ; ++x)
	{
		for(int y = 0; y < GRID_MAX_Y + 1; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			sprintf(buffer, "[%c]", gridMap.getFlagsAsChar(tempPoint) );
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