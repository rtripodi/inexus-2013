#include "GridNav.h"

GridNav::GridNav(Motor *inMotor, Movement *inMovement, IR *inIrs[4], Claw *inClaw)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	claw = inClaw;
	gridMap = GridMap();
	router = Routing(&gridMap);
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
		return (irInMm.frnt < BLOCK_DIST_F);
	else if (relDir == LEFT)
		return (abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
		//(abs(BLOCK_DIST_L - irInMm.lft) < BLOCK_TOLERANCE);
	else if (relDir == RIGHT)
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
	currPoint = adjacentPoint(currPoint, facing, FRONT);
}

//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
//If the grab is unsuccessful, the robot will return to the point at which it sensed it
bool GridNav::obtainBlock(RelDir relDir)
{
	CarDir prevFacing = facing;
	Point tempPoint;
	tempPoint = adjacentPoint(currPoint, facing, relDir);
	gridMap.setFlag(tempPoint, OCCUPIED);
//VIRTUAL	mover->rotateDirection(relDir, DEFAULT_SPEED);
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
		currPoint = adjacentPoint(currPoint, facing, FRONT);
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
//VIRTUAL		mover->rotateDirection(BACK, DEFAULT_SPEED);
		facing = findNewFacing(facing, BACK);
		moveToFrontPoint();
//VIRTUAL		mover->rotateDirection(dirCarToRel(prevFacing), DEFAULT_SPEED);
		grabSuccess = false;
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

//Moves to given point
void GridNav::moveToPoint(Point pt)
{
	CarDir newDir = carDirOfPoint(pt);
//VIRTUAL	mover->rotateDirection(dirCarToRel(newDir), DEFAULT_SPEED);
	facing = newDir;
	moveToFrontPoint();
}

//Travels the current least expensive route to the entrance
void GridNav::returnToEntrance()
{
	Path path;
	Point entrPoint(GRID_MAX_X, 0);
	router.generateRoute(currPoint, entrPoint, facing, &path);
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
	tempPoint = adjacentPoint(currPoint, facing, FRONT);
	gridMap.setFlag(tempPoint, SEEN);
}
	
//Scan right point and set seen flag
void GridNav::scanRightIr()
{
	Point tempPoint;
	irInMm.rght = 1000;//VIRTUALirs[1]->getDist();
	tempPoint = adjacentPoint(currPoint, facing, RIGHT);
	gridMap.setFlag(tempPoint, SEEN);
}

//Scan left point and set seen flag
void GridNav::scanLeftIr()
{
	Point tempPoint;
	irInMm.lft = 1000;//VIRTUALirs[3]->getDist();
	tempPoint = adjacentPoint(currPoint, facing, LEFT);
	gridMap.setFlag(tempPoint, SEEN);
}

int GridNav::findPathProfit(RelDir relDir, unsigned char *numUnknown)
{
/*	Serial.println();//DEBUG
	Serial.print("Path: ");//DEBUG
	printRelDir(relDir);//DEBUG
	Serial.println();//DEBUG*/
	CarDir tempFacing = findNewFacing(facing, relDir);
	int totalProfit = 0;
	if (facing == tempFacing)
		totalProfit += 5;
	Point tempPoint = adjacentPoint(currPoint, facing, relDir);
	Point rightPoint, leftPoint;
	while ( gridMap.contains(tempPoint) && !gridMap.isFlagSet(tempPoint, OCCUPIED))
	{
/*		Serial.print("F(");//DEBUG
		Serial.print(tempPoint.x);//DEBUG
		Serial.print(", ");//DEBUG
		Serial.print(tempPoint.y);//DEBUG
		Serial.print(")");//DEBUG*/
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
/*			Serial.print(" R(");//DEBUG
			Serial.print(rightPoint.x);//DEBUG
			Serial.print(", ");//DEBUG
			Serial.print(rightPoint.y);//DEBUG
			Serial.print(")");//DEBUG*/
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
/*			Serial.print(" L(");//DEBUG
			Serial.print(leftPoint.x);//DEBUG
			Serial.print(", ");//DEBUG
			Serial.print(leftPoint.y);//DEBUG
			Serial.print(")");//DEBUG*/
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
			//stop
			Serial.println("Find other unknowns");//DEBUG
			haveBlock = true;//DEBUG: Allow end condition for testing
		}
		else
		{
			if (leftPathProfit > rightPathProfit)
			{
	//VIRTUAL			mover->rotateDirection(LEFT, DEFAULT_SPEED);
				facing = findNewFacing(facing, LEFT);
				moveToFrontPoint();
			}
			else
			{
	//VIRTUAL			mover->rotateDirection(RIGHT, DEFAULT_SPEED);
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
		//VIRTUAL			mover->rotateDirection(LEFT, DEFAULT_SPEED);
					facing = findNewFacing(facing, LEFT);
					moveToFrontPoint();
				}
				else
				{
		//VIRTUAL			mover->rotateDirection(RIGHT, DEFAULT_SPEED);
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
	
	//Shouldn't need to reset irValues, if not on map should disregard
	if(gridMap.contains( adjacentPoint(currPoint, facing, FRONT)))
		scanFrontIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)))
		scanRightIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, LEFT)))
		scanLeftIr();

	if (gridMap.contains( adjacentPoint(currPoint, facing, FRONT)) && isBlock(FRONT) )
		haveBlock = obtainBlock(FRONT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)) && isBlock(RIGHT) )
		haveBlock = obtainBlock(RIGHT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, LEFT)) && isBlock(LEFT) )
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
	if(gridMap.contains( adjacentPoint(currPoint, facing, FRONT)))
		scanFrontIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)))
		scanRightIr();
	if(gridMap.contains( adjacentPoint(currPoint, facing, LEFT)))
		scanLeftIr();
	if (gridMap.contains( adjacentPoint(currPoint, facing, FRONT)) && isBlock(FRONT) )
		haveBlock = obtainBlock(FRONT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, RIGHT)) && isBlock(RIGHT) )
		haveBlock = obtainBlock(RIGHT);
	else if (gridMap.contains( adjacentPoint(currPoint, facing, LEFT)) && isBlock(LEFT) )
		haveBlock = obtainBlock(LEFT);
	else
	{
		//Choose new path based on maximum profit
		unsigned char numUnknown = 0; // Sum of unknowns for paths
		int frontPathProfit = findPathProfit(FRONT, &numUnknown);
		int rightPathProfit = findPathProfit(RIGHT, &numUnknown);
		if (frontPathProfit > rightPathProfit)
		{
			moveToFrontPoint();
		}
		else
		{
//VIRTUAL			mover->rotateDirection(RIGHT, DEFAULT_SPEED);
			facing = findNewFacing(facing, RIGHT);
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
	facing = WEST;

	initCheckForBlocks();
	printGrid();
	while (!haveBlock)
	{
		checkForBlocks();
		printGrid();
	}
	returnToEntrance();
	Serial.println("Done.");
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