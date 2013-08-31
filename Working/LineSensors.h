#ifndef LineSensors_h
#define LineSensors_h

#include "Config.h"
#include "PololuQTRSensors.h"

//BLK = black, WHT = white, NUL = null/disregard/"don't care".
//Used to specify the states of each QTR sensor in the array
enum LineSensor_ColourValues {BLK, WHT, NUL};

class LineSensors : public PololuQTRSensorsRC
{
	public:
		LineSensors() : PololuQTRSensorsRC(QTR_SENSOR_PINS_LIST, QTR_NUM_PINS, QTR_TIMEOUT, QTR_EMITTER_PIN) {}

		//Holds the last reading of the line sensors iff readCalibrated(void) is called
		unsigned int reading[QTR_NUM_PINS];

		//Obtains a new reading and stores it in the public member reading[]
		//(Just calls readCalibrated(unsigned int*) using the public member reading[])
		void readCalibrated();

		//Returns true if all values in public member reading[] are consistent with the specification in vals[]
		//Each item in vals may be either BLK, WHT or NUL.  That is black, white and null (disregard / don't care) respectively.
		//Thus, the function is used to check if the line sensor is reading the state specified in vals[].
		bool see(const LineSensor_ColourValues vals[8]);
		
		int error();
                int getLineDistance();
};

#endif
