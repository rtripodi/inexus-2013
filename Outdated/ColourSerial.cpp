/*
   Based on ColorPAL demo code for Arduino -
     Source: http://learn.parallax.com/colorpal-arduino-demo
     Author: Martin Heermance, with some assistance from Gordon McComb
	 
   ColorPAL Resources: http://classic.parallax.com/tabid/768/ProductID/617/Default.aspx
		or http://www.parallax.com/product/28380
*/

#include "ColourSerial.h"

//#define PRINT_RAW
#define PRINT_PARSED
#define MULTIPLE_READINGS

ColourSerial::ColourSerial() {}

void ColourSerial::setup()
{
	reset();
	ColourSensor.begin(COLOUR_BAUD);
	ColourSensor.write("= (00 $ m) !");  // Command to read RGB colours, see product documentation
}

ColourSerial::ColourType ColourSerial::senseColour()
{
#ifdef MULTIPLE_READINGS
	reading = averageReadings();
	
	#ifdef PRINT_PARSED
		Serial.print("Average - R: ");
		Serial.print(reading.red);
		Serial.print("\tG: ");
		Serial.print(reading.grn);
		Serial.print("\tB: ");
		Serial.print(reading.blu);
		Serial.println();
	#endif
	
	correctReading();
#else
	waitForReading();
	correctReading();
#endif
	
	#ifdef PRINT_PARSED
		Serial.print("Corrected - R: ");
		Serial.print(reading.red);
		Serial.print("\tG: ");
		Serial.print(reading.grn);
		Serial.print("\tB: ");
		Serial.print(reading.blu);
		Serial.println();
	#endif
	
	float totalRGB = (float)(abs(reading.red) + abs(reading.grn) + abs(reading.blu));
	
	if ( ((float)reading.red / totalRGB) > 0.4)
		return red;
	else if ( ((float)reading.grn / totalRGB) > 0.4)
		return green;
	else if ( ((float)reading.blu / totalRGB) > 0.4)
		return blue;
	else
		return undef;
}

bool ColourSerial::readData()
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
			
			#ifdef PRINT_RAW
				Serial.print("Raw - R: ");
				for (int ii = 0; ii < 3; ++ii)
					Serial.print(buffer[ii]);
				Serial.print("\tG: ");
				for (int ii = 3; ii < 6; ++ii)
					Serial.print(buffer[ii]);
				Serial.print("\tB: ");
				for (int ii = 6; ii < 9; ++ii)
					Serial.print(buffer[ii]);
				Serial.println();
			#endif
			
			return true;
		}
	}
	return false;
}

void ColourSerial::calibrateBlack()
{
#ifdef MULTIPLE_READINGS
	blkRef = averageReadings();
	
	#ifdef PRINT_PARSED
		Serial.print("Average - R: ");
		Serial.print(blkRef.red);
		Serial.print("\tG: ");
		Serial.print(blkRef.grn);
		Serial.print("\tB: ");
		Serial.print(blkRef.blu);
		Serial.println();
	#endif
#else
	waitForReading();
	blkRef = reading;
#endif
}

void ColourSerial::calibrateWhite()
{
#ifdef MULTIPLE_READINGS
	whtRef = averageReadings();
	
	#ifdef PRINT_PARSED
		Serial.print("Average - R: ");
		Serial.print(whtRef.red);
		Serial.print("\tG: ");
		Serial.print(whtRef.grn);
		Serial.print("\tB: ");
		Serial.print(whtRef.blu);
		Serial.println();
	#endif
#else
	waitForReading();
	whtRef = reading;
#endif
}

//Follows following example formula stated in product documentation: Cr = 255 * (Ur – Kr) / (Wr – Kr)
//C: Corrected, U: Uncorrected, K: Black Reference, W: White Reference, r: Red
void ColourSerial::correctReading()
{
	reading.red = round(255.0 * (float) (reading.red - blkRef.red) / (float) (whtRef.red - blkRef.red));
	reading.grn = round(255.0 * (float) (reading.grn - blkRef.grn) / (float) (whtRef.grn - blkRef.grn));
	reading.blu = round(255.0 * (float) (reading.blu - blkRef.blu) / (float) (whtRef.blu - blkRef.blu));
}

void ColourSerial::waitForReading()
{
	while (!readData())
	{
		delay(COLOUR_LOOP_DELAY);
		delay(5);	//Needed or hang occurs.  Strangely, cannot just add to above Config delay.
	}
}

ColourSerial::rgbColour ColourSerial::averageReadings()
{	
	rgbColour temp;
	float totalRed = 0.0, totalGreen = 0.0, totalBlue = 0.0;
	for (int ii = 0; ii < COLOUR_ITERATIONS; ++ii)
	{
		waitForReading();
		totalRed += (float)reading.red;
		totalGreen += (float)reading.grn;
		totalBlue += (float)reading.blu;
	}
	temp.red = (int)(totalRed/((float)COLOUR_ITERATIONS) + 0.5);
	temp.grn = (int)(totalGreen/((float)COLOUR_ITERATIONS) + 0.5);
	temp.blu = (int)(totalBlue/((float)COLOUR_ITERATIONS) + 0.5);
	
	return temp;
}

// Reset ColorPAL; see ColorPAL documentation for sequence
void ColourSerial::reset()
{
	delay(COLOUR_DELAY);
	pinMode(ColourSensor, OUTPUT);
	digitalWrite(ColourSensor, LOW);
	pinMode(ColourSensor, INPUT);
	while (digitalRead(ColourSensor) != HIGH);
	pinMode(ColourSensor, OUTPUT);
	digitalWrite(ColourSensor, LOW);
	delay(80);
	pinMode(ColourSensor, INPUT);
	delay(COLOUR_DELAY);
}
