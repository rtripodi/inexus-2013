#ifndef Movement_h
#define Movement_h

#include "PololuWheelEncoders.h"
#include "LineSensors.h"
#include "Motor.h"

class Movement
{
	public:
		Movement(Motor * inMotors, LineSensors * inSensors);
		bool onCross();
		void moveTillPoint(int speed);
		void moveOffCross(int speed);
		int tickError();
	private:
		PololuWheelEncoders wheelEnc;
		LineSensors * ls;
		Motor * motors;
};

#endif
