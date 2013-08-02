#ifndef MazeImports_h
#define MazeImports_h

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

//TODO: Diagonals values should be between cardinal values, need to rewrite turn finding function for this
enum Direction {NORTH = 1 , EAST = 2, SOUTH = 3, WEST = 4, NEAST = 5 , SEAST = 6, SWEST = 7, NWEST = 8};

class Maze
{
	public:
		//if p1 == p2 or either point is outside the maze  then pointsJoined() also returns false
		virtual bool joined(Point p1, Point p2) = 0;
		virtual bool contains(Point pt) = 0;
		virtual bool isPassable(Point pt) = 0;
};

#endif
