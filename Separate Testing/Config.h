/********************************************
 *  Colour Sensor                           *
 ********************************************/

//Note: Not all pins on the Mega and Mega 2560 support change interrupts, 
//so only the following can be used for RX: 
//10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69)
#define COLOUR_PIN (10)

//Non-existant pin
#define COLOUR_UNUSED_PIN (255)

#define COLOUR_BAUD (4800)

#define COLOUR_DELAY (200)