#ifndef LineSensors_h
#define LineSensors_h

#include "config.h"
#include "PololuQTRSensors.h"

enum LineSensor_ColourValues {BLK, WHT, NUL};

class LineSensors : public PololuQTRSensorsRC
{
	public:
		LineSensors() : PololuQTRSensorsRC(QTR_SENSOR_PINS_LIST, QTR_NUM_PINS, QTR_TIMEOUT, QTR_EMITTER_PIN) {}
		unsigned int reading[QTR_NUM_PINS];
		void readCalibrated();
		bool see(const LineSensor_ColourValues vals[8]);
};

#endif
