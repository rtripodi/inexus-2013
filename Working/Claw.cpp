#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Servo.h>
#include "Claw.h"

#define CLAW_LEFT_PIN 6		//  TODO: Stub value
#define CLAW_RIGHT_PIN 7	//  TODO: Stub value
#define CLAW_OPEN_LEFT 95
#define CLAW_OPEN_RIGHT 89
#define CLAW_CLOSE_LEFT 140
#define CLAW_CLOSE_RIGHT 44
#define CLAW_SHUT_LEFT 189
#define CLAW_SHUT_RIGHT -1

Claw::Claw(){}

//  Init connection to left and right servos
void Claw::setup()
{
	leftServo.attach(CLAW_LEFT_PIN);
	rightServo.attach(CLAW_RIGHT_PIN);
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