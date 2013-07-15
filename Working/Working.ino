//#include <SoftwareSerial.h>  //For colour sensor
#include "Config.h"
#include <PololuWheelEncoders.h>
#include <PololuQTRSensors.h>
#include "Claw.h"
#include "IR.h"
#include "LineSensors.h"
//#include "Colour.h"
#include <Servo.h>
#include "Movement.h"
#include "Routing.h"

Motor motors;

Claw claw(CLAW_LEFT_PIN, CLAW_RIGHT_PIN);
void clawTestSetup();
void clawTest();

IR ir(1, IR::shortRange);
void irTestSetup();
void irTest();

LineSensors ls;
void lineFollowDemoSetup();
void lineFollowDemo();

Movement mover(&motors, &ls);
void testMover()
{
  for(int ii = 0; ii < 4; ++ii)
    mover.moveTillPoint(80);
  motors.stop();
  delay(100000);
}

#define PUSHPIN 3

void setup()
{  
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  motors.setup();
  digitalWrite(LED_PIN,HIGH);
  pinMode(PUSHPIN, INPUT);
  digitalWrite(PUSHPIN, HIGH);
  while(digitalRead(PUSHPIN) == HIGH)
  {
    delay(500); 
  }
  digitalWrite(LED_PIN, LOW);
  digitalWrite(PUSHPIN, HIGH);
  lineFollowDemoSetup();
  clawTestSetup();
}

void loop()
{
  testMover();
}

void clawTestSetup()
{
  claw.setup();
  claw.shut();
}

void clawTest()
{
  delay(4000);
  claw.open();
  delay(4000);
  claw.close();
  delay(4000);
  claw.shut();
}

void irTestSetup()
{
  ir.setup();
  Serial.println("Raw\t\tMillimetres");
}

void irTest()
{
  Serial.print(analogRead(1));
  Serial.print("\t\t");
  Serial.println(ir.getDist());
  delay(100);
}

void lineFollowDemoSetup()
{
  ls.calibrate();
  delay(4000);
  ls.calibrate();
}

//Note, this is NOT the best way to do it.  Just a quick example of how to use the class.
void lineFollowDemo()
{
  LineSensor_ColourValues leftWhite[8] = {NUL,NUL,WHT,NUL,NUL,NUL,NUL,NUL};
  LineSensor_ColourValues rightWhite[8] = {NUL,NUL,NUL,NUL,NUL,WHT,NUL,NUL};
  LineSensor_ColourValues allBlack[8] = {BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK};
  ls.readCalibrated();
  if(ls.see(leftWhite)){
    motors.left(127);
    motors.right(0);
  }
  if(ls.see(rightWhite)){
    motors.right(127);
    motors.left(0);
  }
  if(ls.see(allBlack)){
    motors.stop();
  }
}
