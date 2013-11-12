/*
	Grid directions:
	       y          W
	  .---->          ^
	  |           S <   > N
	  |               v 
	x v               E
*/

#ifndef GridMap_h
#define GridMap_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "GridImports.h"

class GridMap : public Maze
{
	public:
		GridMap();
		
		//Change desired flag(s) to 1 for passed point
		void setFlag(Point pt, unsigned char inFlag);
		
		//Change desired flag(s) to 0 for passed point 
		void removeFlag(Point pt, unsigned char inFlag);
		
		//Returns true if the passed flag(s) is/are set
		bool isFlagSet(Point pt, unsigned char inFlag);
		
		//Returns true if point is in grid
		bool contains(Point point);
		
		//Returns true if there is a connection between pt1 and pt2, and are both inMaze()
		bool joined(Point pt1, Point pt2);
		
		//Returns true if point is not occupied
		bool isPassable(Point point);
		
		//DEBUG: Returns flags for given point
		unsigned char getFlags(Point point);
		
		//DEBUG: Returns flags for given point as char for printing
		char getFlagsAsChar(Point point);
		
		Point getRedPoint();
		Point getGreenPoint();
		Point getBluePoint();
		
		void setRedPoint(Point inPoint);
		void setGreenPoint(Point inPoint);
		void setBluePoint(Point inPoint);
		
	private:
		unsigned char status[GRID_MAX_X + 1][GRID_MAX_Y + 1];
		
		Point redPoint, greenPoint, bluePoint;
};

#endif
