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
		enum Range {shortRange, mediumRange, longRange};//Three types of IR sensor we use
		
		IR(int irPin, Range irType);
	
		//Placeholder incase initialisation is needed
		void setup();
		
		//Returns distance read by sensor in mm
		//Returns -1 on error
		int getDist();

	private:
		int pin; //Analog pin IR sensor is attached to
		Range type; //Type of sensor for determining function to use to convert readings to mm
		
		//  Read multiple raw values from IR sensor and returns the mean
		int read();
		
		//Convert reading to distance in mm for 4-30cm sensor
		//Returns -1 on error
		int shortScan(int reading);
		
		//UNIMPLEMENTED
		//Convert reading to distance in mm for 10-80cm sensor
		//Returns -1 on error
		int mediumScan(int reading);
		
		//UNIMPLEMENTED
		//Convert reading to distance in mm for 20-150cm sensor
		//Returns -1 on errorr
		int longScan(int reading);
};

#endif