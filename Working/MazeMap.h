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
        //forward is positive when going through the maze the first time, negative on it's way back to the start
        unsigned char mazeDirection;
		
		//Used to flag that a reverse has occurred and must eventually be simplified
		bool needSimplify;
		
		//Removes reverses from the maze map, simplifying 3 directions into 1
		void simplify(Direction inDirection);
};

#endif
