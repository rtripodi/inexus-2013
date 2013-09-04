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

//Min wall width = 350mm
//From centre = 175mm
//IR Offset = ~7mm?

#define WALL_MIN (175)
#define WALL_TOLERANCE (25)

class MazeNav
{
	public:
		Motor *motors;
		Movement *mover;
		IR **irs;
		
		MazeNav(Motor *inMotor, Movement *inMovement, IR *irs[4]);
		
		void firstNavigate();
	
	private:
		MazeMap mazeMap;
		
		struct irValues
		{
			int frnt;
			int rght;
			int bck;
			int lft;
		};
		
		//Stores distance in mm read from each IR sensor
		irValues irInMm;
		
		bool isWall(RelDir relDir);
		
		void scanWalls();

		void searchInitEntrance();

		void searchNextMaze();

		RelDir alwaysLeft();
};

#endif
