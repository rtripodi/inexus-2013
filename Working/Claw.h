#ifndef Claw_h
#define Claw_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Config.h"
#include <servo.h>

class Claw
{
	public:
		Claw(int leftServoPin, int rightServoPin);
	
		//  Init connection to left and right servos
		void setup();
		
		//  Opens claw wide
		void open();
		
		//  Closes claw enough to grab block
		void close();
		
		//  Shuts claw completely
		void shut();
		
	private:
		int leftPin, rightPin;
		Servo leftServo, rightServo;
};

#endif