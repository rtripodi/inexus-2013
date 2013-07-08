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
#define UNASSIGNED_SWITCH_1 52 //Closest switch to the edge of the shield
#define UNASSIGNED_SWITCH_2 50
#define UNASSIGNED_SWITCH_3 48
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

/********************************************
 *  Servos / Claw                           *
 ********************************************/
//Pins used for communication with servos
#define CLAW_LEFT_PIN (6)
#define CLAW_RIGHT_PIN (7)
 
//Values for each servo to allow claw to be in open state
#define CLAW_OPEN_LEFT (95)
#define CLAW_OPEN_RIGHT (89)

//Values for each servo to allow claw to be in closed state
#define CLAW_CLOSE_LEFT (140)
#define CLAW_CLOSE_RIGHT (44)

//Values for each servo to allow claw to be in shut state
#define CLAW_SHUT_LEFT (189)
#define CLAW_SHUT_RIGHT (-1)

/********************************************
 *  IR Sensors                              *
 ********************************************/
//Pins used for communication with IR sensors
#define IR_SHORT_PIN (0)
#define IR_MEDIUM_PIN (1)
#define IR_LONG1_PIN (2)
#define IR_LONG2_PIN (3)

//Amount of IR reads to determine mean value
#define IR_ITERATIONS (5)

/********************************************
 *  Motor                                   *
 ********************************************/
//Serial used for Motor Controller
#define MotorControl Serial1