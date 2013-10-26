#ifndef GridNav_h
#define GridNav_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "Claw.h"
#include "IR.h"
#include "Movement.h"
#include "Routing.h"
#include "Navigation.h"

class GridNav
{
	public:
		Motor *motors;
		Movement *mover;
		IrSensors *irs;
		Claw *claw;
		
		GridNav(Motor *inMotor, Movement *inMovement, IrSensors *inIrs, Claw *inClaw);
		
		void findBlock();
		
		void debugIrs();
		
	private:
		GridMap gridMap;
		Routing router;
		Path path;
		
		//Current point on grid
		Point currPoint;
		
		//Current facing cardinal direction
		CarDir facing;
		
		//Stores distance in mm read from each IR sensor
		IrValues irInMm;
		
		//Determines whether an error occurred with grabbing/holding the block
		bool haveBlock;
		
		//Returns the closest point in the inputted relative direction
		//Unexpected return for a relative direction causing a point off the grid
		Point adjacentPoint(Point pt, CarDir inFacing, RelDir relativeTurn);
		
		Point frontDiagPoint(Point pt, CarDir inFacing, RelDir relativeTurn);
		
		//Checks whether there is a block within a specified range of the inputted relative direction
		//Returns false by default for a relative direction causing a point off the grid
		bool isBlock(RelDir relDir);
		
		//TODO: Return point moved to, then currPoint will be made equal to this 
		void moveToFrontPoint();
		
		//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
		//If the grab is unsuccessful, the robot will return to the point at which it sensed it
		bool obtainBlock(RelDir relDir);
		
		//Returns the cardinal direction of a point given the inputted relative direction
		CarDir carDirOfPoint(Point pt);
		
		//Moves to given point
		void moveToAdjPoint(Point pt);
		
		//Travels the current least expensive route to specified point
		void moveToPoint(Point pt);
		
		//Scan front point and set seen flag
		void mapFrontPoint();
		
		//Scan right point and set seen flag
		void mapRightPoint();
		
		//Scan left point and set seen flag
		void mapLeftPoint();
		
		int findPathProfit(RelDir relDir, unsigned char *numUnknown);
		
		Point closestUnknown();
		
		void startNextPath();
		
		void chooseNextPath();
		
		//Checks for blocks on adjacent points
		void checkForBlocks();
		
		void printGrid();
		
		void printCarDir(CarDir carDir);
		
		void printRelDir(RelDir relDir);
		
		Point closestBlock();
				
		void moveToBlock(Point pt);
};

#endif
