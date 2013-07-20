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

void Movement::moveTillPoint(int speed)
{
	moveOffCross(speed);	
	while(!onCross())
	{
		motors->both(speed, ls->error());
	}
}

void Movement::moveOffCross(int speed)
{
	while(onCross())
	{
		motors->both(speed, tickError());
	}
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
  
  while((leftTicks < ticks )|| (rightTicks < ticks))
  {
    //adjust motor speed to compensate for error
    int error = tickError();
    if(error > 0) { leftSpeed -= 1; }
    else if(error < 0) { rightSpeed -= 1; }
    else { leftSpeed = speed; rightSpeed = speed; }
    
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
	return constrain((motorOne - motorTwo), -5, 5);
}

