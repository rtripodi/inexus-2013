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

//Grid dimensions
#define GRID_MAX_X (7)
#define GRID_MAX_Y (7)

//Known locations
#define STARTING_X (GRID_MAX_X)
#define STARTING_Y (0)
#define ENTRANCE_X (0)
#define ENTRANCE_Y (0)
#define DROPZONE_X (GRID_MAX_X)
#define DROPZONE_Y (GRID_MAX_Y)

struct Point
{
	unsigned char x;
	unsigned char y;
	unsigned char flags;
};

//The status of point on the grid is bitmapped to 1 byte and contains the following flags:
//	Seen - The point has been seen
//	Visited - Have been to the point
//	Occupied - Holds a block
//	Red - Holds a block that is mostly red
//	Green - Holds a block that is mostly green
//	Blue - Holds a block that is mostly blue
//
//Order of flags: SVxxORGB, where x denotes currently unused bits.

//Most significant nibble mask bits
#define SEEN	(0x80)
#define VISITED	(0x40)
#define UNUSED1	(0x20)
#define UNUSED2	(0x10)

//Least significant nibble mask bits
#define OCCUPIED	(0x08)
#define RED			(0x04)
#define GREEN		(0x02)
#define BLUE		(0x01)

class GridMap//TODO: inherit Maze
{
	public:
		GridMap();
		
		//Change desired flag(s) to 1 for passed point
		void setFlag(Point *inPoint, unsigned char inFlag);
		
		//Change desired flag(s) to 0 for passed point 
		void removeFlag(Point *inPoint, unsigned char inFlag);
		
		//Returns true if the passed flag(s) is/are set
		bool isFlagSet(Point inPoint, unsigned char inFlag);
		
		//Returns true if point is in grid
		bool contains(Point point);
		
		//Returns true if there is a connection between pt1 and pt2, and are both inMaze()
		bool pointsJoined(Point pt1, Point pt2);
		
		//Returns true if point is not occupied
		bool isPassable(Point point);
		
	private:
		unsigned char grid[GRID_MAX_X + 1][GRID_MAX_Y + 1];
};

#endif
