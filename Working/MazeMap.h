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
		
		//Constructs empty mazeMap/'turn list'
        MazeMap();
		
		//Adds inputted direction to the turns list 
		void updateMap(Direction inDirection);
		
		//Returns the next direction to turn 
        Direction getNext();
		
		//Resets the index to point to the last turn to traverse from exit to entrance
        void resetToStart();
		
		//Resets the index to point to the first turn to traverse from entrance to entrance
        void resetToEnd();
		
	private:
		
        Direction maze[MAZE_MAX];
		
		//The shortest amount of turns taken to traverse the maze
        unsigned char mazeLength;
		
		//Points to next element to be added and then to read from MazeMap once created
        unsigned char index;
		
        //Represents the traversal direction
		//Forward is positive when going forward through the maze, negative on it's way back to the start
        unsigned char mazeDirection;
		
		//Used to flag that a reverse has occurred and must eventually be simplified
		bool needSimplify;
		
		//Removes reverses from the maze map, simplifying 3 directions into 1
		void simplify(Direction inDirection);
};

#endif
