#ifndef MazeNav_h
#define MazeNav_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MazeMap.h"
#include "IR.h"
#include "Movement.h"
#include "Navigation.h"

/*	|       |
	|       '-------.
	|               |
	|       |<----->| = 350mm wall minimum (full)
	|               |
	|       .-------'
	|       |
	
	|       |
	|       |
	|   |<->| = ~175mm wall minimum (half)
	|       |
	|       |
	
	|       |
	|       '-------.
	|               |
	|   |<--------->| ~175mm + 350mm = ~525mm mmax wall
	|               |
	|       .-------'
	|       |
*/

#define WALL_MIN_FULL (350)
#define WALL_MIN_HALF (WALL_MIN_FULL / 2)
#define WALL_TOLERANCE (15)

class MazeNav
{
	public:
		Motor *motors;
		Movement *mover;
		IrSensors *irs;
		
		MazeNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs);
		
		void firstNavigate();
		
		void debugIrs();
	
	private:
		MazeMap mazeMap;
		
		//Stores distance in mm read from each IR sensor
		IrValues irInMm;
		
		bool isWall(RelDir relDir);
		
		void scanWalls();

		void searchInitEntrance();

		void searchNextMaze();

		RelDir alwaysLeft();
};

#endif
