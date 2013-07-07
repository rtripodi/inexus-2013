#include "Robot.h"

int foreward[5] = {1,2,3,4,5};

/*
You must run setupSerialComms() for anything to work!
*/
Robot::Robot(Motor * inMotors)
{
  motors = inMotors;
  wheelEnc.init(8,9,11,10);
  lastError = 0;
  errorSum = 0;
}

//stup incase we ever need setup code
void Robot::setup(){}

void Robot::sdebug()
{
  foreward[0] = getTicks();
  foreward[1] = errorTickAdjustment();
  foreward[2] = getLeftEncoderCounts();
  foreward[3] = getRightEncoderCounts();
  foreward[4] = abs(getLeftEncoderCounts() - getRightEncoderCounts());
    for (int ii = 0; ii < 5; ++ii)
  {
    Serial.print(foreward[ii]);
    Serial.print("\t");
    Serial.print("\t");
  }
  Serial.println();
}

void Robot::resetEncoders()
{
  wheelEnc.getCountsAndResetM1();
  wheelEnc.getCountsAndResetM2();
}

int Robot::getRightEncoderCounts(){
  return wheelEnc.getCountsM2();
}

int Robot::getLeftEncoderCounts(){
  return wheelEnc.getCountsM1();
}

//returns the average of the ticks on both motors
int Robot::getTicks()
{
  int motorOne = abs(wheelEnc.getCountsM1());
  int motorTwo = abs(wheelEnc.getCountsM2());
  return (motorOne + motorTwo) / 2;
}


void Robot::moveTicks(int ticks, int motorSpeed)
{
  wheelEnc.getCountsAndResetM1();
  wheelEnc.getCountsAndResetM2();
  
  if(ticks < 0)
  {
    motorSpeed = - motorSpeed;
    ticks = ticks + 10;    
  }  
  else
  {
    ticks = ticks - 10;
  }
  
  int error, motorOne, motorTwo;
  error=0;                                                //Adjust this to see affects
  //error = constrain(abs(motorOne)-abs(motorTwo),-5,5);      // see if constraining the error or nullifying the error is what we need
  resetEncoders();
  Serial.println(error);
  sdebug();
  do
  {
    delayMicroseconds(500);
    motorOne = abs(wheelEnc.getCountsM1());
    motorTwo = abs(wheelEnc.getCountsM2());
    //error = errorTickAdjustment();
    error = constrain(abs(motorOne)-abs(motorTwo),-5,5);
    if((error > 1) && (abs(motorTwo) < abs(ticks)))
    {
      motors->left(0);
      motors->right(motorSpeed);
      sdebug();
  Serial.println(error);
    }
    else if((error < -1) && (abs(motorOne) < abs(ticks)))
    {
      motors->right(0);
      motors->left(motorSpeed);
      sdebug();
  Serial.println(error);
     }
   else
    {
      motors->both(motorSpeed, error);
      sdebug();
  Serial.println(error);
    }  
     sdebug();
  Serial.println(error);
  } while((motorTwo < abs(ticks)) && (motorOne < abs(ticks)));
  sdebug();
}




// moveTicks moves the number of ticks given.
// A positive ticks number will go forward, a negative ticks number 
// will go backwards.
// We do not stop after hitting the number of ticks. Call motors->stop().
//void Robot::moveTicks(int ticks, int motorSpeed)
//{
//  int error, motorOne, motorTwo;
//
//  wheelEnc.getCountsAndResetM1();
//  wheelEnc.getCountsAndResetM2();
//
//  resetEncoders();
//  motorOne = abs(wheelEnc.getCountsM1());
//  motorTwo = abs(wheelEnc.getCountsM2());
//
//
//  if(ticks < 0)
//    motorSpeed = - motorSpeed;
//  
//Serial.println(ticks);
//  while((abs(wheelEnc.getCountsM1()) < abs(ticks)) || (abs(wheelEnc.getCountsM2()) < abs(ticks)))
//  {
//       if((abs(wheelEnc.getCountsM1())  > abs(ticks)) || (abs(wheelEnc.getCountsM2())  > abs(ticks)))
//      {
//        motors->stop();
//      break;
//      }   
//    //Serial.println(ticks);
//    motorOne = abs(wheelEnc.getCountsM1());
//    motorTwo = abs(wheelEnc.getCountsM2());
//    error = errorTickAdjustment();
//    motors->both(motorSpeed, error);
//    //sdebug();
//    if((wheelEnc.getCountsM1()  > ticks) || (wheelEnc.getCountsM2()  > ticks))
//      {
//        motors->stop();
//      break;
//      }  
//    
//    //delayMicroseconds(500);
//   }
//    delay(500);
//    motors->stop();
//    sdebug();
//    Serial.println("corection");    
//    Serial.print(abs(wheelEnc.getCountsM1()));
//    Serial.print("\t");
//    Serial.print(abs(wheelEnc.getCountsM2()));
//    Serial.print("\t");
//    Serial.print(abs(ticks));
//    Serial.println();
//
//          while(abs(ticks) > abs(wheelEnc.getCountsM1()))
//          {
//             motors->right(0);
//             motors->left(motorSpeed);  
//             delayMicroseconds(500);
//             Serial.print("one");
//             Serial.print("\t");
//             Serial.print( wheelEnc.getCountsM1());
//             Serial.println();
//
//          }
//
//          while(abs(ticks) > abs(wheelEnc.getCountsM2())) 
//           {
//             motors->left(0);
//             motors->right(motorSpeed);
//             delayMicroseconds(500);
//             Serial.print("two");
//             Serial.print("\t");
//             Serial.print( wheelEnc.getCountsM2());
//             Serial.println();
//           } 
//         
//}

//Rotates the number of ticks specified
//if it doesn't stop turning it means one of the encoders aren't working
//try fiddling with some wires or something...
void Robot::rotateTicks(int ticks, int motorSpeed)
{
  resetEncoders();

  if(ticks < 0)
    motorSpeed = - motorSpeed;

  int minSpeed = 28;
  int lowSpeed = motorSpeed / 2;
  if((lowSpeed > 0) && (lowSpeed < minSpeed))
    lowSpeed = minSpeed;
  else if((lowSpeed < 0) && (lowSpeed > -minSpeed))
    lowSpeed = -minSpeed;
 
  int error, motorOne, motorTwo;
  do
  {    
    motorOne = abs(wheelEnc.getCountsM1());
    motorTwo = abs(wheelEnc.getCountsM2());

    error = errorTickAdjustment();
    if(error < 0)
    {
      motors->left( - motorSpeed);
      motors->right(lowSpeed);
    }
    else if(error > 0)
    {
      motors->right(motorSpeed);
      motors->left(- lowSpeed);
    }
    else
    {
      motors->right(motorSpeed);
      motors->left( - motorSpeed);
    }
    delay(1);   
  } while((motorTwo < abs(ticks)) || (motorOne < abs(ticks)));
  motors->stop();
  resetEncoders();
}

// errorAdjustment returns a value used to adjust the movement speed
// of wheels using both errorTickAdjustment and errorSensorAdjustment.
// Returns between -20 and 20
int Robot::errorAdjustment(bool onlyTick)
{
  //now the errorSensorAdjustment() is fixed we might use it for diagonals, we'll see
  int tickError = errorTickAdjustment();
  //int sensorError = errorSensorAdjustment();

  int error = 0;

  // If we only want to use the ticks for error adjustment (say for turning)
  if (onlyTick)
    error = 0;//tickError;
  else
    error = 0;//tickError;
  return error;
}


// errorTickAdjustment returns a value used to adjust the movement speed
// of wheels based on the number of ticks seen by each motor.
// It will be between -5 and 5
int Robot::errorTickAdjustment()
{
  int motorOne = abs(wheelEnc.getCountsM1());
  int motorTwo = abs(wheelEnc.getCountsM2());
  int error = motorOne - motorTwo;

  return constrain(error,-5,5);
}

// errorSensorAdjustment returns a value used to adjust the movement speed
// of wheels based on the position of the sensors over the line.
// It will be between -15 and 15 *****REDACTED*******

