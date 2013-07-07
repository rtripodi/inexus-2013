#include "LineSensors.h"

//Obtains a new reading and stores it in the public member reading[]
//(Just calls readCalibrated(unsigned int*) using the public member reading[])
inline void LineSensors::readCalibrated()
{
	PololuQTRSensors::readCalibrated(reading);
}

//Returns true if all values in public member reading[] are consistent with the specification in vals[]
//Each item in vals may be either BLK, WHT or NUL.  That is black, white and null (disregard / don't care) respectively.
//Thus, the function is used to check if the line sensor is reading the state specified in vals[].
bool LineSensors::see(const LineSensor_ColourValues vals[QTR_NUM_PINS])
{
	for(int ii = 0; ii < QTR_NUM_PINS; ++ii)
	{
		if(
		((vals[ii] == BLK) && (reading[ii] < MIN_BLACK)) || //vals[ii] is black and reading[ii] isn't black OR
		((vals[ii] == WHT) && (reading[ii] > MIN_BLACK))    //vals[ii] is white and reading[ii] isn't white
		) { return false; }
	}
	return true;
}
