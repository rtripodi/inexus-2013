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
		
		//  Read value from IR sensor
		//  Outputs distance in mm
		int read();
	
	private:
		//  Converts raw reading from medium range IR into mm
		int mediumIRtoMillimetres(int inRaw);
		
		//  Converts raw reading from far range IR into mm
		int farIRtoMillimetres(int inRaw);
}

#endif