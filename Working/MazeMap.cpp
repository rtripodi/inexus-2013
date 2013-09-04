#include "MazeMap.h"

MazeMap::MazeMap()
{
    mazeLength = 0;
    index = 0;
    mazeDirection = 1;
	needSimplify = false;
}

void MazeMap::updateMap(RelDir inRelDir)
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
		maze[index] = inRelDir;
	if(maze[index] == BACK)
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
	if ( (maze[index] == RIGHT &&  inRelDir == FRONT) || (maze[index] == FRONT &&  inRelDir == RIGHT) )
	{
		maze[index] = LEFT;
		needSimplify = false;
	}
	else if ( (maze[index] == LEFT &&  inRelDir == FRONT) || (maze[index] == FRONT &&  inRelDir == LEFT) )
	{
		maze[index] = RIGHT;
		needSimplify = false;
	}
	else if ( (maze[index] == LEFT &&  inRelDir == LEFT) || (maze[index] == RIGHT &&  inRelDir == RIGHT) )
	{
		maze[index] = FRONT;
		needSimplify = false;
	}
	else if ( (maze[index] == LEFT &&  inRelDir == RIGHT) || (maze[index] == RIGHT &&  inRelDir == LEFT) || (maze[index] == FRONT &&  inRelDir == FRONT) )
	{
		maze[index] = BACK;
		needSimplify = true;
	}
}

RelDir MazeMap::getNext()
{
    RelDir temp = maze[index];
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
