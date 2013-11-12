#ifndef Colour_h
#define Colour_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Config.h"
#include <Serial.h>

class ColourSerial
{
	public:
		enum ColourType {red, green, blue, undef};
		
		ColourSerial();
		
		void setup();
		
		ColourSerial::ColourType senseColour();
		
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
			int red;
			int grn;
			int blu;
		};
		
		rgbColour reading;
		rgbColour blkRef;
		rgbColour whtRef;
		
		void correctReading();
		
		void waitForReading();
		
		rgbColour averageReadings();
		
		void reset();
};

#endif
