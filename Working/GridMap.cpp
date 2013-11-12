#include "GridMap.h"

GridMap::GridMap()
{
	for (unsigned char x = 0; x <= GRID_MAX_X; ++x)
	{
		for (unsigned char y = 0; y <= GRID_MAX_Y; ++y)
		{
			status[x][y] = 0x00;
		}
	}
	redPoint = Point(-1, -1);
	greenPoint = Point(-1, -1);
	bluePoint = Point(-1, -1);
}

//Change desired flag(s) to 1 for passed point
void GridMap::setFlag(Point pt, unsigned char inFlag)
{
	status[pt.x][pt.y] |= inFlag;
}

//Change desired flag(s) to 0 for passed point 
void GridMap::removeFlag(Point pt, unsigned char inFlag)
{
	status[pt.x][pt.y] &= ~inFlag;
}

//Returns true if the passed flag(s) is/are set
bool GridMap::isFlagSet(Point pt, unsigned char inFlag)
{
	return status[pt.x][pt.y] & inFlag;
}

//Returns true if point is in grid
bool GridMap::contains(Point point)
{
	return (point.x >= 0) && (point.x <= GRID_MAX_X) && (point.y >= 0) && (point.y <= GRID_MAX_Y);
}

//Returns true if there is a connection between pt1 and pt2, and are both inMaze()
bool GridMap::joined(Point pt1, Point pt2)
{
	unsigned char del = abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y);
	return (contains(pt1) && contains(pt2) && del == 1);
}

//Returns true if point is not occupied
bool GridMap::isPassable(Point point)
{
	return !(isFlagSet(point, OCCUPIED));
}

//DEBUG: Returns flags for give point
unsigned char GridMap::getFlags(Point point)
{
	return status[point.x][point.y];
}

Point GridMap::getRedPoint()
{
	return redPoint;
}

Point GridMap::getBluePoint()
{
	return bluePoint;
}

Point GridMap::getGreenPoint()
{
	return greenPoint;
}

void GridMap::setRedPoint(Point inPoint)
{
	redPoint = inPoint;
}

void GridMap::setBluePoint(Point inPoint)
{
	bluePoint = inPoint;
}

void GridMap::setGreenPoint(Point inPoint)
{
	greenPoint = inPoint;
}

char GridMap::getFlagsAsChar(Point point)
{
	if (isFlagSet(point, RED))
		return 'R';
	else if (isFlagSet(point, GREEN))
		return 'G';
	else if (isFlagSet(point, BLUE))
		return 'B';
	else if (isFlagSet(point, OCCUPIED))
		return 'O';
	else if (isFlagSet(point, VISITED))
		return 'V';
	else if (isFlagSet(point, SEEN))
		return 'S';
	else
		return ' ';
}