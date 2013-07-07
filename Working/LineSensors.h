#ifndef LineSensors_h
#define LineSensors_h

#include "PololuQTRSensors.h"

#define MIN_BLACK (250)

enum LineSensor_ColourValues {BLK, WHT, NUL};

class LineSensors : public PololuQTRSensorsRC
{
	public:
		int reading[QTR_NUM_PINS];
		void readCalibrated();
		bool areReading(LineSensor_ColourValues vals[8]);
};

#endif