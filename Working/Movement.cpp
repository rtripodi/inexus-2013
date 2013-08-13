#include "Movement.h"

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
    int difference=0; //Positive means the line is moving to the right
    
void Movement::reversing(int speed)
{
  linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
  while ((abs(linePos-3500))>500)
  {
    motors->left(-speed);
    motors->right(-speed);
    linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
  }
  delay(200);
  linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
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
    delay(QTR_MOVEMENT_DELAY/2);
    lastLinePos=linePos;
    linePos = ls->readLine(ls->reading, QTR_EMITTERS_ON, 1);
    difference = linePos-lastLinePos;
  }
  while(difference<0)
  {
    motors->right(speed);
    motors->left(speed/2);
    delay(QTR_MOVEMENT_DELAY/2);
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
    Movement::reversing();
  }
}

void Movement::moveTillPoint(int speed)
{
  moveOffCross(speed);	
  while(!onCross())
  {
    Movement::lineCorrection(speed);
  }
}

void Movement::moveOffCross(int speed)
{
	while(onCross())
	{
		motors->both(speed, tickError());
	}
}

void Movement::moveForward(int speed)
{
	moveOffCross(speed);
	motors->both(speed, ls->error());
}

//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//WARNING, ERROR, TODO: This is untested, also probably doesn't work going backwards
void Movement::moveTicks(int ticks, int speed)
{
  //reset encoders (both wheels now have 0 ticks)
  wheelEnc.getCountsAndResetM1();
  wheelEnc.getCountsAndResetM2();
  
  //adjust speed and ticks if we're going backwards
  if(ticks < 0) { speed = - speed; ticks = - ticks; }
  
  //Initialise variables to hold ticks and speed
  int leftTicks = 0;
  int rightTicks = 0;
  int leftSpeed = speed;
  int rightSpeed = speed;
  
  while((abs(leftTicks) < abs(ticks) )|| (abs(rightTicks) < abs(ticks)))
  {
    //adjust motor speed to compensate for error
    int error = tickError();
    if(error > 0) { leftSpeed -= 1; }
    else if(error < 0) { rightSpeed -= 1; }
    else { leftSpeed = speed; rightSpeed = speed; }
    leftSpeed = speed; rightSpeed = speed;
    //send message to motors to adjust speed
    motors->left(leftSpeed);
    motors->right(rightSpeed);
    delay(1); //Delay 1ms so we don't flood the Serial line
    
    //check ticks to see if we've moved far enough
    leftTicks = abs(wheelEnc.getCountsM1());
    rightTicks = abs(wheelEnc.getCountsM2());
  }
}




int Movement::tickError()
{
	int motorOne = abs(wheelEnc.getCountsM1());
	int motorTwo = abs(wheelEnc.getCountsM2());
	return 3*constrain((motorOne - motorTwo), -5, 5)/4;
}

//Length is in mm, rounded to whole number.
//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//WARNING, ERROR, TODO: This is untested
void Movement::moveLength(int length, int speed)
{
  int ticks;
  //convert the length to ticks
  //Simplified formulae: ( Length * 48 [number of ticks in one revolution]) / ( Pi * Wheel Diameter)
  ticks = round((length * 48)/135.1);
  speed = speed;
  moveTicks(ticks,speed);
  
}

//clockwise turning is positive angle, anti-clockwise turning is negative angle.Speed is positive.
//This function doesn't stop motors, you should call motors.stop() if you want to stop after moving the number of ticks
//WARNING, ERROR, TODO: This is untested
  void Movement::rotateAngle(int angle, int speed)
{
	//don't do anything if a rotation of zero is inputted
	if (angle != 0)
	{
		angle=constrain(angle,-180,180);

		//reset encoders (both wheels now have 0 ticks)
		  wheelEnc.getCountsAndResetM1();
		  wheelEnc.getCountsAndResetM2();
		  

		  //Initialise variables to hold ticks and speed
		  int ticks = 0;
		  int leftTicks = 0;
		  int rightTicks = 0;
		  int leftSpeed = speed;
		  int rightSpeed = speed;
		 
		  //convert the angle to ticks
		  //Simplified formulae: (Wheel Base Diameter * 48 [number of ticks in one revolution]) / (Wheel Diameter * 360) * Angle
		  ticks = round((abs(angle) * 96)/215);
		 
		   //adjust speed and angle if turning anti-clockwise
		  if(angle < 0) { leftSpeed = - speed; rightSpeed = speed; }
		   //adjust speed and angle if turning clockwise
		  else if(angle > 0) { leftSpeed = speed; rightSpeed = - speed; }
		  
		  while((abs(leftTicks) < abs(ticks) )|| (abs(rightTicks) < abs(ticks)))
		  {
		//    //adjust motor speed to compensate for error
		//    int error = tickError();
		//    if(error > 0) { leftSpeed -= 1; }
		//    else if(error < 0) { rightSpeed -= 1; }
		//    else { leftSpeed = speed; rightSpeed = speed; }
			
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
