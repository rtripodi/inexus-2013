#include "GridNav.h"

#define SIMULATION
#define DEBUG

GridNav::GridNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs, Claw *inClaw)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	claw = inClaw;
	gridMap = GridMap();
	router = Routing(&gridMap);
	
	gridMap.setFlag(Point(GRID_MAX_X, 0), SEEN);
	
	#ifdef SIMULATION
		gridMap.setFlag(Point(2, 2), OCCUPIED);
		gridMap.setFlag(Point(3, 3), OCCUPIED);
		gridMap.setFlag(Point(4, 1), OCCUPIED);
	#endif
}

//Returns the closest point in the inputted relative direction
//Unexpected return for a relative direction causing a point off the grid
Point GridNav::adjacentPoint(Point pt, CarDir inFacing, RelDir relativeTurn)
{
	CarDir newCarDir = findNewFacing(inFacing, relativeTurn);
		
	switch(newCarDir)
	{
		case NORTH:
			pt.y += 1;
			break;
		case SOUTH:
			pt.y -= 1;
			break;
		case EAST:
			pt.x += 1;
			break;
		case WEST:
			pt.x -= 1;
			break;
	}
//	if (gridMap.contains(pt) )
		return pt;
}

//Returns the closest point in the inputted relative direction
//Unexpected return for a relative direction causing a point off the grid
Point GridNav::frontDiagPoint(Point pt, CarDir inFacing, RelDir relativeTurn)
{
	CarDir newCarDir = findNewFacing(inFacing, relativeTurn);
		
	if (inFacing == NORTH && newCarDir == EAST)
	{
		pt.x += 1;
		pt.y += 1;
	}
	else if (inFacing == NORTH && newCarDir == WEST)
	{
		pt.x -= 1;
		pt.y += 1;
	}
	else if (inFacing == SOUTH && newCarDir == EAST)
	{
		pt.x += 1;
		pt.y -= 1;
	}
	else if (inFacing == SOUTH && newCarDir == WEST)
	{
		pt.x -= 1;
		pt.y -= 1;
	}
//	if (gridMap.contains(pt) )
		return pt;
}

//Checks whether there is a block within a specified range of the inputted relative direction
//Returns false by default for a relative direction causing a point off the grid
bool GridNav::isBlock(RelDir relDir)
{
	if (relDir == FRONT)
		return (abs(irInMm.frnt - BLOCK_ACT_DIST) < BLOCK_TOLERANCE);
	else if (relDir == LEFT)
		return (abs(irInMm.lft - BLOCK_ACT_DIST) < BLOCK_TOLERANCE);
	else if (relDir == RIGHT)
		return (abs(irInMm.rght - BLOCK_ACT_DIST) < BLOCK_TOLERANCE);
	else
		return false;
}

//TODO: Return point moved to, then currPoint will be made equal to this 
void GridNav::moveToFrontPoint()
{
#ifndef SIMULATION
	mover->moveTillPoint(DEFAULT_SPEED);
	mover->moveOffCross(DEFAULT_SPEED);
#endif
	currPoint = adjacentPoint(currPoint, facing, FRONT);
}

//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
//If the grab is unsuccessful, the robot will return to the point at which it sensed it
bool GridNav::obtainBlock(RelDir relDir)
{
	bool isSuccess = true;
	bool reachedCross = false;
	claw->open();
	mover->rotateDirection(relDir, DEFAULT_SPEED);
	motors->stop();
	facing = findNewFacing(facing, relDir);
	irInMm.frnt = (irs->frnt)->getDist();
	while (irInMm.frnt > BLOCK_STOP && !reachedCross)
	{
		reachedCross = mover->moveForward(DEFAULT_SPEED);
		irInMm.frnt = (irs->frnt)->getDist();
	}
	motors->stop();
	
	//TODO: INSERT COLOUR CHECK HERE
	
	claw->close();
#ifndef SIMULATION
	if (mover->onCross() )
	{
		mover->moveOffCross();
		currPoint = adjacentPoint(currPoint, facing, FRONT);
	}
	else
	{
#endif
		moveToFrontPoint();
#ifndef SIMULATION
	}
#endif
	gridMap.setFlag(currPoint, VISITED);
	
	Point rightPoint = adjacentPoint(currPoint, facing, RIGHT);
	Point leftPoint = adjacentPoint(currPoint, facing, LEFT);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains(rightPoint))
		mapRightPoint();
	if(gridMap.contains(leftPoint))
		mapLeftPoint();
#ifndef SIMULATION
	irInMm.frnt = (irs->frnt)->getDist();
	if (irInMm.frnt <= BLOCK_STOP)
	{
#endif
		gridMap.removeFlag(currPoint, OCCUPIED);
		return true;
#ifndef SIMULATION
	}
	else
	{
		return false;
	}
#endif
}

//Returns the cardinal direction of a point given the inputted relative direction
CarDir GridNav::carDirOfPoint(Point pt)
{
	if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x))
		return NORTH;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x))
		return SOUTH;
	else if ((pt.y == currPoint.y) && (pt.x == currPoint.x + 1))
		return EAST;
	else if ((pt.y == currPoint.y) && (pt.x == currPoint.x - 1))
		return WEST;
/*	else if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x + 1))
		return DIR_NEAST;
	else if ((pt.y == currPoint.y + 1) && (pt.x == currPoint.x - 1))
		return DIR_NWEST;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x + 1))
		return DIR_SEAST;
	else if ((pt.y == currPoint.y - 1) && (pt.x == currPoint.x - 1))
		return DIR_SWEST;*/
}

//Moves to adjacent point
void GridNav::moveToAdjPoint(Point pt)
{
	CarDir newDir = carDirOfPoint(pt);
	mover->rotateDirection(dirCarToRel(newDir, facing), DEFAULT_SPEED);
	facing = newDir;
	moveToFrontPoint();
}

//Travels the current least expensive route to specified point
void GridNav::moveToPoint(Point pt)
{
	router.generateRoute(currPoint, pt, facing, &path);
	//TODO: Check next facing, turn and then scan. Then follow path
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToAdjPoint(path.path[ii]);
		
		gridMap.setFlag(currPoint, VISITED);
		
		if (gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)))
		{
			mapRightPoint();
		}
		if (gridMap.contains( adjacentPoint(currPoint, facing, LEFT)))
		{
			mapLeftPoint();
		}
		
		#ifdef DEBUG
			printGrid();
		#endif
	}
	motors->stop();
	claw->open();
}

//TODO: Implement properly (using obtainBlock functionality)
void GridNav::moveToBlock(Point pt)
{
	router.generateRoute(currPoint, pt, facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		moveToAdjPoint(path.path[ii]);
		
		gridMap.setFlag(currPoint, VISITED);
		
		if (gridMap.contains( adjacentPoint(currPoint, facing, FRONT)))
		{
			mapFrontPoint();
		}
		if (gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)))
		{
			mapRightPoint();
		}
		if (gridMap.contains( adjacentPoint(currPoint, facing, LEFT)))
		{
			mapLeftPoint();
		}
		
		#ifdef DEBUG
			printGrid();
		#endif
	}
	claw->close();
	motors->stop();
	haveBlock = true;
	gridMap.removeFlag(pt, OCCUPIED);
}

//Scan front point and set seen flag
void GridNav::mapFrontPoint()
{
	Point tempPoint;
#ifdef SIMULATION
irInMm.frnt = (irs->frnt)->getDistLarge();
#else
	irInMm.frnt = (irs->frnt)->getDist();
#endif
	tempPoint = adjacentPoint(currPoint, facing, FRONT);
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(FRONT))
		gridMap.setFlag(tempPoint, OCCUPIED);
}
	
//Scan right point and set seen flag
void GridNav::mapRightPoint()
{
	Point tempPoint;
#ifdef SIMULATION
irInMm.rght = (irs->rght)->getDistLarge();
#else
	irInMm.rght = (irs->rght)->getDist();
#endif
	tempPoint = adjacentPoint(currPoint, facing, RIGHT);
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(RIGHT))
		gridMap.setFlag(tempPoint, OCCUPIED);
}

//Scan left point and set seen flag
void GridNav::mapLeftPoint()
{
	Point tempPoint;
#ifdef SIMULATION
irInMm.lft = (irs->lft)->getDistLarge();
#else
	irInMm.lft = (irs->lft)->getDist();
#endif
	tempPoint = adjacentPoint(currPoint, facing, LEFT);
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(LEFT))
		gridMap.setFlag(tempPoint, OCCUPIED);
}

int GridNav::findPathProfit(RelDir relDir, unsigned char *numUnknown)
{
	CarDir tempFacing = findNewFacing(facing, relDir);
	int totalProfit = 0;
	if (facing == tempFacing)
		totalProfit += 5;
	Point tempPoint = adjacentPoint(currPoint, facing, relDir);
	Point rightPoint, leftPoint;
	while ( gridMap.contains(tempPoint) )//TODO: && !gridMap.isFlagSet(tempPoint, OCCUPIED))
	{
		//Profit of visited
		if (gridMap.isFlagSet(tempPoint, VISITED))
			totalProfit += 5;
		else if (gridMap.isFlagSet(tempPoint, SEEN))
			totalProfit += 10;
		else
		{
			totalProfit += 15;
			(*numUnknown)++;
		}
		
		//Profit of seen left and right points
		rightPoint = adjacentPoint(tempPoint, tempFacing, RIGHT);
		leftPoint = adjacentPoint(tempPoint, tempFacing, LEFT);
		if (gridMap.contains(rightPoint))
		{
			if (gridMap.isFlagSet(rightPoint, VISITED))
				totalProfit += 5;
			else if (gridMap.isFlagSet(rightPoint, SEEN))
				totalProfit += 10;
			else
			{
				totalProfit += 15;
				(*numUnknown)++;
			}
		}
		if (gridMap.contains(leftPoint))
		{
			if (gridMap.isFlagSet(leftPoint, VISITED))
				totalProfit += 5;
			else if (gridMap.isFlagSet(leftPoint, SEEN))
				totalProfit += 10;
			else
			{
				totalProfit += 15;
				(*numUnknown)++;
			}
		}
		tempPoint = adjacentPoint(tempPoint, tempFacing, FRONT);
	}
	return totalProfit;
}

//Doesn't take current direction facing into account 
Point GridNav::closestUnknown()
{
	Point tempPoint, closestPoint = Point(-1, -1);
	int tempDist, minDist = 100;
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			if (!gridMap.isFlagSet(tempPoint, SEEN))
			{
				tempDist = (abs(tempPoint.y - currPoint.y) + abs(tempPoint.x - currPoint.x));
				if (tempDist < minDist)
				{
					closestPoint = tempPoint;
					minDist = tempDist;
				}
			}
		}
	}
	#ifdef DEBUG
		Serial.print("\t\t\t\t");
		closestPoint.debug("Closest Unknown Point");
		Serial.println();
	#endif
	return closestPoint;
}

//TODO: Exact same as closestUnknown.  Should be combined.
Point GridNav::closestBlock()
{
	Point tempPoint, closestPoint = Point(-1, -1);
	int tempDist, minDist = 100;
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			if (gridMap.isFlagSet(tempPoint, OCCUPIED))
			{
				tempDist = (abs(tempPoint.y - currPoint.y) + abs(tempPoint.x - currPoint.x));
				if (tempDist < minDist)
				{
					closestPoint = tempPoint;
					minDist = tempDist;
				}
			}
		}
	}
	#ifdef DEBUG
		Serial.print("\t\t\t\t");
		closestPoint.debug("Closest Block");
		Serial.println();
	#endif
	return closestPoint;
}

void GridNav::startNextPath()
{
	unsigned char numUnknownFront = 0,
		numUnknownLeft = 0,
		numUnknownRight = 0;
	
	int frontPathProfit = findPathProfit(FRONT, &numUnknownFront);
	int leftPathProfit = findPathProfit(LEFT, &numUnknownLeft);
	int rightPathProfit = findPathProfit(RIGHT, &numUnknownRight);
	
	if ( (numUnknownFront + numUnknownLeft + numUnknownRight) == 0)
	{
		moveToPoint(closestUnknown() );
	}
	else
	{
		if ( (frontPathProfit >= leftPathProfit) && (frontPathProfit >= rightPathProfit) )
		{
			moveToFrontPoint();
		}
		else if (leftPathProfit >= rightPathProfit)
		{
			mover->rotateDirection(LEFT, DEFAULT_SPEED);
			facing = findNewFacing(facing, LEFT);
			moveToFrontPoint();
		}
		else
		{
			mover->rotateDirection(RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(facing, RIGHT);
			moveToFrontPoint();
		}
	}
}

void GridNav::chooseNextPath()
{
	Point pastFront = adjacentPoint( adjacentPoint(currPoint, facing, FRONT), facing, FRONT);

	if ( !gridMap.contains(adjacentPoint(currPoint, facing, FRONT)))
	{
		startNextPath();
	}
	else if (!gridMap.contains(pastFront) || gridMap.isFlagSet(pastFront, SEEN))
	{
		Point leftDiag = frontDiagPoint(currPoint, facing, LEFT);
		Point rightDiag = frontDiagPoint(currPoint, facing, RIGHT);
		
		if ( !gridMap.contains(leftDiag) || !gridMap.contains(rightDiag) || (gridMap.isFlagSet(leftDiag, SEEN) && gridMap.isFlagSet(rightDiag, SEEN)))
		{
			startNextPath();
		}
		else
		{
			moveToFrontPoint();
		}
	}
	else
	{
		moveToFrontPoint();
	}
}

//Checks for blocks on adjacent points
void GridNav::checkForBlocks()
{
	gridMap.setFlag(currPoint, VISITED);
	
	Point frontPoint = adjacentPoint(currPoint, facing, FRONT);
	Point rightPoint = adjacentPoint(currPoint, facing, RIGHT);
	Point leftPoint = adjacentPoint(currPoint, facing, LEFT);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if (gridMap.contains(frontPoint))
	{
		mapFrontPoint();
	}
	if (gridMap.contains(rightPoint))
	{
		mapRightPoint();
	}
	if (gridMap.contains(leftPoint))
	{
		mapLeftPoint();
	}
#ifndef SIMULATION
	if (abs((irs->frnt)->getDist() - BLOCK_STOP) < BLOCK_TOLERANCE)	//TODO: Test improvised block grab
	{
		claw->shut();
		haveBlock = true;
	}
	else if (gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
#else
if (gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
#endif
		haveBlock = obtainBlock(FRONT);
	else if (gridMap.contains(rightPoint) && gridMap.isFlagSet(rightPoint, OCCUPIED))
	{
		haveBlock = obtainBlock(RIGHT);
	}
	else if (gridMap.contains(leftPoint) && gridMap.isFlagSet(leftPoint, OCCUPIED))
	{
		haveBlock = obtainBlock(LEFT);
	}
	else
	{
		chooseNextPath();
	}
}

void GridNav::findBlock()
{
	irInMm.frnt = 1000;
	irInMm.rght = 1000;
	irInMm.bck = 1000;
	irInMm.lft = 1000;

	currPoint = Point(GRID_MAX_X, 0);
	Point entrPoint = currPoint;

	printGrid();
	
	haveBlock = false;
	facing = WEST;
	
	Point invalidPt = Point(-1,-1);
	Point closestOccupied = closestBlock();
	
#ifndef SIMULATION
	if (closestOccupied != invalidPt)
	{
		moveToBlock(closestOccupied);
	}
#endif
	while (!haveBlock)
	{
		checkForBlocks();
		#ifdef DEBUG
			printGrid();
		#endif
	}
	#ifdef DEBUG
		Serial.println();
		Serial.println("-----\t\t\tReturning\t\t\t-----");
	#endif
	Point tempPoint = Point(GRID_MAX_X + 1, 0);
	moveToPoint(entrPoint);
	CarDir newDir = carDirOfPoint(tempPoint);
	mover->rotateDirection(dirCarToRel(newDir, facing), DEFAULT_SPEED);
	facing = newDir;
	#ifdef DEBUG
		Serial.print("Done.\tFacing = ");
		printCarDir(facing);
		Serial.println();
	#endif
}

/////////////////////// DEBUG PRINTING FUNCTIONS ///////////////////////
void GridNav::printCarDir(CarDir carDir)
{
	switch(carDir)
	{
		case NORTH:
			Serial.print("North");
			break;
		case SOUTH:
			Serial.print("South");
			break;
		case EAST:
			Serial.print("East");
			break;
		case WEST:
			Serial.print("West");
			break;
	}
}

void GridNav::printRelDir(RelDir relDir)
{
	switch(relDir)
	{
		case FRONT:
			Serial.print("Front");
			break;
		case BACK:
			Serial.print("Back");
			break;
		case RIGHT:
			Serial.print("Right");
			break;
		case LEFT:
			Serial.print("Left");
			break;
	}
}

void GridNav::printGrid()
{
	Point tempPoint(0, 0);
    Serial.println();
	char buffer[50];
	Serial.print(" ");
	for(int y = 0; y < GRID_MAX_Y + 1; ++y)
	{
		sprintf(buffer, "  %d", y);
		Serial.print(buffer);
	}
	Serial.println();
	for(int x = 0; x < GRID_MAX_X + 1 ; ++x)
	{
		sprintf(buffer, "%i ", x);
		Serial.print(buffer);
		for(int y = 0; y < GRID_MAX_Y + 1; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			if ((currPoint.x == x) && (currPoint.y == y))
			{
				switch (facing)
				{
					case NORTH:
						sprintf(buffer, "[>]");
						break;
					case EAST:
						sprintf(buffer, "[v]");
						break;
					case SOUTH:
						sprintf(buffer, "[<]");
						break;
					case WEST:
						sprintf(buffer, "[^]");
						break;
				}
			}
			else
				sprintf(buffer, "[%c]", gridMap.getFlagsAsChar(tempPoint) );
			Serial.print(buffer);
		}
		Serial.println();
	}
	Serial.println();
	currPoint.debug("Current");
	Serial.print("\tFacing = ");
	printCarDir(facing);
	Serial.println();
}
/////////////////////// END DEBUG PRINTING FUNCTIONS ///////////////////////

void GridNav::debugIrs()
{
	irInMm.frnt = (irs->frnt)->getDist();
	irInMm.rght = (irs->rght)->getDist();
	irInMm.bck = (irs->bck)->getDist();
	irInMm.lft = (irs->lft)->getDist();
	
	Serial.print("Reading - ");
	Serial.print("\tF: ");
	Serial.print(irInMm.frnt);
	Serial.print("\tR: ");
	Serial.print(irInMm.rght);
	Serial.print("\tB: ");
	Serial.print(irInMm.bck);
	Serial.print("\tL: ");
	Serial.println(irInMm.lft);
}
