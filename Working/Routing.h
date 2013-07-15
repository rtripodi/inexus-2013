/*
 *TODO:
   Give pathfinder the ability to go through unknown and unpassable terrain (via option) thus if no path can be found a "next best" can be worked out

 
 * The class has a 1 x (256 x 5) byte array (nodeList) and will create a 256 x 4 byte array (path) (about 2.3kB, almost certainly less than 3kB)
 * It is possible that the creation of a Path (being 1kB) might overflow the stack if nested.  Should probably be a private class field.
 * The nodeList (formerly openClosedList) used to be 1 x (256 x 12) bytes so there could be some bad logic somewhere, be careful
 */

#ifndef Routing_h
#define Routing_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

struct Point
{
  int x;
  int y;

  Point(){}
  
  Point(int inX, int inY) { x = inX; y = inY; }

  bool operator==(const Point &a){
    return ((a.x == x) && (a.y == y));
  }

  bool operator!=(const Point &a){
    return ((a.x != x) || (a.y != y));
  }
  
  Point & operator+=(const Point &a) 
  {
    x += a.x;
	y += a.y;
    return *this;
  }
  
  //prints:
  //name = (x,y)
  void debug(char name[])
  {
    Serial.print(name);
    Serial.print(" = (");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.println(")");
  }
  
  //Takes coordinates in the form of 2 ints and transforms them into a byte for sending between robots
  byte encodeCoordinates() const
  {
    byte x_nibble = ((byte) x) << 4;
    byte y_nibble = (byte) y;
    return x_nibble + y_nibble;
  }

  //Receives a byte from encodeCoordinates and transforms it to a Point
  void decodeCoordinates(byte coords)
  {
    x = (int) ((coords & 0xF0) >> 4);
    y = (int) (coords & 0x0F);
  }
};
  
/***************************************************
*START: Required Stuff to make Routing work
***************************************************/
// Path structure is here to return the path that the bot should take.
struct Path
{
  int length;
  Point path[256];
  
  Path(){
    length = 0;
  }
  
  Path(int inLength){
    length = inLength;
  }
  
  Path operator=(Path inPath)
  {
    length = inPath.length;
    for(int ii = 0; ii < length; ii++)
    {
      path[ii] = inPath.path[ii];
    }
  };

  Point last(){
    if(length > 0)
      return path[length - 1];
    else
    {
      Point temp = Point(-1,-1);
      return temp;
    }
  }
  
  bool pointInPath(Point p)
  {
  for(int ii = 0; ii < length; ii++)
  {
    if(p == path[ii])
      return true;
  }
  return false;
  }
};

#define MAZE_X_RANGE 16
#define MAZE_Y_RANGE 16

enum Direction {NORTH, NEAST, EAST, SEAST, SOUTH, SWEST, WEST, NWEST};

class Maze
{
	public:
		//if p1 == p2 or either point is outside the maze  then pointsJoined() also returns false
		virtual bool joined(Point p1, Point p2) = 0;
		virtual bool inMaze(Point pt) = 0;
		virtual bool isPassable(Point pt) = 0;
};

/***************************************************
*END: Required Stuff to make Routing work
***************************************************/

//#define MINIMISE_MEMORY
#if MAZE_X_RANGE <= 16 && MAZE_Y_RANGE <= 16 && defined(MINIMISE_MEMORY)
  #define ENCODE_COORDS
#endif

// Node is to represent both the open list and closed list for generateRoute().
// We have created it like this to save memory.
// read generateRoute() comments
struct Node
{
  private:
    #if defined(ENCODE_COORDS)
      byte encodedParent;
    #else
      Point parentPt;
    #endif
  public:
    static const byte OPEN = 0x00,
                      CLOSED = 0x01,
                      NONE = 0x02,
		      GOAL = 0x04;
  
    int sum; // F
    byte heuristic; // H
    byte listType;
    
    Node()
    {   
      const Point DEFAULT_PARENT(0,0);
      parent(DEFAULT_PARENT);
      sum = -1;
      heuristic = 0x00;
      listType = NONE;
    }
    
    Point parent()
    {
      #if defined(ENCODE_COORDS)
        Point parentPt;
        parent.decodeCoordinates(encodedParent);
      #endif
      return parentPt;
    }
    
    void parent(Point newParent)
    {
      #if defined(ENCODE_COORDS)
        encodedParent = newParent.encodeCoordinates();
      #else
        parentPt = newParent;
      #endif
    }
    
    void set(Point inParent, int inSum, byte inHeuristic, byte inListType)
    {
      parent(inParent);
      sum = inSum;
      heuristic = inHeuristic;
      listType = inListType;
    }
    
    void updateNode(Point current, unsigned int movementCost, byte newHeuristic = 0x00)
    {
      if(listType == OPEN)
      {
        if (movementCost < (sum - (int) heuristic)){// Store parent and cost to openClosedList array if the new way is quicker. (costs less to move)
  	  set(current, movementCost + (int) heuristic, heuristic, OPEN);
        }
        else{
          set(parent(), sum, heuristic, OPEN);
        }
      }
      else if(listType == NONE){
        set(current, movementCost + (int) newHeuristic, newHeuristic, OPEN);
      }
    }
    
    unsigned int calcCost() { return sum - (int) heuristic; }
};

struct NodeList
{
	private:
	  Node node[MAZE_X_RANGE][MAZE_Y_RANGE];
	public:
	  int xLength;
	  int yLength;
  
	  NodeList(){}
  
	  NodeList(int inXLength, int inYLength)
	  {
		xLength = inXLength;
		yLength = inYLength;
	  }
  
	  void resetList(Point goal, Point start, Point pointBehindStart, byte startHeuristic)
	  {
		for (int x = 0; x < xLength; x++)
		{
		  for (int y = 0; y < yLength; y++)
		  {
			node[x][y] = Node();
		  }
		}
                const Point DEFAULT_PARENT(0,0);
                const byte GOAL_COST = 0x00;
                const int GOAL_SUM = 0;
                node[goal.x][goal.y].set(DEFAULT_PARENT, GOAL_SUM, GOAL_COST, Node::GOAL);
		node[start.x][start.y].set(pointBehindStart, startHeuristic, startHeuristic, Node::OPEN);
	  }
  
	  void close(Point pt)
	  {
		node[pt.x][pt.y].listType = Node::CLOSED;
	  }
  
	  void update(Point pt, Point parent, unsigned int cost, byte heuristic)
	  {
		node[pt.x][pt.y].updateNode(parent, cost, heuristic);
	  }

	  void update(Point pt, Point parent, unsigned int cost)
	  {
		node[pt.x][pt.y].updateNode(parent, cost);
	  }
 
	  //Inefficient with time, efficient with space
	  Point findNodeWithLowestSum()
	  {
		Point current;
		bool foundOneOpenPt = false;
		for (int x = 0; x < xLength; x++){// Search through the open list to get the lowest sum node. Set it to current.
		  for (int y = 0; y < yLength; y++){
        	    if (node[x][y].listType == Node::OPEN){
       		      if (!foundOneOpenPt){
   			current = Point(x,y);
        		foundOneOpenPt = true;
        	      }
        	      if (node[x][y].sum < node[current.x][current.y].sum){
        	        current = Point(x,y);
        	      }
        	    }//if
		  }//for
		}//for
		if( ! foundOneOpenPt){
		  current.x = MAZE_X_RANGE + 1;
		  current.y = MAZE_Y_RANGE + 1;
		}
		return current;
	  }
  
	  void setNode(Point p, Node newNode){
		node[p.x][p.y] = newNode;
	  } 

	  unsigned int calcCost(Point pt) { return node[pt.x][pt.y].calcCost(); }
  
	  Point getParent(Point pt) { return node[pt.x][pt.y].parent(); }

	  byte getListType(Point pt) { return node[pt.x][pt.y].listType; }

	  void setParent(Point pt, Point newParent) { node[pt.x][pt.y].parent(newParent); }
};
  
class Routing
{
  public:
    Maze *maze;
    Routing();
    Routing(Maze *maze);
    void generateRoute(Point start, Point goal, Direction currDir, Path * path);
	
  private:
    NodeList nodeList;
    bool classifyPoint(Point adjacentPt, Point current, Point parentOfCurrent, Point goal);
    void generateRouteSetup(Point start, Point goal, Direction currDir);
    void makePath(Path* path, bool atGoal, Point goal, Point start);
    int findPathLength(Point endPoint, Point start);

    int calcCost(Point from, Point thru, Point to);
    int turnCost(Point from, Point thru, Point to);
    byte calcHeuristic(Point one, Point two);
    Point pointBehind(Point pt, Direction dir);
};
#endif
