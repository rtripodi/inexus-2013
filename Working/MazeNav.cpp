#include "MazeNav.h"

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
		return (irInMm.frnt < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == RIGHT)
		return (irInMm.rght < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == BACK)
		return (irInMm.bck < WALL_MIN + WALL_TOLERANCE);
	else if (relDir == LEFT)
		return (irInMm.lft < WALL_MIN + WALL_TOLERANCE);
}

//Updates the IR values
void MazeNav::scanWalls()
{
	irInMm.frnt = ((*irs).frnt)->getDist();
	irInMm.rght = ((*irs).rght)->getDist();
	irInMm.bck = ((*irs).bck)->getDist();
	irInMm.lft = ((*irs).lft)->getDist();
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
		mover->moveLength(170, DEFAULT_SPEED);
		motors->stop();
		mover->rotateDirection(LEFT, DEFAULT_SPEED);
	}
}

void MazeNav::searchNextMaze()
{
	mover->moveLength(170, DEFAULT_SPEED);
	motors->stop();
	scanWalls();
	while (!isWall(FRONT) && isWall(RIGHT) && isWall(LEFT))
	{
		mover->moveForward(DEFAULT_SPEED);
	}
	if (!isWall(FRONT))
	{
		mover->moveLength(170, DEFAULT_SPEED);
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
