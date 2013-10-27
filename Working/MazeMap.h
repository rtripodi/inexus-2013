#ifndef MazeMap_h
#define MazeMap_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Direction.h"
#include "Config.h"

class MazeMap
{
	public:
		
		//Constructs empty mazeMap/'turn list'
		MazeMap();
		
		//Adds inputted relative direction to the turns list 
		void updateMap(RelDir inRelDir, int inEastTicks, int inNorthTicks);
		
		//Returns the next relative direction to turn 
		RelDir getNext();
		
		//Resets the index to point to the last turn to traverse from exit to entrance
		void resetToStart();
		
		//Resets the index to point to the first turn to traverse from entrance to entrance
		void resetToEnd();
		
		//Returns true if the map is pointing at the last turn of the maze
		bool finalTurn();
		
		//Prints the turns in the maze map
		void printMapTurns();
		
		//Prints the turns and ticks in the maze map
		void printMapLog();
		
	private:
		
		typedef struct {
			RelDir turn;
			int eastTicks;
			int northTicks;
		} TurnData;
		
		TurnData maze[MAZE_MAX];
		
		//The shortest amount of turns taken to traverse the maze
		unsigned char mazeLength;
		
		//Points to next element to be added and then to read from MazeMap once created
		int index;
		
		//Represents the relative traversal direction
		//Forward is positive when going forward through the maze, negative on it's way back to the start
		char mazeDirection;
		
		//Used to flag that a reverse has occurred and must eventually be simplified
		bool needSimplify;
		
		//Removes reverses from the maze map, simplifying 3 directions into 1
		void simplify(RelDir inRelDir);
};

#endif
