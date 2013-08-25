/*
   Based on ColorPAL demo code for Arduino -
     Source: http://learn.parallax.com/colorpal-arduino-demo
     Author: Martin Heermance, with some assistance from Gordon McComb
	 
   ColorPAL Resources: http://www.parallax.com/tabid/768/ProductID/617/Default.aspx
*/

#include "Colour.h"

void Colour::setup()
{
	Serial.begin(9600);
	reset();
	ColourSensor.begin(COLOUR_BAUD);
	ColourSensor.write("= (00 $ m) !");  // Command to read RGB colors, see datasheet
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
	if (reading[red] > reading[grn] + reading[blu])
		return red;
	else if (reading[grn] > reading[red] + reading[blu])
		return grn;
	else if (reading[blu] > reading[red] + reading[grn])
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
			sscanf (buffer, "%3d%3d%3d", &reading[red], &reading[grn], &reading[blu]);
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
	for (int ii = 0; ii < 3; ++ii)
		blkRef[ii] = reading[ii];
}

void Colour::calibrateWhite()
{
	bool success = readData();
    while (!success)
	{
		success = readData();
	}
	for (int ii = 0; ii < 3; ++ii)
		whtRef[ii] = reading[ii];
}

//Follows following example formula stated in datasheet: Cr = 255 * (Ur – Kr) / (Wr – Kr)
//C: Corrected, U: Uncorrected, K: Black Reference, W: White Reference, r: Red
void Colour::correctReading()
{
	for (int ii = 0; ii < 3; ++ii)
    	reading[ii] = 255 * (int) ((float) (reading[ii] - blkRef[ii]) / (float) (whtRef[ii] - blkRef[ii]) + 0.5);
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
