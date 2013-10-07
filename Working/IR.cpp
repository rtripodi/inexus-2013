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
	float reading = (float) read();
	
	if(reading > 400.0)
	{
		return (int)((948.33 - reading) / 8.6);
	}
	else if(reading > 280.0)
	{
		return (int)((657.67 - reading) / 4.05); 
	}
	else if(reading > 148.0)
	{
		return (int)((417.86 - reading) / 1.6036); 
	}
	else if(reading > 78.0)
	{
		return (int)((260.1 - reading) / 0.5699); 
	}
	else if(reading > 37.0)
	{
		return (int)((121.04 - reading) / 0.1667); 
	}
	else
	{
		return 510;
	}
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

//Returns large value for debugging purposes
int IR::getDistLarge()
{
	return 10000;
}

//Returns small value for debugging purposes
int IR::getDistSmall()
{
	return 1;
}
