#ifndef Movement_h
#define Movement_h

#include "PololuWheelEncoders.h"
#include "LineSensors.h"
#include "Motor.h"
#include "Config.h"

class Movement
{
	public:
		Movement(Motor * inMotors, LineSensors * inSensors);
		bool onCross();
		void moveTillPoint(int speed = DEFAULT_SPEED);
		void moveOffCross(int speed = DEFAULT_SPEED);
		void moveTicks(int ticks, int speed = DEFAULT_SPEED);
		int tickError();
                void moveLength(int length, int speed = DEFAULT_SPEED);
                void rotateAngle(int angle, int speed = DEFAULT_SPEED);
             	private:
		PololuWheelEncoders wheelEnc;
		LineSensors * ls;
		Motor * motors;
};

#endif
