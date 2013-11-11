#include "MazeNav.h"

//#define SIMULATION
#define DEBUG

MazeNav::MazeNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	mazeMap = MazeMap();
}

//Navigate and map maze from starting point
void MazeNav::firstNavigate()
{
	RelDir turn;
	facing = WEST;
	
	eastTicks = MAX_EAST_TICKS;
	northTicks = 0;
	
	searchInitEntrance();
	while (1) //TODO: Need exit condition
	{
		while (!nextPosition());
#ifdef SIMULATION
if (irInMm.frnt <= 0)
	break;
#endif
		turn = alwaysLeft();
#ifdef DEBUG
Serial.print("Turn: ");
Serial.println((char)turn);
#endif
		facing = findNewFacing(facing, turn);
		updateTicks();
		mazeMap.updateMap(turn, eastTicks, northTicks);
#ifndef SIMULATION
		mover->rotateDirection(turn, DEFAULT_SPEED);
#endif
	}
#ifdef DEBUG
Serial.println("MazeMap: ");
mazeMap.printMapTurns();
Serial.println();
#endif
}

//Returns true if a wall is in given relative direction based on IR values
bool MazeNav::isWall(RelDir relDir)
{
	if (relDir == FRONT)
		return (irInMm.frnt < (WALL_MIN_HALF + WALL_TOLERANCE));
	else if (relDir == BACK)
		return (irInMm.bck < (WALL_MIN_HALF + WALL_TOLERANCE));
	else if ((irInMm.rght + irInMm.lft) > (WALL_MIN_FULL - WALL_TOLERANCE))
	{
		if (relDir == RIGHT)
			return (irInMm.rght < (WALL_MIN_HALF + WALL_TOLERANCE));
		else if (relDir == LEFT)
			return (irInMm.lft < (WALL_MIN_HALF + WALL_TOLERANCE));		
	}
}

//Updates the IR values
void MazeNav::scanWalls()
{
#ifndef SIMULATION
	irInMm.frnt = (irs->frnt)->getDist();
	irInMm.rght = (irs->rght)->getDist();
	irInMm.bck = (irs->bck)->getDist();
	irInMm.lft = (irs->lft)->getDist();
#else
char buffer[12] = {' '};	//3 readings * 3 digits + 2 spaces + 1 (Serial goes out of bounds otherwise)
for(int ii = 0; ii < 11; ++ii)
{
	while (Serial.available() == 0);  //Wait for input
	buffer[ii] = Serial.read();
}
sscanf (buffer, "%d %d %d", &irInMm.bck, &irInMm.rght, &irInMm.lft);
irInMm.frnt = 500;
#endif
#ifdef DEBUG
	Serial.print("\t\t\t\tIRs - B: ");
	Serial.print(irInMm.bck);
	Serial.print("\tR: ");
	Serial.print(irInMm.rght);
	Serial.print("\tL: ");
	Serial.println(irInMm.lft);
#endif
}

//From starting point, search for entrance
void MazeNav::searchInitEntrance()
{
	scanWalls();
	if (isWall(FRONT))
	{
		#ifdef DEBUG
			Serial.println("Front wall found.");
		#endif
#ifndef SIMULATION
		mover->rotateDirection(RIGHT, DEFAULT_SPEED);
#endif
		facing = NORTH;
		scanWalls();
		while (isWall(LEFT))
		{
			#ifdef DEBUG
				Serial.println("Searching for entrance.");
			#endif
#ifndef SIMULATION
			motors->both(DEFAULT_SPEED, mover->tickError());
#endif
			scanWalls();
		}
		#ifdef DEBUG
			Serial.println("Found entrance.");
		#endif
#ifndef SIMULATION
		mover->moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
		motors->stop();
		updateTicks();
		mazeMap.updateMap(LEFT, eastTicks, northTicks);
		mover->rotateDirection(LEFT, DEFAULT_SPEED);
		facing = WEST;
#endif
	}
	else
	{
		updateTicks();
		mazeMap.updateMap(LEFT, eastTicks, northTicks);
		#ifdef DEBUG
			Serial.println("Found entrance.");
		#endif
	}
#ifndef SIMULATION
	mover->moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
	motors->stop();
#endif
	#ifdef DEBUG
		Serial.println("Entered maze");
	#endif
}

bool MazeNav::nextPosition()
{
	scanWalls();
	while (!isWall(FRONT) && isWall(RIGHT) && isWall(LEFT))
	{
#ifdef DEBUG
Serial.println("Hallway");
#endif
#ifndef SIMULATION
		motors->both(DEFAULT_SPEED, mover->tickError());
#endif
		scanWalls();
	}
#ifndef SIMULATION
	if (!isWall(FRONT))
	{
#ifdef DEBUG
Serial.println("Wall infront");
#endif
		mover->moveLength(WALL_MIN_FULL, DEFAULT_SPEED);
		motors->stop();
		//TODO: Check previous scan with current to ensure no change
		return true;
	}
	return true;
#else
	return true;
#endif
}

void MazeNav::followMap(RelDir navDir)
{
	RelDir turn;
	
	if (navDir == FRONT)
		mazeMap.resetToStart();
	else
		mazeMap.resetToEnd();
	while (!mazeMap.finalTurn())
	{
#ifndef SIMULATION	
		while (!nextPosition());
#endif
		turn = mazeMap.getNext();
#ifdef DEBUG
Serial.print("Turn: ");
Serial.println((char)turn);
#endif
#ifndef SIMULATION
		mover->rotateDirection(turn, DEFAULT_SPEED);
#endif
	}
}

void MazeNav::updateTicks()
{
	switch (facing)
	{
		case NORTH:
			northTicks += mover->getTicks();
			break;
		case EAST:
			eastTicks += mover->getTicks();
			break;
		case SOUTH:
			northTicks -= mover->getTicks();
			break;
		case WEST:
			eastTicks -= mover->getTicks();
			break;
	}
}

//Separated to allow easier change to another turning scheme
RelDir MazeNav::alwaysLeft()
{
	if (!isWall(LEFT))
	{
		return LEFT;
	}
	else if (!isWall(FRONT))
	{ 
		return FRONT;
	}
	else if (!isWall(RIGHT))
	{	 
		return RIGHT;
	}
	else
	{
		return BACK;
	}
}

void MazeNav::debugIrs()
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
