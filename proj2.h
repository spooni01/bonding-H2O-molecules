#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> // using function isdigit()

/** STRUCTS **/
typedef struct {
	unsigned int NO; // Number of oxigens atoms
	unsigned int NH; // Number of hydrogens atoms
	unsigned int TI; // The maximum time that an oxygen/hydrogen waits before forming queues for the formation of molecules. (0 <= TI <= 1000)
	unsigned int TB; // The maximum time in milliseconds required to create a single molecule. (0 <= TB <= 1000)
} arguments_t;


/** FUNCTIONS **/
bool 			isUnsignedNumber(char num[]);
arguments_t 	check_arguments(int argc, char *argv[]);

