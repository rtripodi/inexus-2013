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
	serOut.begin(COLOUR_BAUD);
	pinMode(COLOUR_PIN, OUTPUT);
	serOut.print("= (00 $ m) !");  // Command to read RGB colors, see datasheet
	serOut.end();

    serIn.begin(COLOUR_BAUD);
	pinMode(COLOUR_PIN, INPUT);
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

	if (serIn.available() > 0)
	{
		//Wait for a $ character, then read three 3 digit hex numbers
		buffer[0] = serIn.read();
		if (buffer[0] == '$')
		{
			for(int ii = 0; ii < 9; ++ii)
			{
				while (serIn.available() == 0);  //Wait for next input character
				buffer[ii] = serIn.read();
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
	pinMode(COLOUR_PIN, OUTPUT);
	digitalWrite(COLOUR_PIN, LOW);
	pinMode(COLOUR_PIN, INPUT);
	while (digitalRead(COLOUR_PIN) != HIGH);
	pinMode(COLOUR_PIN, OUTPUT);
	digitalWrite(COLOUR_PIN, LOW);
	delay(80);
	pinMode(COLOUR_PIN, INPUT);
	delay(COLOUR_DELAY);
}
