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
#include "MazeImports.h"
#include "GridMap.h"
#include "Routing.h"

GridMap gm;
Routing router(&gm);
Path path;
Point start(1,1);
Point goal(4,7);
Direction startDir = SOUTH;

#define debugStream Serial
void printPathAsListOfPoints(Path * inPath, Point start)
{
  //Print "(x,y)" (where x = start.x, y = start.y)
  debugStream.print("(");
  debugStream.print(start.x);
  debugStream.print(",");
  debugStream.print(start.y);
  debugStream.print(")");
  
  const int CNTR_START_OFFSET = 5,//Number of chars in "(n,n)" could be "(nn,nn)" though! @TODO
            CNTR_INC = 9,//Number of chars in " -> (n,n)" could be " -> (nn,nn)" though! @TODO
            MAX_LINE_WIDTH = 57;//Currently only an approx line width due to the 2 consts above being incorrect.
  int counter = CNTR_START_OFFSET;
  
  for(int ii = 0; ii < inPath->length; ii++)
  {
    counter += CNTR_INC;
    if(counter > MAX_LINE_WIDTH)
    {
      debugStream.println();
      counter = 0;
    }
    
    //print " -> (x,y)" (where x = inPath->path[ii].x and y = inPath->path[ii].y) = the coords of the next point in the path
    debugStream.print(" -> ");
    debugStream.print("(");
    debugStream.print(inPath->path[ii].x);
    debugStream.print(",");
    debugStream.print(inPath->path[ii].y);
    debugStream.print(")");
  }
}

#define PUSHPIN 3

void setup()
{  
  Serial.begin(9600);
  Serial.println("started");

  router.generateRoute(start, goal, startDir, &path);
  Serial.println("here");
  printPathAsListOfPoints(&path, start);
}

void loop()
{

}
