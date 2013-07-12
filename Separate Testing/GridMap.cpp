#include "GridMap.h"

GridMap::GridMap()
{
	//Create map data structure
}

void GridMap::setFlag(Point *inPoint, unsigned char inFlag)
{
	(*inPoint).flags |= inFlag;
}

bool GridMap::isFlagSet(Point inPoint, unsigned char inFlag)
{
	return ((inPoint.flags & inFlag) ? true : false);
}
