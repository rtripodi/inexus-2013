#ifndef ColourSoftware_h
#define ColourSoftware_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Config.h"
#include <OldSoftwareSerial.h>

#define COLOUR_INPUT_PIN 2
#define COLOUR_OUTPUT_PIN 3

class ColourSoftware
{
	public:
		enum ColourType {red, grn, blu, undef};
		
		ColourSoftware() : colSer(COLOUR_INPUT_PIN, COLOUR_OUTPUT_PIN) {}
		
		void setup();
		
		ColourSoftware::ColourType senseColour();
		
		bool readData();
		
		void calibrateBlack();
		
		void calibrateWhite();
		
	private:
		OldSoftwareSerial colSer;
		
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
		
		void reset();
};

#endif
