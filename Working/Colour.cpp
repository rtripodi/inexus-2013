//Source: http://forum.arduino.cc/index.php/topic,21206.0.html#6
#include "Colour.h"

//#define PRINT_RAW
#define PRINT_PARSED
#define MULTIPLE_READINGS

void Colour::setup()
{
	reset();
	colSer.begin(4800);
	colSer.print("= (00 $ m) !");
	pinMode(COLOUR_OUTPUT_PIN, INPUT);
}

Colour::ColourType Colour::senseColour()
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

bool Colour::readData()
{
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
	return false;
}

void Colour::calibrateBlack()
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

void Colour::calibrateWhite()
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

void Colour::correctReading()
{	
	reading.red = round(255.0 * (float) (reading.red - blkRef.red) / (float) (whtRef.red - blkRef.red));
	reading.grn = round(255.0 * (float) (reading.grn - blkRef.grn) / (float) (whtRef.grn - blkRef.grn));
	reading.blu = round(255.0 * (float) (reading.blu - blkRef.blu) / (float) (whtRef.blu - blkRef.blu));
}

void Colour::waitForReading()
{
	while (!readData())
	{
		delay(COLOUR_LOOP_DELAY);
		delay(5);	//Needed or hang occurs.  Strangely, cannot just add to above Config delay.
	}
}

Colour::rgbColour Colour::averageReadings()
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

void Colour::reset() 
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
	
	while ((digitalRead(COLOUR_INPUT_PIN) != HIGH) || (digitalRead(COLOUR_OUTPUT_PIN) != HIGH))
	{
		delay(50);
	}
	
	pinMode(COLOUR_INPUT_PIN, OUTPUT);
	pinMode(COLOUR_OUTPUT_PIN, OUTPUT);
	digitalWrite(COLOUR_INPUT_PIN, LOW);
	digitalWrite(COLOUR_OUTPUT_PIN, LOW);
	delay(80);
	
	pinMode(COLOUR_INPUT_PIN, INPUT);
	pinMode(COLOUR_OUTPUT_PIN, OUTPUT);
	delay(COLOUR_DELAY);
} 
