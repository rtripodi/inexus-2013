#ifndef GridMap_h
#define GridMap_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"

//Grid dimensions
#define GRID_LENGTH (8)
#define GRID_WIDTH (8)

//Known locations
#define STARTING_X (0)
#define STARTING_Y (0)
#define ENTRANCE_X (0)
#define ENTRANCE_Y (GRID_LENGTH - 1)
#define DROPZONE_X (GRID_WIDTH - 1)
#define DROPZONE_Y (0)

struct Point
{
	int x;
	int y;
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

class GridMap
{
	public:
		GridMap();
		
		void setFlag(Point *inPoint, unsigned char inFlag);
		
		bool isFlagSet(Point inPoint, unsigned char inFlag);
		
	private:
		Point grid[GRID_LENGTH][GRID_WIDTH];	//	Placeholder, array index stored inside struct is not space efficent
};

#endif
