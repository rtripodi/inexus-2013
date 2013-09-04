/*
   Based on ColorPAL demo code for Arduino -
     Source: http://learn.parallax.com/colorpal-arduino-demo
     Author: Martin Heermance, with some assistance from Gordon McComb
	 
   ColorPAL Resources: http://classic.parallax.com/tabid/768/ProductID/617/Default.aspx
		or http://www.parallax.com/product/28380
*/

#include "Colour.h"

Colour::Colour() {}

void Colour::setup()
{
	reset();
	ColourSensor.begin(COLOUR_BAUD);
	ColourSensor.write("= (00 $ m) !");  // Command to read RGB colours, see product documentation
}

Colour::ColourType Colour::senseColour()
{
	bool success = readData();
    while (!success)
	{
		success = readData();
	}
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

bool Colour::readData()
{
	char buffer[32];
	
	if (ColourSensor.available() > 0)
	{
		//Wait for a $ character, then read three 3 digit hex numbers
		buffer[0] = ColourSensor.read();
		if (buffer[0] == '$')
		{
			for(int ii = 0; ii < 9; ++ii)
			{
				while (ColourSensor.available() == 0);  //Wait for next input character
				buffer[ii] = ColourSensor.read();
				if (buffer[ii] == '$')  //Return early if $ character encountered
					return false;
			}
			sscanf (buffer, "%3x%3x%3x", &reading.red, &reading.grn, &reading.blu);
			delay(10);
			return true;
		}
	}
	return false;
}

void Colour::calibrateBlack()
{
	bool success = readData();
    while (!success)
	{
		success = readData();
	}
	blkRef = reading;
}

void Colour::calibrateWhite()
{
	bool success = readData();
    while (!success)
	{
		success = readData();
	}
	whtRef = reading;
}

//Follows following example formula stated in product documentation: Cr = 255 * (Ur – Kr) / (Wr – Kr)
//C: Corrected, U: Uncorrected, K: Black Reference, W: White Reference, r: Red
void Colour::correctReading()
{
	reading.red = 255 * (int) ((float) (reading.red - blkRef.red) / (float) (whtRef.red - blkRef.red) + 0.5);
	reading.grn = 255 * (int) ((float) (reading.grn - blkRef.grn) / (float) (whtRef.grn - blkRef.grn) + 0.5);
	reading.blu = 255 * (int) ((float) (reading.blu - blkRef.blu) / (float) (whtRef.blu - blkRef.blu) + 0.5);
}

// Reset ColorPAL; see ColorPAL documentation for sequence
void Colour::reset()
{
	delay(COLOUR_DELAY);
	pinMode(ColourSensor, OUTPUT);
	digitalWrite(ColourSensor, LOW);
	pinMode(ColourSensor, INPUT);
	while (digitalRead(ColourSensor) != HIGH) Serial.println("Waiting for low");;
	pinMode(ColourSensor, OUTPUT);
	digitalWrite(ColourSensor, LOW);
	delay(80);
	pinMode(ColourSensor, INPUT);
	delay(COLOUR_DELAY);
}
