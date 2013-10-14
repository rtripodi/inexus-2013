#include "GridNav.h"

GridNav::GridNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs, Claw *inClaw)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	claw = inClaw;
	gridMap = GridMap();
	router = Routing(&gridMap);
	
	//DEBUG START
/*	Point pt1 = Point(2, 2);
	Point pt2 = Point(3, 3);
	Point pt3 = Point(4, 1);
	
	gridMap.setFlag(pt1, OCCUPIED);
	gridMap.setFlag(pt2, OCCUPIED);
	gridMap.setFlag(pt3, OCCUPIED);*/
	//DEBUG END
}

//Returns the new facing cardinal direction given a turn in the relative direction
//Unexpected return for inputs other then: Front, Right, Back, Left
CarDir GridNav::findNewFacing(CarDir inFacing, RelDir relativeTurn)
{
	if ((inFacing == NORTH && relativeTurn == FRONT) || (inFacing == EAST && relativeTurn == LEFT) || (inFacing == SOUTH && relativeTurn == BACK) || (inFacing == WEST && relativeTurn == RIGHT))
		return NORTH;
	else if ((inFacing == NORTH && relativeTurn == RIGHT) || (inFacing == EAST && relativeTurn == FRONT) || (inFacing == SOUTH && relativeTurn == LEFT) || (inFacing == WEST && relativeTurn == BACK))
		return EAST;
	else if ((inFacing == NORTH && relativeTurn == BACK) || (inFacing == EAST && relativeTurn == RIGHT) || (inFacing == SOUTH && relativeTurn == FRONT) || (inFacing == WEST && relativeTurn == LEFT))
		return SOUTH;
	else if ((inFacing == NORTH && relativeTurn == LEFT) || (inFacing == EAST && relativeTurn == BACK) || (inFacing == SOUTH && relativeTurn == RIGHT) || (inFacing == WEST && relativeTurn == FRONT))
		return WEST;
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
		return (irInMm.frnt < CENTRE_DIST);
	else if (relDir == LEFT)
		return (irInMm.lft < CENTRE_DIST);
	else if (relDir == RIGHT)
		return (irInMm.rght < CENTRE_DIST);
	else
		return false;
}

//TODO: Return point moved to, then currPoint will be made equal to this 
void GridNav::moveToFrontPoint()
{
	mover->moveTillPoint(DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
	mover->moveOffCross(DEFAULT_SPEED);//VIRTUAL
	currPoint = adjacentPoint(currPoint, facing, FRONT);
}

//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
//If the grab is unsuccessful, the robot will return to the point at which it sensed it
bool GridNav::obtainBlock(RelDir relDir)
{
	bool isSuccess = true;
	
	mover->rotateDirection(relDir, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
	motors->stop();
	facing = findNewFacing(facing, relDir);
	claw->open();
	irInMm.frnt = (irs->frnt)->getDist();//DEBUG
	while (irInMm.frnt > BLOCK_STOP)
	{
		mover->moveForward(DEFAULT_SPEED);//VIRTUAL
		irInMm.frnt = (irs->frnt)->getDist();//DEBUG
		if (!mover->onCross())
		{
			isSuccess = false;
			break;
		}
	}
	motors->stop();//VIRTUAL
	
	//TODO: INSERT COLOUR CHECK HERE
	
	claw->close();//VIRTUAL
	if (mover->onCross() )//VIRTUAL START: DEBUG Causes hang
	{
		mover->moveOffCross();
		currPoint = adjacentPoint(currPoint, facing, FRONT);
	}
	else if (isSuccess)
	{
		moveToFrontPoint();
	}
	else
		currPoint = adjacentPoint(currPoint, facing, FRONT);//VIRTUAL END
	irInMm.frnt = (irs->frnt)->getDist();//DEBUG
	if (irInMm.frnt <= BLOCK_STOP)
	{
		gridMap.removeFlag(currPoint, OCCUPIED);
		return true;
	}
	else
	{
		return false;
	}
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

//Turns the robot in the desired cardinal direction
//Unexpected results for inputs other then: North, East, South, West
RelDir GridNav::dirCarToRel(CarDir newCardDir)
{
	if (facing == newCardDir)
		return FRONT;
	else if ((facing == NORTH && newCardDir == SOUTH) || (facing == EAST && newCardDir == WEST) || (facing == SOUTH && newCardDir == NORTH) || (facing == WEST && newCardDir == EAST))
		return BACK;
	else if ((facing == NORTH && newCardDir == WEST) || (facing == EAST && newCardDir == NORTH) || (facing == SOUTH && newCardDir == EAST) || (facing == WEST && newCardDir == SOUTH))
		return LEFT;
	else if ((facing == NORTH && newCardDir == EAST) || (facing == EAST && newCardDir == SOUTH) || (facing == SOUTH && newCardDir == WEST) || (facing == WEST && newCardDir == NORTH))
		return RIGHT;
}

//Moves to adjacent point
void GridNav::moveToAdjPoint(Point pt)
{
	CarDir newDir = carDirOfPoint(pt);
	mover->rotateDirection(dirCarToRel(newDir), DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
	facing = newDir;
	moveToFrontPoint();
}

//Travels the current least expensive route to specified point
void GridNav::moveToPoint(Point pt)
{
	router.generateRoute(currPoint, pt, facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		path.path[ii].debug("curr");//DEBUG
		moveToAdjPoint(path.path[ii]);
	}	
	motors->stop();//VIRTUAL
	claw->open();
}

//TODO: Implement properly (using obtainBlock functionality)
void GridNav::moveToBlock(Point pt)
{
	router.generateRoute(currPoint, pt, facing, &path);
	for (int ii = 0; ii < path.length; ++ii)
	{
		path.path[ii].debug("curr");//DEBUG
		moveToAdjPoint(path.path[ii]);
	}
	claw->close();
	motors->stop();//VIRTUAL
	haveBlock = true;
	gridMap.removeFlag(pt, OCCUPIED);
}

//Scan front point and set seen flag
void GridNav::mapFrontPoint()
{
	Point tempPoint;
	irInMm.frnt = (irs->frnt)->getDist();//DEBUG
/*	Serial.print("Front: ");//DEBUG
	Serial.println(irInMm.frnt);//DEBUG*/
	tempPoint = adjacentPoint(currPoint, facing, FRONT);
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(FRONT))
		gridMap.setFlag(tempPoint, OCCUPIED);
}
	
//Scan right point and set seen flag
void GridNav::mapRightPoint()
{
	Point tempPoint;
	irInMm.rght = (irs->rght)->getDist();//DEBUG
/*	Serial.print("Right: ");//DEBUG
	Serial.println(irInMm.rght);//DEBUG*/
	tempPoint = adjacentPoint(currPoint, facing, RIGHT);
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(RIGHT))
		gridMap.setFlag(tempPoint, OCCUPIED);
}

//Scan left point and set seen flag
void GridNav::mapLeftPoint()
{
	Point tempPoint;
	irInMm.lft = (irs->lft)->getDist();//DEBUG
/*	Serial.print("Left: ");//DEBUG
	Serial.println(irInMm.lft);//DEBUG*/
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
	while ( gridMap.contains(tempPoint) )//DEBUG&& !gridMap.isFlagSet(tempPoint, OCCUPIED))
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

Point GridNav::closestUnknown()
{
	Point tempPoint, closestPoint;
	int minDist = 100;
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			tempPoint.x = currPoint.x;
			tempPoint.y = currPoint.y;
			if (!gridMap.isFlagSet(tempPoint, SEEN) && (abs(tempPoint.y - currPoint.y) + abs(tempPoint.x - currPoint.x)) < minDist)
				closestPoint = tempPoint;
		}
	}
	return closestPoint;
}

Point GridNav::closestBlock()
{
	Point tempPoint, closestPoint = Point(-1,-1);
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			if (gridMap.isFlagSet(tempPoint, OCCUPIED))
				closestPoint = tempPoint;
		}
	}
	return closestPoint;
}

//TODO:	If path doesn't contain unknown, find closest unknown and use routing until gridmap
//		is filled with SEEN at minimum
//		Needs visited and seen counts
void GridNav::chooseNextPath()
{
	Point pastFront = adjacentPoint( adjacentPoint(currPoint, facing, FRONT), facing, FRONT);

	if(!gridMap.contains(adjacentPoint(currPoint, facing, FRONT)))
	{
		unsigned char numUnknown = 0; // Sum of unknowns for paths
		int leftPathProfit = findPathProfit(LEFT, &numUnknown);
		int rightPathProfit = findPathProfit(RIGHT, &numUnknown);
		if(numUnknown == 0)
		{
			moveToPoint(closestUnknown() );
		}
		else
		{
			if (leftPathProfit > rightPathProfit)
			{
				mover->rotateDirection(LEFT, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
				facing = findNewFacing(facing, LEFT);
				moveToFrontPoint();
			}
			else
			{
				mover->rotateDirection(RIGHT, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
				facing = findNewFacing(facing, RIGHT);
				moveToFrontPoint();
			}
		}
	}
	else if (!gridMap.contains(pastFront) || gridMap.isFlagSet(pastFront, SEEN))
	{
		Point leftDiag = frontDiagPoint(currPoint, facing, LEFT);
		Point rightDiag = frontDiagPoint(currPoint, facing, RIGHT);
		
		if ( !gridMap.contains(leftDiag) || !gridMap.contains(rightDiag) || (gridMap.isFlagSet(leftDiag, SEEN) && gridMap.isFlagSet(rightDiag, SEEN)))
		{
			unsigned char numUnknown = 0; // Sum of unknowns for paths
			int leftPathProfit = findPathProfit(LEFT, &numUnknown);
			int rightPathProfit = findPathProfit(RIGHT, &numUnknown);
			if(numUnknown == 0)
			{
				//stop
				Serial.println("Find other unknowns");//DEBUG
				haveBlock = true;//DEBUG: Allow end condition for testing
			}
			else
			{
				if (leftPathProfit > rightPathProfit)
				{
					mover->rotateDirection(LEFT, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
					facing = findNewFacing(facing, LEFT);
					moveToFrontPoint();
				}
				else
				{
					mover->rotateDirection(RIGHT, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
					facing = findNewFacing(facing, RIGHT);
					moveToFrontPoint();
				}
			}
		}
		else
			moveToFrontPoint();
	}
	else
		moveToFrontPoint();
}

//Checks for blocks on adjacent points
void GridNav::checkForBlocks()
{
	gridMap.setFlag(currPoint, VISITED);
	
	Point frontPoint = adjacentPoint(currPoint, facing, FRONT);
	Point rightPoint = adjacentPoint(currPoint, facing, RIGHT);
	Point leftPoint = adjacentPoint(currPoint, facing, LEFT);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains(frontPoint))
		mapFrontPoint();
	if(gridMap.contains(rightPoint))
		mapRightPoint();
	if(gridMap.contains(leftPoint))
		mapLeftPoint();
	if (gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
		haveBlock = obtainBlock(FRONT);
	else if (gridMap.contains(rightPoint) && gridMap.isFlagSet(rightPoint, OCCUPIED))
		haveBlock = obtainBlock(RIGHT);
	else if (gridMap.contains(leftPoint) && gridMap.isFlagSet(leftPoint, OCCUPIED))
		haveBlock = obtainBlock(LEFT);
	else
	{
		chooseNextPath();
	}
}

void GridNav::initCheckForBlocks()
{
	gridMap.setFlag(currPoint, VISITED);

	//Shouldn't need to reset irValues, if not on map should disregard
	Point frontPoint = adjacentPoint(currPoint, facing, FRONT);
	Point rightPoint = adjacentPoint(currPoint, facing, RIGHT);
	Point leftPoint = adjacentPoint(currPoint, facing, LEFT);
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains(frontPoint))
		mapFrontPoint();
	if(gridMap.contains(rightPoint))
		mapRightPoint();
	if(gridMap.contains(leftPoint))
		mapLeftPoint();
	if (gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
		haveBlock = obtainBlock(FRONT);
	else if (gridMap.contains(rightPoint) && gridMap.isFlagSet(rightPoint, OCCUPIED))
		haveBlock = obtainBlock(RIGHT);
	else if (gridMap.contains(leftPoint) && gridMap.isFlagSet(leftPoint, OCCUPIED))
		haveBlock = obtainBlock(LEFT);
	else
	{
		//Choose new path based on maximum profit
		unsigned char numUnknown = 0; // Sum of unknowns for paths
		int frontPathProfit = findPathProfit(FRONT, &numUnknown);
		int rightPathProfit = findPathProfit(RIGHT, &numUnknown);
		if(numUnknown == 0)
		{
			moveToPoint(closestUnknown() );
		}
		else
		{
			if (frontPathProfit > rightPathProfit)
			{
				moveToFrontPoint();
			}
			else
			{
				mover->rotateDirection(RIGHT, DEFAULT_SPEED);//VIRTUAL: DEBUG Causes hang
				facing = findNewFacing(facing, RIGHT);
				moveToFrontPoint();
			}
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
	Point entrPoint = currPoint;

	printGrid();
	
	haveBlock = false;
	facing = WEST;
	
	Point invalidPt = Point(-1,-1);
	Point closestOccupied = closestBlock();
	
	//DEBUG START
	Serial.print("Current: (");
	Serial.print(currPoint.x);
	Serial.print(", ");
	Serial.print(currPoint.y);
	Serial.print(")\tFacing: ");
	printCarDir(facing);
	Serial.println();
	//DEBUG END*/
	
	if (closestOccupied != invalidPt)
	{
		moveToBlock(closestOccupied);
	}
	else
	{
		initCheckForBlocks();
		printGrid();
	}
	while (!haveBlock)
	{
		checkForBlocks();
		printGrid();//DEBUG
	}
	Serial.println("Returning");//DEBUG
	moveToPoint(entrPoint);
	//TODO: Face East
	Serial.println("Done.");//DEBUG
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
