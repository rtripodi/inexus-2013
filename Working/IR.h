#ifndef IR_h
#define IR_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class IR
{
	public:
		//  Init connection to IR sensor
		void setup();

		//  Read value from medium ranged IR sensor and returns the distance in mm
		int mediumScan(int inPin);
		
		//  Read value from long ranged IR sensor and returns the distance in mm
		int longScan(int inPin);

	private:
		//  Read multiple raw values from IR sensor and returns the mean
		int read(int inPin)
}

#endif