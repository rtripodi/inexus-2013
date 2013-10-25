#include "Movement.h"

#define SIMULATION

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
			if (difference<-30)
			{
				motors->left(speed);
				motors->right(speed-speed/4);
			}
			if (difference>30)
			{
				motors->right(speed);
				motors->left(speed-speed/4);
			}
			if (abs(difference)<30)
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

void Movement::moveTillPoint(int speed)
{
	moveOffCross(speed);	
	while(!onCross())
	{
		lineCorrection(speed);
		delay(75);
	}
	moveOffCross(speed);
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
//It will be between -5 and 5
int Movement::tickError()
{
	int motorOne = abs(wheelEnc.getCountsM1());
	int motorTwo = abs(wheelEnc.getCountsM2());
	return constrain((motorOne - motorTwo), -5, 5);		//DEBUG: Removed multiplication by 3/4
}

//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//WARNING, ERROR, TODO: This is untested, also probably doesn't work going backwards
void Movement::moveTicks(int ticks, int speed)
{
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

	while ((abs(leftTicks) < abs(ticks) ) || (abs(rightTicks) < abs(ticks)))
	{
		//adjust motor speed to compensate for error
		/*int error = tickError();			//TODO: Investigate effect
		if (error > 0)
		{
			leftSpeed -= 1;
		}
		else if (error < 0)
		{
			rightSpeed -= 1;
		}
		else
		{*/
			leftSpeed = speed;
			rightSpeed = speed;
		//}
		
		//send message to motors to adjust speed
		motors->left(leftSpeed);
		motors->right(rightSpeed);
		delay(1); //Delay 1ms so we don't flood the Serial line

		//check ticks to see if we've moved far enough
		leftTicks = abs(wheelEnc.getCountsM1());
		rightTicks = abs(wheelEnc.getCountsM2());
	}
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
		if (error < 0)
		{
			motors->right(0);
			motors->left(motorSpeed);
		}
		else if (error > 0)
		{
			motors->left(0);
			motors->right(motorSpeed);
		}
		else
		{
			motors->both(motorSpeed, error);
		}
	} while ( (motorTwo < abs(ticks)) && (motorOne < abs(ticks)) );
}

//Length is in mm, rounded to whole number.
//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//convert the length to ticks
//Simplified formulae: ( Length * 48 [number of ticks in one revolution]) / ( Pi * Wheel Diameter)
void Movement::moveLength(int length, int speed)
{
	moveTicks( round((float)(length * 48)/135.1), speed);
}

void Movement::oldMoveLength(int length, int speed)
{
	oldMoveTicks( round((float)(length * 48)/135.1), speed);
}

//Uses rotateTicks function from old code
void Movement::oldRotateAngle(int angle, int speed)
{
	rotateTicks( round((float)(abs(angle) * 96)/215.0), speed);
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

//clockwise turning is positive angle, anti-clockwise turning is negative angle.Speed is positive.
//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
void Movement::rotateAngle(int angle, int speed)
{
	//Don't do anything if a rotation of zero is inputted
	if (angle != 0)
	{
		angle = constrain(angle,-180,180);

		resetEncoders();

		//Initialise variables to hold ticks and speed
		int ticks = 0;
		int leftTicks = 0;
		int rightTicks = 0;
		int leftSpeed = speed;
		int rightSpeed = speed;

		//convert the angle to ticks
		//Simplified formulae: (Wheel Base Diameter * 48 [number of ticks in one revolution]) / (Wheel Diameter * 360) * Angle
		ticks = round((abs(angle) * 96)/215);
		//ticks = round((18*abs(angle/360)/(3))*48)
		//replace 18 with distance between wheels, 3 with wheel diameter

		//adjust speed and angle if turning anti-clockwise
		if (angle < 0)
		{ 
			leftSpeed = - speed; 
			rightSpeed = speed; 
		}
		//adjust speed and angle if turning clockwise
		else if (angle > 0)
		{ 
			leftSpeed = speed; 
			rightSpeed = - speed; 
		}

		while ( (abs(leftTicks) < abs(ticks) ) || (abs(rightTicks) < abs(ticks)) )
		{
			//adjust motor speed to compensate for error
			/*int error = tickError();			//TODO: Investigate effect
			if (error > 0)
			{
				leftSpeed -= 1;
			}
			else if (error < 0)
			{
				rightSpeed -= 1;
			}
			else
			{*/
				leftSpeed = speed;
				rightSpeed = speed;
			//}

			//send message to motors to adjust speed
			motors->left(leftSpeed);
			motors->right(rightSpeed);
			delay(1); //Delay 1ms so we don't flood the Serial line

			//check ticks to see if we've moved far enough
			leftTicks = abs(wheelEnc.getCountsM1());
			rightTicks = abs(wheelEnc.getCountsM2());
		}
	}
}

void Movement::rotateTicks(int ticks, int speed)
{
Serial.println(ticks);
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
			leftTicks = abs(wheelEnc.getCountsM1());
			rightTicks = abs(wheelEnc.getCountsM2());
		}
		motors->stop();
		resetEncoders();
	}
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
