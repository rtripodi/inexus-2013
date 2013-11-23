/*
	UNTESTED:
		Accounting for case where multiple blocks can be seen from current point
		Accounting for ghost block
		Accounting for immediately going to occupied block without colour flags
		Accounting for process of elimination to determine unknown colour
	
	NOTE: An infinite loop occurs when trying to move to a closest unknown point that is unreachable
*/

#include "GridNav.h"

//#define SIMULATION
//#define DEBUG

GridNav::GridNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs, Claw *inClaw, Colour *inColour, bool isAvoidBlocks)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	claw = inClaw;
	colourSensor = inColour;
	gridMap = GridMap();
	gridMap.setAvoidBlocks(isAvoidBlocks);
	router = Routing(&gridMap);
	
	gridMap.setFlag(Point(GRID_MAX_X, 0), SEEN);
	attempt = 0;
	coloursScanned = 0;
	
	isColourMode = true; //default
	
#ifdef SIMULATION
Point redPoint = Point(6, 2);
Point greenPoint = Point(5, 1);
Point bluePoint = Point(4, 0);

gridMap.setFlag(redPoint, OCCUPIED);
gridMap.setFlag(greenPoint, OCCUPIED);
gridMap.setFlag(bluePoint, OCCUPIED);

gridMap.setFlag(redPoint, RED);
gridMap.setFlag(greenPoint, GREEN);
gridMap.setFlag(bluePoint, BLUE);
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

void GridNav::moveToPreviousPoint()
{
#ifndef SIMULATION
	mover->moveTillPoint(-DEFAULT_SPEED);
#endif
	currPoint = adjacentPoint(currPoint, facing, BACK);
}

//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
//If the grab is unsuccessful, the robot will return to the point at which it sensed it
bool GridNav::obtainBlock(RelDir relDir)
{
	CarDir prevFacing = facing;
	Point prevPoint = currPoint;
	
	bool isSuccess = true;
	bool reachedCross = false;
	Colour::ColourType blockColour;
	claw->open();
	mover->rotateDirection(relDir, DEFAULT_SPEED);
	motors->stop();
	facing = findNewFacing(facing, relDir);
	Point blockPoint = adjacentPoint(currPoint, facing, FRONT);
	
	#ifdef DEBUG
		Serial.print("Observing grab block at");
		blockPoint.debug("");
		Serial.println();
	#endif
	
	//Try to approach block and stop before making contact
	irInMm.frnt = (irs->frnt)->getDist();
	while (irInMm.frnt > BLOCK_STOP && !reachedCross)
	{
		reachedCross = mover->moveForward(DEFAULT_SPEED);
		irInMm.frnt = (irs->frnt)->getDist();
	}
	motors->stop();
	
	//If ghost block, ignore it.
#ifndef SIMULATION
	if (reachedCross)
	{
		irInMm.frnt = (irs->frnt)->getDist();
		if (irInMm.frnt > BLOCK_STOP)
		{
			#ifdef DEBUG
				Serial.println("Ignoring 'ghost block'.");
			#endif
			mover->moveOffCross();
			currPoint = blockPoint;
			gridMap.removeFlag(blockPoint, OCCUPIED);
			return false;
		}
	}
#endif
	
if (isColourMode)
{
#ifndef SIMULATION
	blockColour = colourSensor->senseColour();
#else
if (gridMap.isFlagSet(blockPoint, RED))
	blockColour = Colour::red;
else if (gridMap.isFlagSet(blockPoint, GREEN))
	blockColour = Colour::green;
else if (gridMap.isFlagSet(blockPoint, BLUE))
	blockColour = Colour::blue;
#endif
	
	#ifdef DEBUG
		Serial.print("Colour of block: ");
		colourSensor->printColour(blockColour);
		Serial.println();
	#endif
	
	switch (blockColour)
	{
		case Colour::red:
			gridMap.setFlag(blockPoint, RED);
			gridMap.setRedPoint(blockPoint);
			coloursScanned++;
			//if (attempt != 1)	should never occur
			//if it does, the colour sensor has failed
			//and the bot will just have to grab the block
			break;
		case Colour::green:
			gridMap.setFlag(blockPoint, GREEN);
			gridMap.setGreenPoint(blockPoint);
			coloursScanned++;
			if (attempt != 2)
			{
				#ifdef DEBUG
					Serial.println("Unwanted colour. Returning to previous point.");
				#endif
				currPoint = blockPoint;
				moveToPreviousPoint();
				return false;
			}
			break;
		case Colour::blue:
			gridMap.setFlag(blockPoint, BLUE);
			gridMap.setBluePoint(blockPoint);
			coloursScanned++;
			if (attempt != 3)
			{
				if ( (gridMap.getBlockCount() == NUMBER_OF_BLOCKS) && (coloursScanned >= 2) )
				{
					#ifdef DEBUG
						Serial.println("Not Green.  Moving to estimated Green location.");
					#endif
					setLastColour();
					currPoint = blockPoint;
					moveToPreviousPoint();
					grabBlockAtPoint(gridMap.getGreenPoint());
					return true;
				}
				else
				{
					#ifdef DEBUG
						Serial.println("Unwanted colour. Returning to previous point.");
					#endif
					currPoint = blockPoint;
					moveToPreviousPoint();
					return false;
				}
			}
			break;
		default:
			currPoint = blockPoint;
			moveToPreviousPoint();
			return false;
			break;
	}
}//end colourMode
	
	claw->close();
#ifndef SIMULATION
	if (reachedCross)
	{
		mover->moveOffCross();
		currPoint = blockPoint;
	}
	else
	{
#endif
		moveToFrontPoint();
#ifndef SIMULATION
	}
#endif
	
	//Update GridMap
	gridMap.setFlag(currPoint, VISITED);
	
	Point rightPoint = adjacentPoint(currPoint, facing, RIGHT);
	Point leftPoint = adjacentPoint(currPoint, facing, LEFT);
	
	if(gridMap.contains(rightPoint))
		mapRightPoint();
	if(gridMap.contains(leftPoint))
		mapLeftPoint();
	
	//Check if grab was successful
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
bool GridNav::moveToPoint(Point pt)
{
	router.generateRoute(currPoint, pt, facing, &path);
	if (path.length == 0)
	{
		return false;
	}
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
	return true;
}

//TODO: Implement properly (using obtainBlock functionality)
void GridNav::grabBlockAtPoint(Point pt)
{
	if (gridMap.contains(pt))
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
}

//Scan front point and set seen flag
void GridNav::mapFrontPoint()
{
	Point tempPoint = adjacentPoint(currPoint, facing, FRONT);
#ifdef SIMULATION
irInMm.frnt = (irs->frnt)->getDistLarge();
if(gridMap.isFlagSet(tempPoint, OCCUPIED))
{
	gridMap.setBlockPoint(tempPoint);
}
#else
	irInMm.frnt = (irs->frnt)->getDist();
#endif
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(FRONT))
	{
		gridMap.setFlag(tempPoint, OCCUPIED);
		gridMap.setBlockPoint(tempPoint);
	}
}
	
//Scan right point and set seen flag
void GridNav::mapRightPoint()
{
	Point tempPoint = adjacentPoint(currPoint, facing, RIGHT);
#ifdef SIMULATION
irInMm.rght = (irs->rght)->getDistLarge();
if(gridMap.isFlagSet(tempPoint, OCCUPIED))
{
	gridMap.setBlockPoint(tempPoint);
}
#else
	irInMm.rght = (irs->rght)->getDist();
#endif
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(RIGHT))
	{
		gridMap.setFlag(tempPoint, OCCUPIED);
		gridMap.setBlockPoint(tempPoint);
	}
}

//Scan left point and set seen flag
void GridNav::mapLeftPoint()
{
	Point tempPoint = adjacentPoint(currPoint, facing, LEFT);
#ifdef SIMULATION
irInMm.lft = (irs->lft)->getDistLarge();
if(gridMap.isFlagSet(tempPoint, OCCUPIED))
{
	gridMap.setBlockPoint(tempPoint);
}
#else
	irInMm.lft = (irs->lft)->getDist();
#endif
	gridMap.setFlag(tempPoint, SEEN);
	if (isBlock(LEFT))
	{
		gridMap.setFlag(tempPoint, OCCUPIED);
		gridMap.setBlockPoint(tempPoint);
	}
}

int GridNav::findPathProfit(RelDir relDir, unsigned char *numUnknown)
{
	#ifdef DEBUG
		printRelDir(relDir);
		Serial.println(" Path:");
	#endif
	CarDir tempFacing = findNewFacing(facing, relDir);
	int totalProfit = 0;
	if (facing == tempFacing)
		totalProfit += FACING_PROFIT;
	Point tempPoint = adjacentPoint(currPoint, facing, relDir);
	Point rightPoint, leftPoint;
#ifdef SIMULATION	//Needed for simulation because it knows OCCUPIED points
bool isVisitedBlock = false;
for (int ii = 0; ii < NUMBER_OF_BLOCKS; ++ii)
{
	if (tempPoint == gridMap.getBlockPoint(ii))
	{
		isVisitedBlock = true;
	}
}
while ( gridMap.contains(tempPoint) && !isVisitedBlock)
	{
		for (int ii = 0; ii < NUMBER_OF_BLOCKS; ++ii)
		{
			if (tempPoint == gridMap.getBlockPoint(ii))
				{
					isVisitedBlock = true;
				}
		}
#else
	while ( gridMap.contains(tempPoint) && !gridMap.isFlagSet(tempPoint, OCCUPIED))
	{
#endif
		#ifdef DEBUG
			tempPoint.debug("\tPoint");
			Serial.println();
		#endif
		//Profit of visited
		if (gridMap.isFlagSet(tempPoint, VISITED))
			totalProfit += VISITED_PROFIT;
		else if (gridMap.isFlagSet(tempPoint, SEEN))
			totalProfit += SEEN_PROFIT;
		else
		{
			totalProfit += UNKNOWN_PROFIT;
			(*numUnknown)++;
		}
		
		//Profit of seen left and right points
		rightPoint = adjacentPoint(tempPoint, tempFacing, RIGHT);
		leftPoint = adjacentPoint(tempPoint, tempFacing, LEFT);
		if (gridMap.contains(rightPoint))
		{
			if (gridMap.isFlagSet(rightPoint, VISITED))
				totalProfit += VISITED_PROFIT;
			else if (gridMap.isFlagSet(rightPoint, SEEN))
				totalProfit += SEEN_PROFIT;
			else
			{
				totalProfit += UNKNOWN_PROFIT;
				(*numUnknown)++;
			}
		}
		if (gridMap.contains(leftPoint))
		{
			if (gridMap.isFlagSet(leftPoint, VISITED))
				totalProfit += VISITED_PROFIT;
			else if (gridMap.isFlagSet(leftPoint, SEEN))
				totalProfit += SEEN_PROFIT;
			else
			{
				totalProfit += UNKNOWN_PROFIT;
				(*numUnknown)++;
			}
		}
		tempPoint = adjacentPoint(tempPoint, tempFacing, FRONT);
	}
	#ifdef DEBUG
		Serial.print("\tProfit: ");
		Serial.println(totalProfit);
		Serial.println();
	#endif
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

//Doesn't take current direction facing into account 
Point GridNav::closestNonVisited()
{
	Point tempPoint, closestPoint = Point(-1, -1);
	int tempDist, minDist = 100;
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			tempPoint.x = x;
			tempPoint.y = y;
			if (!gridMap.isFlagSet(tempPoint, VISITED))
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
		closestPoint.debug("Closest NonVisited Point");
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
	#ifdef DEBUG
		Serial.println("Finding best path.");
	#endif
	
	unsigned char numUnknownFront = 0,
		numUnknownLeft = 0,
		numUnknownRight = 0;
	
	int frontPathProfit = findPathProfit(FRONT, &numUnknownFront);
	int leftPathProfit = findPathProfit(LEFT, &numUnknownLeft);
	int rightPathProfit = findPathProfit(RIGHT, &numUnknownRight);
	
	if ( (numUnknownFront + numUnknownLeft + numUnknownRight) == 0)
	{
		Point closestUnknownPoint = closestUnknown();
		if (gridMap.contains(closestUnknownPoint))
		{
			if (!moveToPoint(closestUnknownPoint))
			{
				moveToPoint(Point(GRID_MAX_X, 0));
			}
		}
		else
		{
			moveToPoint( closestNonVisited() );
		}
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
	Point frontPoint = adjacentPoint(currPoint, facing, FRONT);
	Point pastFront = adjacentPoint(frontPoint, facing, FRONT);

	if ( !gridMap.contains(frontPoint) || gridMap.isFlagSet(frontPoint, OCCUPIED) )
	{
		startNextPath();
	}
	else if (!gridMap.contains(pastFront) || gridMap.isFlagSet(pastFront, SEEN))
	{
		Point leftDiag = adjacentPoint(frontPoint, facing, LEFT);
		Point rightDiag = adjacentPoint(frontPoint, facing, RIGHT);
		
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
	if (!haveBlock && gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
#else
if (gridMap.contains(frontPoint) && gridMap.isFlagSet(frontPoint, OCCUPIED))
#endif
	{
		#ifdef DEBUG
			printGrid();
		#endif
		haveBlock = obtainBlock(FRONT);
	}
	if (!haveBlock && gridMap.contains(rightPoint) && gridMap.isFlagSet(rightPoint, OCCUPIED))
	{
		#ifdef DEBUG
			printGrid();
		#endif
		haveBlock = obtainBlock(RIGHT);
		if (!haveBlock)		//Face original direction
		{
			mover->rotateDirection(LEFT, DEFAULT_SPEED);
			motors->stop();
			facing = findNewFacing(facing, LEFT);
		}
	}
	if (!haveBlock && gridMap.contains(leftPoint) && gridMap.isFlagSet(leftPoint, OCCUPIED))
	{
		#ifdef DEBUG
			printGrid();
		#endif
		haveBlock = obtainBlock(LEFT);
		if (!haveBlock)		//Face original direction
		{
			mover->rotateDirection(RIGHT, DEFAULT_SPEED);
			motors->stop();
			facing = findNewFacing(facing, RIGHT);
		}
	}
	if (!haveBlock)
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
	
	attempt++;
	currPoint = Point(GRID_MAX_X, 0);
	Point entrPoint = currPoint;
	haveBlock = false;
	facing = WEST;
	
	printGrid();
		
if (isColourMode)
{
	if ( (gridMap.getBlockCount() == NUMBER_OF_BLOCKS) && (coloursScanned >= 2) )
	{
		setLastColour();
	}
	switch (attempt)
	{
		case 2:
			if ( gridMap.contains(gridMap.getGreenPoint()) )
			{
				#ifdef DEBUG
					Serial.print("Attempting to grab previously seen Green at");
					(gridMap.getGreenPoint()).debug("");
					Serial.println();
				#endif
				grabBlockAtPoint( gridMap.getGreenPoint() );
			}
			else if (gridMap.getBlockCount() > coloursScanned)
			{
				#ifdef DEBUG
					Serial.println("Sensing colour of closest block.");
				#endif
				moveNextToBlock(closestBlock());
			}
			break;
		case 3:
			if ( gridMap.contains(gridMap.getBluePoint()) )
			{
				#ifdef DEBUG
					Serial.print("Attempting to grab previously seen Blue");
					(gridMap.getBluePoint()).debug("");
					Serial.println();
				#endif
				grabBlockAtPoint( gridMap.getBluePoint() );
			}
			else
			{
				grabBlockAtPoint(closestBlock());
			}
			break;
		default: break;
	}
}
else	//not colourMode
{
#ifndef SIMULATION
	Point closestOccupied = closestBlock();
	
	if (gridMap.contains(closestBlock()))
	{
		#ifdef DEBUG
			Serial.println("Obtaining closest block");
		#endif
		grabBlockAtPoint(closestBlock());
	}
#endif
}//end colourMode

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
	//Point tempPoint = Point(GRID_MAX_X + 1, 0);	//For facing the maze exit
	moveToPoint(entrPoint);
	claw->open();
	/*CarDir newDir = carDirOfPoint(tempPoint);		//For facing the maze exit
	mover->rotateDirection(dirCarToRel(newDir, facing), DEFAULT_SPEED);
	facing = newDir;
	#ifdef DEBUG
		Serial.print("Done.\tFacing = ");
		printCarDir(facing);
		Serial.println();
	#endif*/
}

//Use process of elimination if possible. NOTE: VERY poor mechanism.
void GridNav::setLastColour()
{
	char redID = -1, 
		greenID = -1,
		blueID = -1;
	
	//Find which BlockPoint ID corresponds to which colour
	for (int ii = 0; ii < NUMBER_OF_BLOCKS; ++ii)
	{
		if (gridMap.getRedPoint() == gridMap.getBlockPoint(ii))
		{
			redID = ii;
		}
	}
	for (int ii = 0; ii < NUMBER_OF_BLOCKS; ++ii)
	{
		if (gridMap.getGreenPoint() == gridMap.getBlockPoint(ii))
		{
			greenID = ii;
		}
	}
	for (int ii = 0; ii < NUMBER_OF_BLOCKS; ++ii)
	{
		if (gridMap.getBluePoint() == gridMap.getBlockPoint(ii))
		{
			blueID = ii;
		}
	}
	
	//(redID == -1) Should never occur
	if (greenID == -1)
	{
		greenID = 3 - (redID + blueID);
		#ifdef DEBUG
			(gridMap.getBlockPoint(greenID)).debug("Green point");
			Serial.println(" from elimination.");
		#endif
		gridMap.setGreenPoint( gridMap.getBlockPoint(greenID) );
	}
	else if (blueID == -1)
	{
		blueID = 3 - (redID + greenID);
		#ifdef DEBUG
			(gridMap.getBlockPoint(blueID)).debug("Blue point");
			Serial.println(" from elimination.");
		#endif
		gridMap.setBluePoint( gridMap.getBlockPoint(blueID) );
	}
}

void GridNav::moveNextToBlock(Point pt)
{
	if (gridMap.contains(pt))
	{
		router.generateRoute(currPoint, pt, facing, &path);
		for (int ii = 0; ii < (path.length - 1); ++ii)
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
		checkForBlocks();
	}
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

////// Block Avoiding ////////
void GridNav::mapPoints(Point nextPoint)
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
	#ifdef DEBUG
		printGrid();
	#endif
}

void GridNav::avoidBlocks()
{
	Path path;
	Point startPoint = Point(GRID_MAX_X, 0);
	currPoint = startPoint;
	Point goalPoint = Point(0, GRID_MAX_Y);
	facing = WEST;
	#ifdef DEBUG
		printGrid();
	#endif
	mapPoints(currPoint);
	while (currPoint != goalPoint)
	{
		router.generateRoute(currPoint, goalPoint, facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (gridMap.isFlagSet(path.path[ii], OCCUPIED))
			{
				#ifdef DEBUG
					Serial.println("Finished Path:");
					for (int ii = 0; ii < path.length; ++ii)
					{
						path.path[ii].debug("\tNode");
						Serial.println();
					}
					Serial.println("Re-routing.");
				#endif
				break;
			}
			else
			{
				moveToAdjPoint(path.path[ii]);
				mapPoints(path.path[ii]);
			}
		}
	}
	//motors->stop();
	mapPoints(currPoint);
	while (currPoint != startPoint)
	{
		router.generateRoute(currPoint, startPoint, facing, &path);
		for (int ii = 0; ii < path.length; ++ii)
		{
			if (gridMap.isFlagSet(path.path[ii], OCCUPIED))
			{
				#ifdef DEBUG
					Serial.println("Re-routing.");
				#endif
				break;
			}
			else
			{
				moveToAdjPoint(path.path[ii]);
				mapPoints(path.path[ii]);
			}
		}
	}
	motors->stop();
}

void GridNav::setColourMode(bool inIsColourMode)
{
	isColourMode = inIsColourMode;
}
