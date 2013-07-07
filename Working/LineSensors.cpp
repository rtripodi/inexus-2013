#include "LineSensors.h"

inline void LineSensors::readCalibrated()
{
	readCalibrated(reading);
}

bool LineSensors::areReading(LineSensor_ColourValues vals[QTR_NUM_PINS])
{
	for(int ii = 0; ii < QTR_NUM_PINS; ++ii)
	{
		if(
		((vals[ii] == BLK) && (reading[ii] < MIN_BLACK)) ||
		((vals[ii] == WHT) && (reading[ii] > MIN_BLACK))
		) { return false; }
	}
	return true;
}