#include "MazeMap.h"

MazeMap::MazeMap()
{
	mazeLength = 0;
	index = 0;
	mazeDirection = 1;
	needSimplify = false;
}

void MazeMap::updateMap(RelDir inRelDir, int inEastTicks, int inNorthTicks)
{
	if(needSimplify)
	{
		//TODO: I don't think the case of consecutive reversals is possible. Only put here as a safetynet for now
		if (inRelDir == BACK)
		{
			index -= 2;
			mazeLength -= 2;
			needSimplify = false;
		}	
		else
			simplify(inRelDir);
	}
	else
	{
		maze[index].turn = inRelDir;
		maze[index].eastTicks = inEastTicks;
		maze[index].northTicks = inNorthTicks;
	}
	if(maze[index].turn == BACK)
		needSimplify = true;
	index++;
	mazeLength++;
}

void MazeMap::simplify(RelDir inRelDir)
{
	index -= 2;
	mazeLength -= 2;

	//Left, Right, Forward, Back(reverse)
	//L = RBF = FBR, R = LBF = FBL, F = LBL = RBR, B = LBR = RBL = FBF
	if ( (maze[index].turn == RIGHT &&  inRelDir == FRONT) || (maze[index].turn == FRONT &&  inRelDir == RIGHT) )
	{
		maze[index].turn = LEFT;
		needSimplify = false;
	}
	else if ( (maze[index].turn == LEFT &&  inRelDir == FRONT) || (maze[index].turn == FRONT &&  inRelDir == LEFT) )
	{
		maze[index].turn = RIGHT;
		needSimplify = false;
	}
	else if ( (maze[index].turn == LEFT &&  inRelDir == LEFT) || (maze[index].turn == RIGHT &&  inRelDir == RIGHT) )
	{
		maze[index].turn = FRONT;
		needSimplify = false;
	}
	else if ( (maze[index].turn == LEFT &&  inRelDir == RIGHT) || (maze[index].turn == RIGHT &&  inRelDir == LEFT) || (maze[index].turn == FRONT &&  inRelDir == FRONT) )
	{
		maze[index].turn = BACK;
		needSimplify = true;
	}
}

RelDir MazeMap::getNext()
{
	RelDir temp = maze[index].turn;
	index += mazeDirection;
	if (mazeDirection == 1 || temp == FRONT)
		return temp;
	else
	{
		if (temp == RIGHT)
			return LEFT;
		else
			return RIGHT;
	}
}

void MazeMap::resetToStart()
{
	mazeDirection = 1;
	index = 0;
}

void MazeMap::resetToEnd()
{
	mazeDirection = -1;
	index = (mazeLength - 1);
}

bool MazeMap::finalTurn()
{
	if (mazeDirection == 1)
		return (index >= mazeLength);
	else
		return (index < 0);
}

void MazeMap::printMapTurns()
{
	Serial.print((char)maze[0].turn);
	for (int ii = 1; ii < mazeLength; ++ii)
	{
		Serial.print(", ");
		Serial.print((char)maze[ii].turn);
	}
}

void MazeMap::printMapLog()
{
	for (int ii = 0; ii < mazeLength; ++ii)
	{
		Serial.print((char)maze[ii].turn);
		Serial.print("\teastTicks: ");
		Serial.print(maze[ii].eastTicks);
		Serial.print("\tnorthTicks: ");
		Serial.println(maze[ii].northTicks);
	}
}
