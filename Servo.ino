// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.

#include <Servo.h> 
 
class Claw
{
  public:
    void setup(int leftServoPin, int rightServoPin)
    {
      leftServo.attach(leftServoPin);
      rightServo.attach(rightServoPin);
    }
    
    void open()
    {
      leftServo.write(95);
      rightServo.write(44+45);
    }
    
    void close()
    {
      leftServo.write(140);
      rightServo.write(44);
    }
    
    void shut()
    {
    leftServo.write(189);
    rightServo.write(-1);
    }
    
  private:
    Servo leftServo, rightServo;
};

Claw myClaw;


void setup() 
{
  myClaw.setup(10, 9);
  
   int count = 0;
 
 while(count < 1) 
{ 
  myClaw.shut();
  delay(1000);
  myClaw.close();
  delay(1000);
  myClaw.open();
  delay(1000);
  count++;
} 
} 
 
void loop() {}
