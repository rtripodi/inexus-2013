/********************************************
 *  MISC Shield Pins
 ********************************************/
//Most prominant LED on shield, used for signalling info
#define LED_PIN 2

//The left-most (NOT CENTRE-MOST!) push button on the shield
//Used for sending signals to the robot (ie, start moving)
#define PUSH_PIN 3

//Named for convenience, a XBee is connected to Serial3 (used for wireless comunication)
#define XBee Serial3

/********************************************
 *  DIP Switches                             
 *(The box at the front)
 ********************************************/
 //Those buttons labeled UNASSIGNED_SWITCH_n should be renamed
 //when a purpose has been assigned to them
#define OPENDAY_MODE_SWITCH 52 //Closest switch to the edge of the shield
#define OPENDAY_IR_SWITCH 50
#define OPENDAY_ALT_SWITCH 48
#define UNASSIGNED_SWITCH_4 46
#define UNASSIGNED_SWITCH_5 44
#define UNASSIGNED_SWITCH_6 42

//Competitions usually allow the robot to be restarted
//Use this pin to tell the robot that it is being restarted
//(This normally means it's being moved back to the start point
//and should wait to be told to start moving again)
#define RESTART_PIN 40

//Use this switch to turn debugging on and off
#define DEBUG_PIN 38 //Closest switch to the centre of the shield

/********************************************
 *  QTR Sensor                              *
 ********************************************/
//Pins used for input from QTR sensors
#define QTR_SENSOR_PINS_LIST ((unsigned char[]) {22,23,24,25,26,27,28,29})

//Number of input pins for QTR sensors
#define QTR_NUM_PINS (8)

//At 16MHz you divide this number by 2 to get the number of microseconds.
//if QTR_TIMEOUT = 4000 then it takes a maximum of 2000us for the class
//to read the sensors (Advice is to use between 1000-3000us.)
#define QTR_TIMEOUT (4000)

//Pin to control infrared emitter on sensors
#define QTR_EMITTER_PIN (39)

//Minimum reading of CALIBRATED sensors for it to be declared black
//Anything above or equal to MIN_BLACK is black, anything below MIN_BLACK is white
#define MIN_BLACK (250)

//Distance between sensor 0 and sensor 7 in mm, used to find out the lines position.
#define QTR_WIDTH (80)
/********************************************
 *  Servos / Claw                           *
 ********************************************/
//Pins used for communication with servos
#define CLAW_LEFT_PIN (6)
#define CLAW_RIGHT_PIN (7)
 
//Values for each servo to allow claw to be in open state
#define CLAW_OPEN_LEFT (80)
#define CLAW_OPEN_RIGHT (100)

//Values for each servo to allow claw to be in closed state
#define CLAW_CLOSE_LEFT (CLAW_OPEN_LEFT + 60 - 20)
#define CLAW_CLOSE_RIGHT (CLAW_OPEN_RIGHT - 60 + 20)

//Values for each servo to allow claw to be in shut state
#define CLAW_SHUT_LEFT (CLAW_OPEN_LEFT + 80)
#define CLAW_SHUT_RIGHT (CLAW_OPEN_RIGHT - 80)

/********************************************
 *  IR Sensors                              *
 ********************************************/
//Pins used for communication with IR sensors
#define IR_FRONT_PIN (4)
#define IR_RIGHT_PIN (1)
#define IR_BACK_PIN (0)
#define IR_LEFT_PIN (2)

//IR sensor offset from midpoint in mm
#define IR_FRONT_OFFSET (5)
#define IR_RIGHT_OFFSET (70)
#define IR_BACK_OFFSET (70)
#define IR_LEFT_OFFSET (60)

//Amount of IR reads to determine mean value
#define IR_ITERATIONS (10)

/********************************************
 *  Motor                                   *
 ********************************************/
//Serial used for Motor Controller
#define MotorControl Serial2

#define MOTOR_BAUD_RATE (38400)
#define MOTOR_ESTABLISH_BAUD (0xAA)


#define M1_CLOCKWISE		0x8A
#define M1_ANTI_CLOCKWISE	0x88
#define M2_CLOCKWISE		0x8C
#define M2_ANTI_CLOCKWISE	0x8E

//Left Motor: M0 on board, M1 in code
#define LEFT_ANTI_CLOCKWISE		M1_ANTI_CLOCKWISE
#define LEFT_CLOCKWISE			M1_CLOCKWISE

//Right Motor: M1 on board, M2 in code
#define RIGHT_ANTI_CLOCKWISE	M2_ANTI_CLOCKWISE
#define RIGHT_CLOCKWISE			M2_CLOCKWISE

#define getCountsLeft getCountsM1
#define getCountsRight getCountsM2

/********************************************
 *  Colour Sensor                           *
 ********************************************/

//Serial pins
#define ColourSensor Serial1

//SoftwareSerial pins
//Note: Not all pins on the Mega and Mega 2560 support change interrupts,  so only the following can be used for RX: 
//10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69)
#define COLOUR_PIN (10)
#define COLOUR_UNUSED_PIN (255)

//OldSoftwareSerial pins
#define COLOUR_INPUT_PIN (2)
#define COLOUR_OUTPUT_PIN (3)

#define COLOUR_BAUD (4800)

#define COLOUR_DELAY (200)

#define COLOUR_LOOP_DELAY (100)

/********************************************
 *  Movement
 * TODO: Rename MOTOR_1, MOTOR_2 to LEFT, RIGHT (need to look at HW to determine which is which)
 ********************************************/

//#defines for wheel encoders,
//Quadrature encoders use 2 signals per motor, signal A and signal B
#define MOTOR_1_SIG_A 8
#define MOTOR_1_SIG_B 9
#define MOTOR_2_SIG_A 11
#define MOTOR_2_SIG_B 10

#define DEFAULT_SPEED 95

//For line following.
#define EDGE_SENSITIVITY 1000
#define QTR_READ_DELAY 75
#define CORRECTION_ANGLE 40

#define TURN_FRONT 0
#define TURN_RIGHT (90 -5)
#define TURN_BACK 180
#define TURN_LEFT (-TURN_RIGHT +5)

#define TICKS_FRONT (0)
#define TICKS_RIGHT (39)
#define TICKS_BACK (78)
#define TICKS_LEFT (-TICKS_RIGHT)

/********************************************
 *  GridMap
 ********************************************/

//Grid dimensions
#define GRID_MAX_X (7)
#define GRID_MAX_Y (3)

//Known locations
#define ENTRANCE_X (GRID_MAX_X)
#define ENTRANCE_Y (0)

//The status of point on the grid is bitmapped to 1 byte and contains the following flags:
//	Seen - The point has been seen
//	Visited - Have been to the point
//	Occupied - Holds a block
//	Red - Holds a block that is mostly red
//	Green - Holds a block that is mostly green
//	Blue - Holds a block that is mostly blue
//
//Order of flags: SVxxORGB, where x denotes currently unused bits.

//Most significant nibble mask bits
#define SEEN	(0x80)
#define VISITED	(0x40)
#define UNUSED1	(0x20)
#define UNUSED2	(0x10)

//Least significant nibble mask bits
#define OCCUPIED	(0x08)
#define RED			(0x04)
#define GREEN		(0x02)
#define BLUE		(0x01)

/********************************************
 *  MazeMap
 ********************************************/

#define MAZE_MAX 256

/********************************************
 *  GridNav
 ********************************************/

#define CENTRE_DIST (300)
#define BLOCK_ACT_DIST (250)
#define BLOCK_TOLERANCE (50)
#define BLOCK_STOP (80 - 30)

/********************************************
 *  MazeNav
 ********************************************/
/*	|       |
	|       '-------.
	|               |
	|       |<----->| = 350mm wall minimum (full)
	|               |
	|       .-------'
	|       |
	
	|       |
	|       |
	|   |<->| = ~175mm wall minimum (half)
	|       |
	|       |
	
	|       |
	|       '-------.
	|               |
	|   |<--------->| ~175mm + 350mm = ~525mm max wall
	|               |
	|       .-------'
	|       |
*/
#define WALL_MIN_FULL (350)
#define WALL_MIN_HALF (175)
#define WALL_TOLERANCE (50)

//Maze is 2400mm x 2400mm, Dropzone 350mm x 350mm
//Using: ticks ~= length*(48/132)
#define MAX_EAST_TICKS (809)	// length = (2400-350/2)
#define MAX_NORTH_TICKS (681)	// length = (2400-350*(3/2))
