#include "Movement.h"

Movement::Movement(Motor * inMotors, LineSensors * inSensors)
{
	motors = inMotors;
	wheelEnc.init(MOTOR_1_SIG_A, MOTOR_1_SIG_B, MOTOR_2_SIG_A, MOTOR_2_SIG_B);
	ls = inSensors;
}

bool Movement::onCross()
{
	const LineSensor_ColourValues CROSS[QTR_NUM_PINS] = {WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT};
	return ls->see(CROSS);
}

void Movement::moveTillPoint(int speed)
{
	moveOffCross(speed);	
	while(!onCross())
	{
		motors->both(speed, ls->error());
	}
}

void Movement::moveOffCross(int speed)
{
	while(onCross())
	{
		motors->both(speed, tickError());
	}
}

int Movement::tickError()
{
	int motorOne = abs(wheelEnc.getCountsM1());
	int motorTwo = abs(wheelEnc.getCountsM2());
	return constrain((motorOne - motorTwo), -5, 5);
}

