#ifndef Movement_h
#define Movement_h

#include "PololuWheelEncoders.h"
#include "LineSensors.h"
#include "Motor.h"
#include "Direction.h"
#include "Config.h"

class Movement
{
	public:
		Movement(Motor * inMotors, LineSensors * inSensors);
		bool onCross();
		void moveTillPoint(int speed = DEFAULT_SPEED);
		void moveOffCross(int speed = DEFAULT_SPEED);
		bool moveForward(int speed = DEFAULT_SPEED);
		void moveTicks(int ticks, int speed = DEFAULT_SPEED);
		int tickError();
		void moveLength(int length, int speed = DEFAULT_SPEED);
		void rotateDirection(RelDir relDir, int speed = DEFAULT_SPEED);
		void rotateAngle(int angle, int speed = DEFAULT_SPEED);
		void rotateTicks(int ticks, int speed = DEFAULT_SPEED);

		void resetEncoders();
		int getTicks();
		void oldMoveTicks(int ticks, int motorSpeed);
		void oldMoveLength(int length, int speed);
		void oldRotateAngle(int angle, int speed);
		void oldRotateTicks(int ticks, int motorSpeed);
		
	private:
		void reversing(int speed = DEFAULT_SPEED);
		void lineCorrection(int speed = DEFAULT_SPEED);
		PololuWheelEncoders wheelEnc;
		LineSensors * ls;
		Motor * motors;
};

#endif
