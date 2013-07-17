#ifndef MazeMap_h
#define MazeMap_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define MAZE_MAX 256

class MazeMap
{
	public:
        enum Direction{left, right, forward, reverse};
        MazeMap();
        void updateMap(Direction inDirection);
        Direction getNext();
        void resetToStart();
        void resetToEnd();

		
	private:
        Direction maze[MAZE_MAX];
        unsigned char mazeLength;
        unsigned char index;
        unsigned char simplify();
        //forward is positive when going through the maze the first time, negative on it's way back to the start
        unsigned char mazeDirection;
};

#endif
