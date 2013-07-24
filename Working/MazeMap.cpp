#include "MazeMap.h"

MazeMap::MazeMap()
{
    mazeLength = 0;
    index = 0;
    mazeDirection = 1;
	needSimplify = false;
}

void MazeMap::updateMap(Direction inDirection)
{
    if(needSimplify)
	{
		//TODO: I don't think the case of consecutive reversals is possible. Only put here as a safetynet for now
		if (inDirection == reverse)
		{
			index -= 2;
			mazeLength -= 2;
			needSimplify = false;
		}	
		else
			simplify(inDirection);
	}
	else
		maze[index] = inDirection;
	if(maze[index] == reverse)
		needSimplify = true;
    index++;
	mazeLength++;
}

void MazeMap::simplify(Direction inDirection)
{
	index -= 2;
	mazeLength -= 2;

	//Left, Right, Forward, Back(reverse)
	//L = RBF = FBR, R = LBF = FBL, F = LBL = RBR, B = LBR = RBL = FBF
	if ( (maze[index] == right &&  inDirection == forward) || (maze[index] == forward &&  inDirection == right) )
	{
		maze[index] = left;
		needSimplify = false;
	}
	else if ( (maze[index] == left &&  inDirection == forward) || (maze[index] == forward &&  inDirection == left) )
	{
		maze[index] = right;
		needSimplify = false;
	}
	else if ( (maze[index] == left &&  inDirection == left) || (maze[index] == right &&  inDirection == right) )
	{
		maze[index] = forward;
		needSimplify = false;
	}
	else if ( (maze[index] == left &&  inDirection == right) || (maze[index] == right &&  inDirection == left) || (maze[index] == forward &&  inDirection == forward) )
	{
		maze[index] = reverse;
		needSimplify = true;
	}
}

MazeMap::Direction MazeMap::getNext()
{
    Direction temp = maze[index];
	index += mazeDirection;
    return temp;
}

void MazeMap::resetToStart()
{
    mazeDirection = 1;
	index = 0;
}
void MazeMap::resetToEnd()
{
    mazeDirection = -1;
	index = mazeLength;
}
