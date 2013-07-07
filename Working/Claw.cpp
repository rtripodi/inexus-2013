#include "Claw.h"

Claw::Claw(int leftServoPin, int rightServoPin)
{
	leftPin = leftServoPin;
	rightPin = rightServoPin;
}

//  Init connection to left and right servos
void Claw::setup()
{
	leftServo.attach(leftPin);
	rightServo.attach(rightPin);
}

//  Opens claw wide
void Claw::open()
{
	leftServo.write(CLAW_OPEN_LEFT);
	rightServo.write(CLAW_OPEN_RIGHT);
}

//  Closes claw enough to grab block
void Claw::close()
{
	leftServo.write(CLAW_CLOSE_LEFT);
	rightServo.write(CLAW_CLOSE_RIGHT);
}

//  Shuts claw completely
void Claw::shut()
{
	leftServo.write(CLAW_SHUT_LEFT);
	rightServo.write(CLAW_SHUT_RIGHT);
}