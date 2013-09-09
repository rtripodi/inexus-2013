//Source: http://forum.arduino.cc/index.php/topic,21206.0.html#6
#include "ColourSoftware.h"

void ColourSoftware::setup()
{
	reset();
	colSer.begin(4800);
}

ColourSoftware::ColourType ColourSoftware::senseColour()
{
	while (!readData())
		delay(COLOUR_LOOP_DELAY);
	correctReading();
	senseColour();
	Serial.print("R: ");//DEBUG
	Serial.print(reading.red);//DEBUG
	Serial.print("\tG: ");//DEBUG
	Serial.print(reading.grn);//DEBUG
	Serial.print("\tB: ");//DEBUG
	Serial.print(reading.blu);//DEBUG
	Serial.println();//DEBUG
	if (reading.red > reading.grn + reading.blu)
		return red;
	else if (reading.grn > reading.red + reading.blu)
		return grn;
	else if (reading.blu > reading.red + reading.grn)
		return blu;
	else
		return undef;
}

bool ColourSoftware::readData()
{
	colSer.print("= (00 $ m) !");
	pinMode(COLOUR_OUTPUT_PIN, INPUT);
	char buffer[9];
	buffer[0] = colSer.read();
	if (buffer[0] == '$')
	{
		for (int ii = 0; ii < 9; ++ii)
		{
			buffer[ii] = colSer.read();
			if (buffer[ii] == '$')  //Return early if $ character encountered
				return false;
		}
		sscanf (buffer, "%3x%3x%3x", &reading.red, &reading.grn, &reading.blu);
		Serial.print("R: ");//DEBUG
		for (int ii = 0; ii < 3; ++ii)//DEBUG
			Serial.print(buffer[ii]);//DEBUG
		Serial.print("\tG: ");//DEBUG
		for (int ii = 3; ii < 6; ++ii)//DEBUG
			Serial.print(buffer[ii]);//DEBUG
		Serial.print("\tB: ");//DEBUG
		for (int ii = 6; ii < 9; ++ii)//DEBUG
			Serial.print(buffer[ii]);//DEBUG
		Serial.println();//DEBUG
		return true;
	}
	return false;
}

void ColourSoftware::calibrateBlack()
{
    while (!readData())
		delay(COLOUR_LOOP_DELAY);
	blkRef = reading;
}

void ColourSoftware::calibrateWhite()
{
	while (!readData())
		delay(COLOUR_LOOP_DELAY);
	whtRef = reading;
}

void ColourSoftware::correctReading()
{
	reading.red = 255 * (int) ((float) (reading.red - blkRef.red) / (float) (whtRef.red - blkRef.red) + 0.5);
	reading.grn = 255 * (int) ((float) (reading.grn - blkRef.grn) / (float) (whtRef.grn - blkRef.grn) + 0.5);
	reading.blu = 255 * (int) ((float) (reading.blu - blkRef.blu) / (float) (whtRef.blu - blkRef.blu) + 0.5);
}

void ColourSoftware::reset() 
{ 
	pinMode(COLOUR_INPUT_PIN, INPUT);
	pinMode(COLOUR_OUTPUT_PIN, INPUT);
	digitalWrite(COLOUR_INPUT_PIN, HIGH); // Enable the pull-up resistor
	digitalWrite(COLOUR_OUTPUT_PIN, HIGH); // Enable the pull-up resistor

	pinMode(COLOUR_INPUT_PIN, OUTPUT);
	pinMode(COLOUR_OUTPUT_PIN, OUTPUT);
	digitalWrite(COLOUR_INPUT_PIN, LOW);
	digitalWrite(COLOUR_OUTPUT_PIN, LOW);

	pinMode(COLOUR_INPUT_PIN,INPUT);
	pinMode(COLOUR_OUTPUT_PIN,INPUT);

	while ((digitalRead(COLOUR_INPUT_PIN) != HIGH) || (digitalRead(COLOUR_OUTPUT_PIN) != HIGH));

	pinMode(COLOUR_INPUT_PIN, OUTPUT);
	pinMode(COLOUR_OUTPUT_PIN, OUTPUT);
	digitalWrite(COLOUR_INPUT_PIN, LOW);
	digitalWrite(COLOUR_OUTPUT_PIN, LOW);
	delay(80);

	pinMode(COLOUR_INPUT_PIN, INPUT);
	pinMode(COLOUR_OUTPUT_PIN, OUTPUT);
	delay(100);
} 
