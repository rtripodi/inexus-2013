#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "IR.h"

#define IR_ITERATIONS 5

IR::IR(){}

//  Init connection to IR sensor
void IR::setup()
{
	Serial.begin(9600);
}

//  Read value from medium ranged IR sensor and returns the distance in mm
//  TODO: check the effect on values outside 79-478 raw value range
int IR::mediumScan(int inPin);
(
	int rawReading = read(inPin);
	
	int millimetres = 0;	//  Stub value for default
	if(inRaw > 352 && inRaw <= 478)
		millimetres = (int)(-0.1586*inRaw + 125.66);
	else if(inRaw > 224 && inRaw <= 352)
		millimetres = (int)(-0.3093*inRaw + 177.79);
	else if(inRaw > 110 && inRaw <= 224)
		millimetres = (int)(-0.855*inRaw + 298.04);
	else if(inRaw >= 79 && inRaw <= 110)
		millimetres = (int)(-2.7738*inRaw + 513.8);
	else if(inRaw < 79)
		distanceInCm = 31;	//  Stub value for out of range
	return millimetres;
}

//  Read value from long ranged IR sensor and returns the distance in mm
//  TODO: actually implement
int IR::longScan(int inPin);
(
	int rawReading;
	rawReading = read(inPin);

	int millimetres = 0;	//  Stub value for default
	return millimetres;	//  Stub value
}

//  Read multiple raw values from IR sensor and returns the mean
//  TODO:	check if outputValue is needed
//			implement outlier diregarding functionality
int IR::read(int inPin)
{
	int mean,
		sum = 0;
	
	for (int ii = 0; ii < IR_ITERATIONS; ii++)
	{
		sum += analogRead(pin);
	}
	
	mean = (int)((float)sum / (float)IR_ITERATIONS + 0.5);	//  TODO: Check if values can be negative
	
	return mean;
}