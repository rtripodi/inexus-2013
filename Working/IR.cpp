#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "IR.h"

#define IR_IN_PIN A0	//  TODO: Stub value
#define IR_OUT_PIN 9	//  TODO: Stub value, not needed?

IR::IR(){}

//  Init connection to IR sensor
void IR::setup()
{
	Serial.begin(9600);
}

//  Read value from IR sensor
//  Outputs distance in mm
//  TODO:	check if outputValue is needed
//			allow for different range IR sensors
int IR::read()
{
	int sensorValue = 0;
//	int outputValue = 0;
	sensorValue = analogRead(IR_IN_PIN);
//	outputValue = map(sensorValue, 0, 1023, 0, 255);
//	analogWrite(IR_OUT_PIN, outputValue);
	return rawIRtoMillimetres(sensorValue);	//  Use sensorValue?
}

//  Converts raw reading from medium range IR into mm
//  TODO: check the effect on values outside 79-478 raw value range
int IR::mediumIRtoMillimetres(int inRaw)
(
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

//  Converts raw reading from far range IR into mm
//  TODO: actually implement
int IR::farIRtoMillimetres(int inRaw)
(
	return inRaw;	//  Stub value
}