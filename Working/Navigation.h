#ifndef Navigation_h
#define Navigation_h

typedef struct {
	int frnt;
	int rght;
	int bck;
	int lft;
} IrValues;

typedef struct {
	IR *frnt;
	IR *rght;
	IR *bck;
	IR *lft;
} IrSensors;

#endif
