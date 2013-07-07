#include "IR.h"

IR::IR(int irPin, Range irType)
{
	pin = irPin;
	type = irType;
}

//No init necessary, placeholder function
void IR::setup(){}

//Returns distance read by sensor in mm
//Returns -1 on error
int IR::getDist()
{
	int reading = read();
	int distInMillis = -1;
	switch(type)
	{
		case shortRange:
			distInMillis = shortScan(reading);
			break;
		case mediumRange:
			distInMillis = mediumScan(reading);
			break;
		case longRange:
			distInMillis = longScan(reading);
			break;
	}
	return distInMillis;
}

//  Read multiple raw values from IR sensor and returns the mean
//  TODO:	check if outputValue is needed
//			implement outlier diregarding functionality
int IR::read()
{
	int sum = 0;
	
	for (int ii = 0; ii < IR_ITERATIONS; ii++)
	{
		sum += analogRead(pin);
	}
	
	int mean = (int)((float)sum / (float)IR_ITERATIONS + 0.5);
	
	return mean;
}

//Convert reading to distance in mm for 4-30cm sensor
//Returns -1 on error
//  TODO: check the effect on values outside 79-478 raw value range
int IR::shortScan(int reading)
{
	int millimetres = -1;	//Init to error value
	float rawReading = (float) reading;
	if(reading > 352 && reading <= 478)
		millimetres = (int)(-0.1586*rawReading + 125.66);
	else if(reading > 224 && reading <= 352)
		millimetres = (int)(-0.3093*rawReading + 177.79);
	else if(reading > 110 && reading <= 224)
		millimetres = (int)(-0.855*rawReading + 298.04);
	else if(reading >= 79 && reading <= 110)
		millimetres = (int)(-2.7738*rawReading + 513.8);
	else if(reading < 79)
		millimetres = 310;	//  Stub value for out of range
	return millimetres;
}

//UNIMPLEMENTED
//Convert reading to distance in mm for 10-80cm sensor
//Returns -1 on error
int IR::mediumScan(int reading) { return -1; }

//UNIMPLEMENTED
//Convert reading to distance in mm for 20-150cm sensor
//Returns -1 on error
int IR::longScan(int reading) { return -1; }