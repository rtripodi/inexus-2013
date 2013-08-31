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

#include "GridImports.h"
#include "Direction.h"
#include "GridMap.h"

//#define MINIMISE_MEMORY
#if MAZE_X_RANGE <= 16 && MAZE_Y_RANGE <= 16 && defined(MINIMISE_MEMORY)
  #define ENCODE_COORDS
#endif

struct Node
{
  private:
    #if defined(ENCODE_COORDS)
      byte encodedParent;
    #else
      Point parentPt;
    #endif
  public:
    int sum; // F
    byte heuristic; // H
    byte listType;
    
    Node() {}
    
    Node(byte inListType){ listType = inListType; }
    
    Point parent(){
      #if defined(ENCODE_COORDS)
        Point parentPt;
        parent.decodeCoordinates(encodedParent);
      #endif
      return parentPt;
    }
    
    void parent(Point newParent){
      #if defined(ENCODE_COORDS)
        encodedParent = newParent.encodeCoordinates();
      #else
        parentPt = newParent;
      #endif
    }
    
    void set(Point inParent, int inSum, byte inHeuristic, byte inListType){
      parent(inParent);
      sum = inSum;
      heuristic = inHeuristic;
      listType = inListType;
    }
    
    unsigned int calcCost() { return sum - (int) heuristic; }
};

struct NodeList
{
	private:
	  Node node[GRID_MAX_X+1][GRID_MAX_Y+1];
	public:
          static const byte NONE = 0x00,
                            CLOSED = 0x01,
                            OPEN = 0x02,
    		            GOAL = 0x04;
	  int xLength;
	  int yLength;
  
	  NodeList(){}
  
	  NodeList(int inXLength, int inYLength){
		xLength = inXLength;
		yLength = inYLength;
	  }
  
	  void resetList(Point goal, Point start, Point pointBehindStart, byte startHeuristic){
		for (int x = 0; x < xLength; x++){
		  for (int y = 0; y < yLength; y++){
			node[x][y] = Node(NONE);
		  }
		}
                const Point DEFAULT_PARENT(0,0);
                const byte GOAL_COST = 0x00;
                const int GOAL_SUM = 0;
                node[goal.x][goal.y].set(DEFAULT_PARENT, GOAL_SUM, GOAL_COST, GOAL);
		node[start.x][start.y].set(pointBehindStart, startHeuristic, startHeuristic, OPEN);
	  }
  
	  void close(Point pt){
		node[pt.x][pt.y].listType = CLOSED;
	  }
  
         void update(Point pt, Point parent, unsigned int cost, byte heuristic = 0x00){
           if((node[pt.x][pt.y].listType == GOAL) && (node[pt.x][pt.y].sum == 0)) { node[pt.x][pt.y].set(parent, cost, heuristic, GOAL); }
           if((node[pt.x][pt.y].listType == OPEN) || (node[pt.x][pt.y].listType == GOAL)){
             if (cost < node[pt.x][pt.y].calcCost()){// Store parent and cost to openClosedList array if the new way is quicker. (costs less to move)
      	       node[pt.x][pt.y].set(parent, cost + (int) heuristic, heuristic, OPEN);
            }
          }
          else if(node[pt.x][pt.y].listType == NONE){
            node[pt.x][pt.y].set(parent, cost + (int) heuristic, heuristic, OPEN);
          }
        }
 
	  //Inefficient with time, efficient with space
	  Point findNodeWithLowestSum(){
		Point current;
		bool foundOneOpenPt = false;
		for (int x = 0; x < xLength; x++){// Search through the open list to get the lowest sum node. Set it to current.
		  for (int y = 0; y < yLength; y++){
        	    if (node[x][y].listType == OPEN){
       		      if (( ! foundOneOpenPt) || (node[x][y].sum < node[current.x][current.y].sum)){
   			current = Point(x,y);
        		foundOneOpenPt = true;
        	      }
        	    }//if
		  }//for
		}//for
		return foundOneOpenPt ? current : Point(GRID_MAX_X + 1, GRID_MAX_Y + 1);
	  }
  
	  //Following functions hide Node class and array implementation from higher level code
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
    void generateRoute(Point start, Point goal, CarDir currDir, Path * path);
	
  private:
    NodeList nodeList;
    bool classifyPoint(Point adjacentPt, Point current, Point parentOfCurrent, Point goal);
    void generateRouteSetup(Point start, Point goal, CarDir currDir);
    void makePath(Path* path, bool atGoal, Point goal, Point start);
    int findPathLength(Point endPoint, Point start);

    int calcCost(Point from, Point thru, Point to);
    int turnCost(Point from, Point thru, Point to);
    byte calcHeuristic(Point one, Point two);
    Point pointBehind(Point pt, CarDir dir);
};
#endif
