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

class MazeNav
{
	public:
		Motor *motors;
		Movement *mover;
		IrSensors *irs;
		
		MazeNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs);
		
		void firstNavigate();
		
		void followMap(RelDir navDir);
		
		void debugIrs();
	
	private:
		MazeMap mazeMap;
		
		//Stores distance in mm read from each IR sensor
		IrValues irInMm;
		
		CarDir facing;
		
		int eastTicks, northTicks;
		
		bool isWall(RelDir relDir);
		
		void scanWalls();

		void searchInitEntrance();

		bool nextPosition();
		
		void updateTicks();

		RelDir alwaysLeft();
};

#endif
