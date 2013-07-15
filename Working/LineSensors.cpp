#include "LineSensors.h"

//Obtains a new reading and stores it in the public member reading[]
//(Just calls readCalibrated(unsigned int*) using the public member reading[])
void LineSensors::readCalibrated()
{
	PololuQTRSensors::readCalibrated(reading);
}

//Returns true if all values in public member reading[] are consistent with the specification in vals[]
//Each item in vals may be either BLK, WHT or NUL.  That is black, white and null (disregard / don't care) respectively.
//Thus, the function is used to check if the line sensor is reading the state specified in vals[].
bool LineSensors::see(const LineSensor_ColourValues vals[QTR_NUM_PINS])
{
  readCalibrated();
	for(int ii = 0; ii < QTR_NUM_PINS; ++ii)
	{
		if(
		((vals[ii] == BLK) && (reading[ii] < MIN_BLACK)) || //vals[ii] is black and reading[ii] isn't black OR
		((vals[ii] == WHT) && (reading[ii] > MIN_BLACK))    //vals[ii] is white and reading[ii] isn't white
		) { return false; }
	}
	return true;
}

int LineSensors::error()
{
	const bool WHITE_LINES = true;
        int error = PololuQTRSensors::readLine(reading, QTR_EMITTERS_ON, WHITE_LINES); //error = 0 to 6000

	error -= 3000; //give us -3000 to 3000

	//-2 is to shift the robot a bit because the calculation is a little off
	//(gives about -14 to 16).
	error = ((15 * error) / 3000) - 2;

	if((error < 5) && (error > -5)){//amplify the forwardError if it's too low
		error *= 2;
	}
        return error;
}
