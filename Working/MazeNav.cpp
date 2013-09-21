#include "MazeNav.h"

MazeNav::MazeNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs)
{
	motors = inMotor;
	mover = inMovement;
	irs = inIrs;
	mazeMap = MazeMap();
	
	Serial.print("Class Container Ref: ");
	Serial.println((int)irs);
	
	Serial.print("Class Contained Ref - ");
	Serial.print("\tF: ");
	Serial.print((int)(irs->frnt));
	Serial.print("\tR: ");
	Serial.print((int)(irs->rght));
	Serial.print("\tB: ");
	Serial.print((int)(irs->bck));
	Serial.print("\tL: ");
	Serial.println((int)(irs->lft));
}

//Navigate and map maze from starting point
void MazeNav::firstNavigate()
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
bool MazeNav::isWall(RelDir relDir)
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
void MazeNav::scanWalls()
{
	irInMm.frnt = (irs->frnt)->getDist();
	irInMm.rght = (irs->rght)->getDist();
	irInMm.bck = (irs->bck)->getDist();
	irInMm.lft = (irs->lft)->getDist();
}

//From starting point, search for entrance
void MazeNav::searchInitEntrance()
{
	scanWalls();
	if (isWall(FRONT))
	{
		mover->rotateDirection(RIGHT, DEFAULT_SPEED);
		while (isWall(LEFT))
		{
			mover->moveForward(DEFAULT_SPEED);
			scanWalls();
		}
		mover->moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
		motors->stop();
		mover->rotateDirection(LEFT, DEFAULT_SPEED);
	}
}

void MazeNav::searchNextMaze()
{
	mover->moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
	motors->stop();
	scanWalls();
	while (!isWall(FRONT) && isWall(RIGHT) && isWall(LEFT))
	{
		mover->moveForward(DEFAULT_SPEED);
	}
	if (!isWall(FRONT))
	{
		mover->moveLength(WALL_MIN_HALF, DEFAULT_SPEED);
		motors->stop();
	}
}

//Separated to allow easier change to another turning scheme
RelDir MazeNav::alwaysLeft()
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
