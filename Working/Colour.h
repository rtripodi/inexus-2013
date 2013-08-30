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
		
		Colour();
		
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
		struct rgbColour
		{
			unsigned char red;
			unsigned char green;
			unsigned char blue;
		};
		
		rgbColour reading;
		rgbColour blkRef;
		rgbColour whtRef;
		
		void correctReading();
		
		void reset();
};

#endif
