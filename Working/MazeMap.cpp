#include "MazeMap.h"


MazeMap::MazeMap()
{
    mazeLength=0;
    index=0;
    mazeDirection=1;
}

void MazeMap::updateMap(Direction inDirection)
{
    maze[index]=inDirection;
    index++;
}

MazeMap::Direction MazeMap::getNext()
{
    index+=mazeDirection;
    return(maze[index-mazeDirection]);
}
        
unsigned char MazeMap::simplify()
{
}

void MazeMap::resetToStart()
{
    mazeDirection = 1;
}
void MazeMap::resetToEnd()
{
    mazeDirection = -1;
}
