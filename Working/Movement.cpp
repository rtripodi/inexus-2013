#include "Movement.h"

#define SIMULATION
//#define DEBUG

Movement::Movement(Motor * inMotors, LineSensors * inSensors)
{
	motors = inMotors;
	wheelEnc.init(MOTOR_1_SIG_A, MOTOR_1_SIG_B, MOTOR_2_SIG_A, MOTOR_2_SIG_B);
	ls = inSensors;
}

bool Movement::onCross()
{
	const LineSensor_ColourValues CROSS[QTR_NUM_PINS] = {WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT};
	return ls->see(CROSS);
}


int lastLinePos = 0;
int linePos = 0;
int difference = 0; //Positive means the line is moving to the right

void Movement::reversing(int speed)
{
	linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
	while ((abs(linePos-3500))>500)
	{
		motors->left(-speed);
		motors->right(-speed);
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
	}
	motors->left(speed);
	motors->right(speed);
	linePos = 7000;
	while(abs(linePos-3500))
	{
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
	}
	int lineFirstSighted=linePos;
	motors->left(speed);
	motors->right(speed);
	delay(QTR_READ_DELAY);
	lastLinePos=linePos;
	linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
	difference = linePos-lastLinePos;
	while(difference>0)
	{
		motors->right(speed/2);
		motors->left(speed);
		delay(QTR_READ_DELAY/2);
		lastLinePos=linePos;
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
		difference = linePos-lastLinePos;
	}
	while(difference<0)
	{
		motors->right(speed);
		motors->left(speed/2);
		delay(QTR_READ_DELAY/2);
		lastLinePos=linePos;
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
		difference = linePos-lastLinePos;
	}
}

void Movement::lineCorrection(int speed)
{
	int qtrTotal;

	lastLinePos = linePos;
	qtrTotal = ls->reading[0]+ls->reading[1]+ls->reading[2]+ls->reading[3]+ls->reading[4]+ls->reading[5]+ls->reading[6]+ls->reading[7];
	if (qtrTotal<7000)
	{
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
		difference = linePos-lastLinePos;
		//If the line is within a margin of EDGE_SENSITIVITY
		if (abs(linePos-3500)<EDGE_SENSITIVITY)
		{
			if (difference<-CORRECTION_ANGLE)
			{
				motors->left(speed);
				motors->right(speed-speed/4);
			}
			if (difference>CORRECTION_ANGLE)
			{
				motors->right(speed);
				motors->left(speed-speed/4);
			}
			if (abs(difference)<CORRECTION_ANGLE)
			{
				motors->both(speed);
			}
		}
		//Otherwise, if it's right on the edge of the sensors
		else if (linePos>6000)
		{
			motors->left(speed/4);
			motors->right(speed);
		}
		else if (linePos<1000)
		{
			motors->left(speed);
			motors->right(speed/4);
		}
		//Else if it's off center but not too bad
		else if (linePos-3500>EDGE_SENSITIVITY)
		{
			motors->right(speed);
			motors->left(speed/2);
		}
		else if (linePos-3500<-EDGE_SENSITIVITY)
		{
			motors->right(speed/2);
			motors->left(speed);
		}
	}
	else 
	{
		//reversing();
	}
}

void Movement::reverseLineCorrection(int speed)
{
	int qtrTotal;

	lastLinePos = linePos;
	qtrTotal = ls->reading[0]+ls->reading[1]+ls->reading[2]+ls->reading[3]+ls->reading[4]+ls->reading[5]+ls->reading[6]+ls->reading[7];
	if (qtrTotal<7000)
	{
		linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
		difference = linePos-lastLinePos;
		//If the line is within a margin of EDGE_SENSITIVITY
				if (abs(linePos-3500)<EDGE_SENSITIVITY)
		{
			if (difference<-CORRECTION_ANGLE)
			{
				motors->left(speed);
				motors->right(speed-speed/4);
			}
			if (difference>CORRECTION_ANGLE)
			{
				motors->right(speed);
				motors->left(speed-speed/4);
			}
			if (abs(difference)<CORRECTION_ANGLE)
			{
				motors->both(speed);
			}
		}
		//Otherwise, if it's right on the edge of the sensors
		else if (linePos>6000)
		{
			motors->left(speed);
			motors->right(speed/4);
		}
		else if (linePos<1000)
		{
			motors->left(speed/4);
			motors->right(speed);
		}
		//Else if it's off center but not too bad
		else if (linePos-3500>EDGE_SENSITIVITY)
		{
			motors->right(speed/2);
			motors->left(speed);
		}
		else if (linePos-3500<-EDGE_SENSITIVITY)
		{
			motors->right(speed);
			motors->left(speed/2);
		}
	}
}

void Movement::moveTillPoint(int speed)
{
	if (speed > 0)
	{
		moveOffCross(speed);	
		while(!onCross())
		{
			lineCorrection(speed);
			delay(75);
		}
		moveOffCross(speed);
	}
	else if (speed < 0)
	{
		speed = - speed;		//Make speed positive to allow following code to be more understandable
		motors->both(-speed, tickError());
		while(!onCross())
		{
			delay(5);
		}
		moveOffCross(speed);
	}
}

void Movement::moveOffCross(int speed)
{
	while(onCross())
	{
		motors->both(speed, tickError());
	}
	motors->stop();
}

bool Movement::moveForward(int speed)
{
#ifndef SIMULATION
	if (!onCross())
	{	
		lineCorrection(speed);
		return false;
	}
	else
	{
#endif
		return true;
#ifndef SIMULATION
	}
#endif
}


void Movement::resetEncoders()
{
	wheelEnc.getCountsAndResetM1();
	wheelEnc.getCountsAndResetM2();
}

//returns the average of the ticks on both motors
int Movement::getTicks()
{
	int motorOne = abs(wheelEnc.getCountsM1());
	int motorTwo = abs(wheelEnc.getCountsM2());
	return (motorOne + motorTwo) / 2;
}

//tickError returns a value used to adjust the movement speed
//of wheels based on the number of ticks seen by each motor.
//It will be between -5 (M1 < M2) and 5 (M1 > M2)
int Movement::tickError()
{
	int leftTicks = abs(wheelEnc.getCountsLeft());
	int rightTicks = abs(wheelEnc.getCountsRight());
	return constrain((leftTicks - rightTicks), -5, 5);		//DEBUG: Removed multiplication by 3/4
}

//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//WARNING, ERROR, TODO: This is untested, also probably doesn't work going backwards
void Movement::moveTicks(int ticks, int speed)
{
#ifndef SIMULATION
	resetEncoders();
	
	//adjust speed and ticks if we're going backwards
	if (ticks < 0)
	{ 
		speed = - speed;
		ticks = - ticks;
	}
	
	//Initialise variables to hold ticks and speed
	int leftTicks = 0;
	int rightTicks = 0;
	int leftSpeed = speed;
	int rightSpeed = speed;
	
	#ifdef DEBUG
		Serial.print("Left\tRight");
	#endif
	while ( (abs(leftTicks) < abs(ticks) ) || (abs(rightTicks) < abs(ticks) ) )
	{
		//adjust motor speed to compensate for error
		int error = tickError();
		#ifdef DEBUG
			Serial.print("\t\tError: ");
			Serial.print(error);
		#endif
		if (error > 0)
		{
			#ifdef DEBUG
				Serial.println("\tSlowing left");
			#endif
			leftSpeed -= round(speed*TICKS_CORRECTION_FACTOR);
			rightSpeed = speed;
		}
		else if (error < 0)
		{
			#ifdef DEBUG
				Serial.println("\tSlowing right");
			#endif
			leftSpeed = speed;
			rightSpeed -= round(speed*TICKS_CORRECTION_FACTOR);
		}
		else
		{
			#ifdef DEBUG
				Serial.println("\tReseting speeds");
			#endif
			leftSpeed = speed;
			rightSpeed = speed;
		}
		
		//send message to motors to adjust speed
		motors->left(leftSpeed);
		motors->right(rightSpeed);
		delay(1); //Delay 1ms so we don't flood the Serial line
		
		//check ticks to see if we've moved far enough
		leftTicks = abs(wheelEnc.getCountsLeft());
		rightTicks = abs(wheelEnc.getCountsRight());
		#ifdef DEBUG
			Serial.print(leftTicks);
			Serial.print("\t");
			Serial.print(rightTicks);
		#endif
	}
#endif
}

//Converts length into ticks and calls moveTicks function
void Movement::moveLength(int length, int speed)
{
	//ticks = length*48/(D_w*pi) = length*48/(42*pi) ~= length*(48/132)
	//Where D_w is the wheel diameter
	moveTicks( round((float)(length * 48)/132.0), speed);
}

//Converts angle into ticks and calls rotateTicks function
void Movement::rotateAngle(int angle, int speed)
{
	//ticks = (angle/360)*(48*D_t)/D_w = (angle/360)*(48*145)/42 = angle*(23/63)
	//Where D_t is the turning circle diameter and D_w is the wheel diameter
	rotateTicks( round((float)(angle * 23)/63.0), speed);
}

//Turns in given relative direction
void Movement::rotateDirection(RelDir relDir, int speed)
{
#ifndef SIMULATION
	switch (relDir)
	{
	case FRONT:
		break;
	case RIGHT:
		rotateTicks(TICKS_RIGHT);
		break;
	case BACK:
		rotateTicks(TICKS_BACK);
		break;
	case LEFT:
		rotateTicks(TICKS_LEFT);
		break;
	}
#endif
}

void Movement::rotateTicks(int ticks, int speed)
{
#ifndef SIMULATION
	//Don't do anything if a rotation of zero is inputted
	if (ticks != 0)
	{
		resetEncoders();

		//Initialise variables to hold ticks and speed
		int leftTicks = 0;
		int rightTicks = 0;
		int leftSpeed = speed;
		int rightSpeed = speed;

		//adjust speed and angle if turning anti-clockwise
		if (ticks < 0)
		{ 
			leftSpeed = (-speed); 
			rightSpeed = speed; 
		}
		//adjust speed and angle if turning clockwise
		else if (ticks > 0)
		{ 
			leftSpeed = speed; 
			rightSpeed = (-speed); 
		}

		while ( (abs(leftTicks) < abs(ticks) ) || (abs(rightTicks) < abs(ticks)) )
		{
			//adjust motor speed to compensate for error
			/*int error = tickError();
			if (error > 0)
			{
				leftSpeed -= 1;
			}
			else if (error < 0)
			{
				rightSpeed -= 1;
			}*/

			//send message to motors to adjust speed
			motors->left(leftSpeed);
			motors->right(rightSpeed);
			delay(1); //Delay 1ms so we don't flood the Serial line

			//check ticks to see if we've moved far enough
			leftTicks = abs(wheelEnc.getCountsLeft());
			rightTicks = abs(wheelEnc.getCountsRight());
		}
		motors->stop();
		resetEncoders();
	}
#endif
}

// moveTicks moves the number of ticks given.
// A positive ticks number will go forward, a negative ticks number 
// will go backwards.
// We do not stop after hitting the number of ticks. Call motorStop().
void Movement::oldMoveTicks(int ticks, int motorSpeed)
{
	resetEncoders();
	
	if(ticks < 0)
	{
		motorSpeed = - motorSpeed;
	}
	
	int error, motorOne, motorTwo;
	do
	{
		motorOne = abs(wheelEnc.getCountsM1());
		motorTwo = abs(wheelEnc.getCountsM2());
		error = tickError();
		/*if (error < 0)
		{
			motors->left(motorSpeed);
			motors->right(0);
		}
		else if (error > 0)
		{
			motors->left(0);
			motors->right(motorSpeed);
		}
		else
		{*/
			motors->both(motorSpeed, error);
		//}
	} while ( (motorTwo < abs(ticks)) && (motorOne < abs(ticks)) );
}

void Movement::oldMoveLength(int length, int speed)
{
	oldMoveTicks( round((float)(length * 48)/132.0), speed);
}

//Rotates the number of ticks specified
//if it doesn't stop turning it means one of the encoders aren't working
//try fiddling with some wires or something...
void Movement::oldRotateTicks(int ticks, int motorSpeed)
{
	resetEncoders();

	if (ticks < 0)
	{
		motorSpeed = - motorSpeed;
	}

	int minSpeed = 28;
	int lowSpeed = motorSpeed / 2;
	if ( (lowSpeed > 0) && (lowSpeed < minSpeed) )
	{
		lowSpeed = minSpeed;
	}
	else if ( (lowSpeed < 0) && (lowSpeed > ( - minSpeed)) )
	{
		lowSpeed = - minSpeed;
	}

	int error, motorOne, motorTwo;
	do
	{
		motorOne = abs(wheelEnc.getCountsM1());
		motorTwo = abs(wheelEnc.getCountsM2());

		error = tickError();
		if (error < 0)
		{
			motors->left( - motorSpeed);
			motors->right(lowSpeed);
		}
		else if (error > 0)
		{
			motors->right(motorSpeed);
			motors->left( - lowSpeed);
		}
		else
		{
			motors->right(motorSpeed);
			motors->left( - motorSpeed);
		}
		delay(1);
	} while( (motorTwo < abs(ticks)) || (motorOne < abs(ticks)) );
	motors->stop();
	resetEncoders();
}
