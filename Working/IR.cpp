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
	switch (type)
	{
		case front:
			distInMillis = shortScan(reading);
			break;
		case right:
			distInMillis = mediumScanR(reading);
			break;
		case back:
			distInMillis = mediumScanB(reading);
			break;
		case left:
			distInMillis = mediumScanL(reading);
			break;
	}
	return distInMillis;
}

//Reads multiple raw values from IR sensor and returns a mean that disregards outliers
int IR::read()
{
	int rawReadings[IR_ITERATIONS];
	
	for (int ii = 0; ii < IR_ITERATIONS; ++ii)
	{
		rawReadings[ii] = analogRead(pin);
	}
	
	return calcMeanNoOutliers(rawReadings, IR_ITERATIONS);
}

//Reads multiple raw values from IR sensor and returns a pure mean that includes outliers
int IR::readPureMean()
{
	int dataSum = 0;
	for (int ii = 0; ii < IR_ITERATIONS; ++ii)
	{
		dataSum += analogRead(pin);
	}

	return (int) ( (float) dataSum / (float) IR_ITERATIONS + 0.5);
}

//Converts reading to distance in mm for 4-30cm sensor
//Returns -1 on error
//  TODO: check the effect on values outside 79-478 raw value range
int IR::shortScan(int reading)
{
	int millimetres = -1;	//Init to error value
	float rawReading = (float) reading;
	if (reading > 352 && reading <= 478)//50mm to 70mm
		millimetres = (int)(-0.1586*rawReading + 125.66);
	else if (reading > 224 && reading <= 352)//70mm to 108mm
		millimetres = (int)(-0.3093*rawReading + 177.79);
	else if (reading > 110 && reading <= 224)//108mm to 204mm
		millimetres = (int)(-0.855*rawReading + 298.04);
	else if (reading >= 79 && reading <= 110)//204mm to 295mm
		millimetres = (int)(-2.7738*rawReading + 513.8);
	else if (reading < 79)
		millimetres = 310;	//  Stub value for out of range
	return millimetres;
}

//UNIMPLEMENTED
//Converts reading to distance in mm for 10-80cm sensor
//Returns -1 on error
int IR::mediumScanR(int reading)
{
	if (reading > 627)
		return -1;
	else if (reading < 140)
		return 400;
	else
	{
		float result;
		result = 23170*pow( (float)reading, -0.844);
		reading = (int) (result + 0.5);
		
		return reading;
	}
}

int IR::mediumScanL(int reading)
{
	if (reading > 627)
		return -1;
	else if (reading < 150)
		return 400;
	else
	{
		float result;
		result = 24574*pow( (float)reading, -0.861);
		reading = (int) (result + 0.5);
		
		return reading;
	}
}

int IR::mediumScanB(int reading)
{
	if (reading > 627)
		return -1;
	else if (reading < 182)
		return 380;
	else
	{
		float result;
		result = 240052.0*pow( (float)reading, -1.243);
		reading = (int) (result + 0.5);
		
		return reading;
	}
}

//UNIMPLEMENTED
//Converts reading to distance in mm for 20-150cm sensor
//Returns -1 on error
int IR::longScan(int reading) { return -1; }

//Returns the mean which best represents the inputted data by disregarding outliers
int IR::calcMeanNoOutliers(int data[], int length)
{
	//Calculate raw mean
	int dataSum = 0;
	for (int ii = 0; ii < length; ++ii)
	{
		dataSum += data[ii];
	}
	float mean = (float) dataSum / (float) length;

	//Calculate standard deviation
	float devSum = 0.0;
	for (int ii = 0; ii < length; ++ii) 
	{
		devSum += pow((float) data[ii] - mean, 2);
	}
	float stdDev = pow(devSum / (float) length, 0.5);

	//Sum all values inclusive of the standard deviation
	dataSum = 0;
	int newCount = 0;
	for (int ii = 0; ii < length; ++ii)
	{
		if ((data[ii] > (mean - stdDev)) && (data[ii] < (mean + stdDev)))
		{
			dataSum += data[ii];
			newCount++;
		}
	}

	//If at least one element is inclusive of the standard deviation, recalculate the mean
	if (dataSum != 0)
		mean = (float) dataSum / (float) newCount;

	return (int) (mean + 0.5);
}
