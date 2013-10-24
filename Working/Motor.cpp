#include "Motor.h"

Motor::Motor(){}

//Init connection to motor controller.
//Run in setup() function.
void Motor::setup()
{
	MotorControl.begin(MOTOR_BAUD_RATE);
	MotorControl.write(MOTOR_ESTABLISH_BAUD);
}

//Adjust the speed of both motors
//Positive speed goes forwards, negative goes backwards
void Motor::both(int motorSpeed, int error)
{
	if (motorSpeed < 0)
	{
		error = - error;
	}
	
	left(motorSpeed - error);
	right(motorSpeed + error);
}

//Positive for forward, negative for backwards.
//motorSpeed will be constrained between 0 and 127
void Motor::left(int motorSpeed)
{
	const byte leftMotorAntiClockwise = 0x8C,
		leftMotorClockwise = 0x8E;

	if (motorSpeed >= 0)
	{
		MotorControl.write(leftMotorAntiClockwise);
		MotorControl.write(limit_0_to_127(motorSpeed));
	}
	else
	{
		motorSpeed = - motorSpeed;
		MotorControl.write(leftMotorClockwise);
		MotorControl.write(limit_0_to_127(motorSpeed));
	}
}

//Positive for forward, negative for backwards.
//motorSpeed will be constrained between 0 and 127
void Motor::right(int motorSpeed)
{
	const byte rightMotorAntiClockwise = 0x8A,
		rightMotorClockwise = 0x88;

	if (motorSpeed >= 0)
	{
		MotorControl.write(rightMotorClockwise);
		MotorControl.write(limit_0_to_127(motorSpeed));
	}
	else
	{
		motorSpeed = - motorSpeed;
		MotorControl.write(rightMotorAntiClockwise);
		MotorControl.write(limit_0_to_127(motorSpeed));
	}
}

//Stops both motors
//If we stop, we need to remove the error
void Motor::stop()
{
	both(0,0);
}

//Returns val if it is between 0 and 127.
//If less than 0, it returns 0.
//If greater than 127, it returns 127.
int Motor::limit_0_to_127(int val)
{
	if (val < 0)
	{
		val = 0;
	}
	else if (val > 127)
	{
		val = 127;
	}
	return val;
}
