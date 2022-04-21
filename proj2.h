#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> // using function isdigit()
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

// todo: sort
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <string.h>


/** STRUCTS **/
typedef struct {
	unsigned int NO; 		// Number of oxigen atoms
	unsigned int NH; 		// Number of hydrogen atoms
	unsigned int molecules; // Number of created molecules
	unsigned int line;		// Number of lines

	unsigned int NO_used;	// Number of used oxigen atoms
	unsigned int NH_used;	// Number of used hydrogen atoms
} count_t;

typedef struct {
	unsigned int NO; // Number of oxigens atoms
	unsigned int NH; // Number of hydrogens atoms
	unsigned int TI; // The maximum time that an oxygen/hydrogen waits before forming queues for the formation of molecules. (0 <= TI <= 1000)
	unsigned int TB; // The maximum time in milliseconds required to create a single molecule. (0 <= TB <= 1000)
} arguments_t;

/** FUNCTIONS **/
bool 			isUnsignedNumber(char num[]);
arguments_t 	check_arguments(int argc, char *argv[]);
void			init();
void			end();
//todo: add function prototzpes

