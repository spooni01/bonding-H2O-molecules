/************************************************/
/*	    Author: Adam Ližičiar (xlizic00)	    */
/************************************************/

#include "proj2.h"

#define SUCCESS 0
#define FAILURE 1

/** FUNCTIONS **/

// Check if text is made just from integers
bool isUnsignedNumber(char num[])
{
    for (int i = 0; num[i] != 0; i++)
        if (!isdigit(num[i]))
            return false;
    
    return true;
}

// Function will check, if there is right number of arguments
// and also if their format is correct.
// Format: ./proj2 {INT} {INT} {INT} {INT}
arguments_t check_arguments(int argc, char *argv[]) {
    if(argc != 5) {
        fprintf(stderr, "Stdin must have 4 arguments, format: ./proj2 {INT} {INT} {INT} {INT}");
        exit(FAILURE);
    }
    
    // Check if argv-s are numbers
    if (!isUnsignedNumber(argv[1]) || !isUnsignedNumber(argv[2]) || !isUnsignedNumber(argv[3]) || !isUnsignedNumber(argv[4])) {
        fprintf(stderr, "Arguments must be just unsigned ints, format: ./proj2 {INT} {INT} {INT} {INT}");
        exit(FAILURE);
    }

    // Save arguments to arguments_t
    arguments_t arg = {
        atoi(argv[1]), // NO
        atoi(argv[2]), // NH
        atoi(argv[3]), // TI
        atoi(argv[4]), // TB
    };

    // Check if third or fourth argument is 0 <= TI/TB <= 1000.
    // Don't need to test, if arg.TI or arg.TB is less than 0,
    // because it is test in isUnsignedNumber() function.
    if(1000 < arg.TI || 1000 < arg.TB) {
        fprintf(stderr, "Second and third argument must be in interval [0 <= TI/TB <= 1000]");
        exit(FAILURE);
    }
        
    return arg;
}


/** MAIN FUNCTION **/

int main (int argc, char *argv[])
{
    arguments_t arguments = check_arguments(argc, &(*argv));
    /* control */ printf("%d", arguments.NH);
    return 0; 
}