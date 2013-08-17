#include "Config.h"
#include "Claw.h"
#include "IR.h"
#include "LineSensors.h"
#include "Movement.h"
#include "Motor.h"
#include "MazeImports.h"
#include "GridMap.h"
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

#define TURN_FRONT 0
#define TURN_RIGHT (90 - 20) //Tweaking given speed of 80
#define TURN_BACK 180
#define TURN_LEFT (-90 + 20)

//F=0, R=1, B=2, L=3
//enum RelativeDir {RelFront, RelRight, RelBack, RelLeft};
#define REL_FRONT 0
#define REL_RIGHT 1
#define REL_BACK 2
#define REL_LEFT 3

#define DIR_NORTH 1
#define DIR_EAST 2
#define DIR_SOUTH 3
#define DIR_WEST 4

class GridNav
{
	public:
		Motor *motors;
		Movement *mover;
		IR **irs;
		Claw *claw;

		GridNav(Motor *inMotor, Movement *inMovement, IR *irs[4], Claw *inClaw);
	
		//Returns the value is degrees needed to excute a turn in the given relative direction
		int findAngle(unsigned char relDir);
		
		//Returns the new facing cardinal direction given a turn in the relative direction
		//Unexpected return for inputs other then: Front, Right, Back, Left
		unsigned char findNewFacing(unsigned char inFacing, unsigned char relativeTurn);
		
		//Returns the closest point in the inputted relative direction
		//Unexpected return for a relative direction causing a point off the grid
		Point adjacentPoint(Point pt, unsigned char inFacing, unsigned char relativeTurn);
		
		Point frontDiagPoint(Point pt, unsigned char inFacing, unsigned char relativeTurn);
		
		//Checks whether there is a block within a specified range of the inputted relative direction
		//Returns false by default for a relative direction causing a point off the grid
		bool isBlock(unsigned char dir);
		
		//TODO: Return point moved to, then currPoint will be made equal to this 
		void moveToFrontPoint();
		
		//Sets the OCCUPIED flag for the point in the inputted relative direction and attempts to grab the block
		//If the grab is unsuccessful, the robot will return to the point at which it sensed it
		bool obtainBlock(unsigned char relDir);
		
		//Returns the cardinal direction of a point given the inputted relative direction
		unsigned char dirOfPoint(Point pt);
		
		//Turns the robot in the desired cardinal direction
		//Unexpected results for inputs other then: North, East, South, West
		unsigned char dirCardToRel(unsigned char newCardDir);
		
		//Moves to given point
		void moveToPoint(Point pt);
		
		//Travels the current least expensive route to the entrance
		void returnToEntrance();
		
		//Scan front point and set seen flag
		void scanFrontIr();
		
		//Scan right point and set seen flag
		void scanRightIr();
		
		//Scan left point and set seen flag
		void scanLeftIr();
		
		int findPathProfit(unsigned char relDir, unsigned char *numUnknown);
		
		void chooseNextPath();
		
		//Checks for blocks on adjacent points
		void checkForBlocks();
		
		void initCheckForBlocks();
		
		void findBlock();
		
		void printGrid();
		
		void printDirection(unsigned char dir);
		
		void printRelDirection(unsigned char dir);
		
	private:
		GridMap gridMap;
		Routing router;

		//Current point on grid
		Point currPoint;
		
		//Current facing cardinal direction
		unsigned char facing;

		
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

		//TODO: Change type back to Direction
		
};
