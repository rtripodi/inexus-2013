#include<stdio.h>>

//Grid size
#define GRID_LENGTH (8)
#define GRID_WIDTH (8)

//The status of point on the grid is bitmapped to 1 byte and contains the following flags:
//	Seen - The point has been seen
//	Visited - Have been to the point
//	Occupied - Holds a block
//	Red - Holds a block that is mostly red
//	Green - Holds a block that is mostly green
//	Blue - Holds a block that is mostly blue
//
//Order of flags: SVxxORGB, where x denotes currently unused bits.

//Most significant nibble
#define POINT_SEEN 0x80
#define POINT_VISITED 0x40
#define POINT_UNUSED1 0x20
#define POINT_UNUSED2 0x10

//Least significant nibble
#define POINT_OCCUPIED 0x01
#define POINT_RED 0x08
#define POINT_GREEN 0x04
#define POINT_BLUE 0x02

union
{
	unsigned char point_status;
	struct
	{
		unsigned char blue		: 1;
		unsigned char green		: 1;
		unsigned char red		: 1;
		unsigned char occupied	: 1;
		unsigned char unused2	: 1;
		unsigned char unused1	: 1;
		unsigned char visited	: 1;
		unsigned char seen		: 1;
	} status_bit;
} grid[GRID_LENGTH][GRID_WIDTH];

main()
{
	grid[0][0].point_status = 0x88;
	printf("Point (0,0) has been seen and it is known that it holds a block -\n");
	printf("\tBinary: %x%x%x%x %x%x%x%x\n",
		grid[0][0].status_bit.seen, grid[0][0].status_bit.visited, grid[0][0].status_bit.unused1, grid[0][0].status_bit.unused2,
		grid[0][0].status_bit.occupied, grid[0][0].status_bit.red, grid[0][0].status_bit.green, grid[0][0].status_bit.blue);
	printf("\tHex: %x\n", grid[0][0].point_status);
	
	grid[0][0].status_bit.visited = 0x01;
	printf("\nSetting 'visited' status flag using union -\n");
	printf("\tBinary: %x%x%x%x %x%x%x%x\n",
		grid[0][0].status_bit.seen, grid[0][0].status_bit.visited, grid[0][0].status_bit.unused1, grid[0][0].status_bit.unused2,
		grid[0][0].status_bit.occupied, grid[0][0].status_bit.red, grid[0][0].status_bit.green, grid[0][0].status_bit.blue);
	printf("\tHex: %x\n", grid[0][0].point_status);
	grid[0][0].status_bit.visited = 0x00;

	grid[0][0].point_status |= POINT_VISITED;
	printf("\nSetting 'visited' status flag using bitwise -\n");
	printf("\tBinary: %x%x%x%x %x%x%x%x\n",
		grid[0][0].status_bit.seen, grid[0][0].status_bit.visited, grid[0][0].status_bit.unused1, grid[0][0].status_bit.unused2,
		grid[0][0].status_bit.occupied, grid[0][0].status_bit.red, grid[0][0].status_bit.green, grid[0][0].status_bit.blue);
	printf("\tHex: %x\n", grid[0][0].point_status);
}
