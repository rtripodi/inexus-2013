#ifndef Navigation_h
#define Navigation_h

typedef struct {
	int frnt;
	int rght;
	int bck;
	int lft;
} IrValues;

typedef struct {
	IR *frnt;
	IR *rght;
	IR *bck;
	IR *lft;
} IrSensors;

//Returns the new facing cardinal direction given a turn in the relative direction
//Unexpected return for inputs other then: Front, Right, Back, Left
inline CarDir findNewFacing(CarDir inFacing, RelDir relativeTurn)
{
	if ((inFacing == NORTH && relativeTurn == FRONT) || (inFacing == EAST && relativeTurn == LEFT) || (inFacing == SOUTH && relativeTurn == BACK) || (inFacing == WEST && relativeTurn == RIGHT))
		return NORTH;
	else if ((inFacing == NORTH && relativeTurn == RIGHT) || (inFacing == EAST && relativeTurn == FRONT) || (inFacing == SOUTH && relativeTurn == LEFT) || (inFacing == WEST && relativeTurn == BACK))
		return EAST;
	else if ((inFacing == NORTH && relativeTurn == BACK) || (inFacing == EAST && relativeTurn == RIGHT) || (inFacing == SOUTH && relativeTurn == FRONT) || (inFacing == WEST && relativeTurn == LEFT))
		return SOUTH;
	else if ((inFacing == NORTH && relativeTurn == LEFT) || (inFacing == EAST && relativeTurn == BACK) || (inFacing == SOUTH && relativeTurn == RIGHT) || (inFacing == WEST && relativeTurn == FRONT))
		return WEST;
}

//Turns the robot in the desired cardinal direction
//Unexpected results for inputs other then: North, East, South, West
inline RelDir dirCarToRel(CarDir newCardDir, CarDir facing)
{
	if (facing == newCardDir)
		return FRONT;
	else if ((facing == NORTH && newCardDir == SOUTH) || (facing == EAST && newCardDir == WEST) || (facing == SOUTH && newCardDir == NORTH) || (facing == WEST && newCardDir == EAST))
		return BACK;
	else if ((facing == NORTH && newCardDir == WEST) || (facing == EAST && newCardDir == NORTH) || (facing == SOUTH && newCardDir == EAST) || (facing == WEST && newCardDir == SOUTH))
		return LEFT;
	else if ((facing == NORTH && newCardDir == EAST) || (facing == EAST && newCardDir == SOUTH) || (facing == SOUTH && newCardDir == WEST) || (facing == WEST && newCardDir == NORTH))
		return RIGHT;
}

#endif
