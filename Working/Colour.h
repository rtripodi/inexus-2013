#ifndef Colour_h
#define Colour_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Config.h"
#include <Serial.h>

class Colour
{
	public:
		enum ColourType {red, grn, blu, undef};
		
		Colour();// : serIn(COLOUR_PIN, COLOUR_UNUSED_PIN), serOut(COLOUR_UNUSED_PIN, COLOUR_PIN) {}
		
		void setup();
		
		Colour::ColourType senseColour();
		
		bool readData();
		
		void calibrateBlack();
		
		void calibrateWhite();
		
	private:
		//Set up two software serials on the same pin
		//SoftwareSerial serIn;
		//SoftwareSerial serOut;
		
		//Reading in format {red, blue, green}
		//Change to struct later
		int reading[3];
		int blkRef[3];
		int whtRef[3];
		
		void correctReading();
		
		void reset();
};

#endif
