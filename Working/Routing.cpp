#include "Routing.h"

Routing::Routing(){}

Routing::Routing(Maze *inMaze)
{
  maze = inMaze;
  nodeList = NodeList(GRID_MAX_X+1, GRID_MAX_Y+1);
}

 //The route will be generated using the A* pathfinding algorithm
 //Returns a path that only contains the startPoint if no path can be found
 //If not, it contains a path up to and including the goal but not including the start point
 //(It will not generate a path through unknown or unpassable terrain)
 //
 //THE CODE EXPLAINED:
 //
 //We then loop continuously, every time we loop:
 //  We pick the the point with the lowest cost on the OPEN list
 //    If there are no more points on the OPEN list then we stop looping
 //  Put the point on the CLOSE list
 //  We call this point "current"
 //  Examine every point adjacent to it, for each of those points:
 //    If it is OPEN we check if we can get to it faster from "current" and if so chenge current to the pathParent of the point
 //    If it is NONE we make "current" the pathParent of the point
 //	 If it is GOAL we set atGoal to true
 //	 If it is CLOSED we've already considered it completely and so do nothing
 //Finish if there are no points left that cost less to get to than the goal
void Routing::generateRoute(Point start, Point goal, Direction currDir, Path * path)
{
  generateRouteSetup(start, goal, currDir);
  bool atGoal = false;        //  atGoal is set to true if the goal point was considered while looping
  Point current = start;
  while(maze->contains(current) && !(atGoal && (nodeList.calcCost(current) > nodeList.calcCost(goal))))// loop continuously, it will break if we run out of points to investigate or if we find the goal
  {
    //Check every adjacent node to the current one, allowing diagonal movement
    for (int xDelta = -1; xDelta <= 1; xDelta++)
    {
      for (int yDelta = -1; yDelta <= 1; yDelta++)
      {
	Point adjacentPt(current.x + xDelta, current.y + yDelta); //This will hold every new point adjacent to current that we investigate
        if(maze->joined(current, adjacentPt))//if current = adjacentPt or either point is outside the maze maze->joined() also returns false
        {
	  atGoal = classifyPoint(adjacentPt, current, nodeList.getParent(current), goal) || atGoal;
        }//if
      }//for
    }//for
    nodeList.close(current); //We have finished with this point so we close it
    current = nodeList.findNodeWithLowestSum();//get new point, returns {-1,-1} if no OPEN points left
  }//while
  
  makePath(path, atGoal, goal, start);
}

void Routing::generateRouteSetup(Point start, Point goal, Direction currDir)
{
  Point pointBehindStart = pointBehind(start, currDir);
  if( ! maze->contains(pointBehindStart))
    pointBehindStart = start;
  nodeList.resetList(goal, start, pointBehindStart, calcHeuristic(start,goal));
}
  
void Routing::makePath(Path* path, bool atGoal, Point goal, Point start)
{
  Point endPoint = atGoal ? goal : start;

  path->length = findPathLength(endPoint, start);
  //create path
  Point tempPoint = endPoint;
  if(path->length > 0)
  {
    for (int ii = path->length - 1; ii >= 0; ii--)
    {
      path->path[ii] = tempPoint;
      tempPoint = nodeList.getParent(tempPoint);
    }
  }
}
  
bool Routing::classifyPoint(Point adjacentPt, Point current, Point parentOfCurrent, Point goal)
{
  unsigned int adjacentPtCost = calcCost(parentOfCurrent, current, adjacentPt) + nodeList.calcCost(current);
  //Make it OPEN if it is NONE; if it is OPEN check if we can get there faster via the current node, if so we change the parent
  switch (nodeList.getListType(adjacentPt))
  {
    case NodeList::OPEN:
      nodeList.update(adjacentPt, current, adjacentPtCost);
      break;
    case NodeList::NONE:
      if( ! maze->isPassable(adjacentPt))
        nodeList.close(adjacentPt);
      else
	nodeList.update(adjacentPt, current, adjacentPtCost, calcHeuristic(adjacentPt, goal));
      break;
    case NodeList::GOAL:
      nodeList.update(adjacentPt, current, adjacentPtCost);
      return true;
  }
  return false;
}
		  
//Only parents are stored in Nodes so we have to work backwards from the goal to find the length of the path
//This is used to set the array length of the Path we will return from generateRoute
int Routing::findPathLength(Point endPoint, Point start)
{ 
  Point tempPoint = endPoint; //(we start from the end and work backwards as we store parents but not children
  int pathLength = 0;
  while (tempPoint != start)
  {
    tempPoint = nodeList.getParent(tempPoint);
    pathLength++;
  }
  return pathLength;
}

//Calculates the cost of moving from Point "thru" to Point "to", uses Point "from" to determine the cost of turning
//Takes into account distance the robot has to turn, the distance between points and adds a large cost if the points are
//not connected (according to Maze).
int Routing::calcCost(Point from, Point thru, Point to)
{
  const int DIAGONAL_COST = 7,      //  Comes from multiplying "an isosceles right angle triangle with hypotenuse = 1" by 7
            ORTHOGONAL_COST = 5,  //  Comes from multiplying "an isosceles right angle triangle with hypotenuse = 1" by 7
            UNPASSABLE_COST = 200; // This cost is for when the user forces a path to be generated and the cost of passing an UNPASSABLE node must be defined
  
  int cost = turnCost(from, thru, to); 
  
  if( ! maze->joined(thru,to))
    cost += UNPASSABLE_COST;
  
  int deltaY = abs(thru.y - to.y);
  int deltaX = abs(thru.x - to.x);
  
  if((deltaX != 0) && (deltaY != 0))
    return cost + DIAGONAL_COST;
  else
    return cost + ORTHOGONAL_COST;
}

//Returns a cost proportional to how far the robot has to turn from "thru" to "to", uses "from" to work out
//the direction the robot is heading in.
int Routing::turnCost(Point from, Point thru, Point to)
{
  const int NO_TURN_COST = 0,
            TURN_45_COST = 15,
	    TURN_90_COST = 15,
	    TURN_135_COST = 45,
	    TURN_180_COST = 75;

  if(from == thru) //this may occur if no valid "from" can be passed to this function
    return NO_TURN_COST;
	
  int xDir = thru.x - from.x;
  int yDir = thru.y - from.y;
  int xChange = to.x - thru.x;
  int yChange = to.y - thru.y;
  
  if((xDir == xChange) && (yDir == yChange))
    return NO_TURN_COST;
  else if(((xDir == 0) && (yDir == -yChange)) || ((yDir == 0) && (xDir == -xChange)))
    return TURN_135_COST;
  else if(((xDir == xChange) && (yDir != yChange)) || ((yDir == yChange) && (xDir != xChange)))
    return TURN_45_COST;
  else if((xDir == - xChange) && (yDir == - yChange))
    return TURN_180_COST;
  else
    return TURN_90_COST;
}

// Calculates the heuristic between two points.
// Gives a cost roughly proportional to the distance between "one" and "two"
// If cost is greater than 255 then it will wrap around, need to fix this.
byte Routing::calcHeuristic(Point one, Point two)
{
  // The distance in the x dimension plus the distance in the y dimension. (Manhattan Distance)
  // The pythagoarean distance provides more accuracy but at the cost of speed
  return (byte) (abs(one.x - two.x) + abs(one.y - two.y));//consistent and admissible IF cost of travel between 2 nodes is always >= 2
  //return (byte) sqrt(((one.x - two.x)*(one.x - two.x) + (one.y - two.y)*(one.y - two.y)));//pythagoarean distance = admissible and consistent
}

Point Routing::pointBehind(Point pt, Direction dir)
{
  switch(dir)
  {
    case NORTH:
      pt.y -= 1;
      break;
    case SOUTH:
      pt.y += 1;
      break;
    case EAST:
      pt.x -= 1;
      break;
    case WEST:
      pt.x += 1;
      break;
    case NEAST:
      pt.y -= 1;
      pt.x -= 1;
      break;
    case NWEST:
      pt.y -= 1;
      pt.x += 1;
      break;
    case SEAST:
      pt.y += 1;
      pt.x += 1;
      break;
    case SWEST:
      pt.y += 1;
      pt.x += 1;
      break;
  }
  return pt;
}
