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

#define CENTRE_DIST (300)
#define BLOCK_ACT_DIST (250)
#define BLOCK_DIST_F (300)
#define BLOCK_DIST_L (250)
#define BLOCK_DIST_R (250)

#define BLOCK_TOLERANCE (100)
#define BLOCK_STOP (60)

//Front IR senses 275-300mm for block
//Left IR senses 275-300mm for block
//Right IR senses 250-275mm for block

class GridNav
{
	public:
		Motor *motors;
		Movement *mover;
		IR **irs;
		Claw *claw;

		GridNav(Motor *inMotor, Movement *inMovement, IR *irs[4], Claw *inClaw);
		
		void findBlock();

		
	private:
		GridMap gridMap;
		Routing router;

		//Current point on grid
		Point currPoint;
		
		//Current facing cardinal direction
		CarDir facing;

		
		struct irValues
		{
			int frnt;
			int rght;
			int bck;
			int lft;
		};
		
		//Stores distance in mm read from each IR sensor
		irValues irInMm;
				
		//Determines whether an error occured with grabbing/holding the block
		bool haveBlock;
		bool grabSuccess;

		//Returns the new facing cardinal direction given a turn in the relative direction
		//Unexpected return for inputs other then: Front, Right, Back, Left
		CarDir findNewFacing(CarDir inFacing, RelDir relativeTurn);
		
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
		
		//Turns the robot in the desired cardinal direction
		//Unexpected results for inputs other then: North, East, South, West
		RelDir dirCarToRel(CarDir newCardDir);
		
		//Moves to given point
		void moveToAdjPoint(Point pt);
		
		//Travels the current least expensive route to specified point
		void moveToPoint(Point pt);
		
		//Scan front point and set seen flag
		void scanFrontIr();
		
		//Scan right point and set seen flag
		void scanRightIr();
		
		//Scan left point and set seen flag
		void scanLeftIr();
		
		int findPathProfit(RelDir relDir, unsigned char *numUnknown);
		
		Point closestUnknown();
		
		void chooseNextPath();
		
		//Checks for blocks on adjacent points
		void checkForBlocks();
		
		void initCheckForBlocks();		
		
		void printGrid();
		
		void printCarDir(CarDir carDir);
		
		void printRelDir(RelDir relDir);
};

#endif
