/*
   Based on ColorPAL demo code for Arduino -
     Source: http://learn.parallax.com/colorpal-arduino-demo
     Author: Martin Heermance, with some assistance from Gordon McComb
	 
   ColorPAL Resources: http://classic.parallax.com/tabid/768/ProductID/617/Default.aspx
		or http://www.parallax.com/product/28380
*/

#include "Colour.h"

void Colour::setup()
{
	Serial.begin(9600);
	reset();
	ColourSensor.begin(COLOUR_BAUD);
	ColourSensor.write("= (00 $ m) !");  // Command to read RGB colors, see product documentation
	ColourSensor.end();
}

Colour::ColourType Colour::senseColour()
{
	bool success = readData();
    while (!success)
	{
		success = readData();
	}
	correctReading();
	if (reading.red > reading.green + reading.blue)
		return red;
	else if (reading.green > reading.red + reading.blue)
		return grn;
	else if (reading.blue > reading.red + reading.green)
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
			sscanf (buffer, "%3d%3d%3d", &reading.red, &reading.green, &reading.blue);
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
	reading.green = 255 * (int) ((float) (reading.green - blkRef.green) / (float) (whtRef.green - blkRef.green) + 0.5);
	reading.blue = 255 * (int) ((float) (reading.blue - blkRef.blue) / (float) (whtRef.blue - blkRef.blue) + 0.5);
}

// Reset ColorPAL; see ColorPAL documentation for sequence
void Colour::reset()
{
	delay(200);
	ColourSensor.write(LOW);
	while (ColourSensor.read() != HIGH);
	ColourSensor.write(LOW);
	delay(80);
	delay(COLOUR_DELAY);
}
