/************************************************/
/*	    Author: Adam Ližičiar (xlizic00)	    */
/************************************************/

#include "proj2.h"

#define SUCCESS 0
#define FAILURE 1
#define ERROR  -1

#define SEMAPHORE "/xlizic00-ios-semaphore"

FILE *output_file = NULL;

sem_t *semaphore = NULL;
state_t *shared_memory = NULL;


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
        fprintf(stderr, "Stdin must have 4 arguments, format: ./proj2 {INT} {INT} {INT} {INT}\n");
        exit(FAILURE);
    }
    
    // Check if argv-s are numbers
    if (!isUnsignedNumber(argv[1]) || !isUnsignedNumber(argv[2]) || !isUnsignedNumber(argv[3]) || !isUnsignedNumber(argv[4])) {
        fprintf(stderr, "Arguments must be just unsigned ints, format: ./proj2 {INT} {INT} {INT} {INT}\n");
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
        fprintf(stderr, "Second and third argument must be in interval [0 <= TI/TB <= 1000]\n");
        exit(FAILURE);
    }
        
    return arg;
}

// Init of semaphores.
void init() {
    // Unlink semaphores when previous program crash.
    sem_unlink(SEMAPHORE);

    // Opening/creating file for output.
	output_file = fopen("proj2.out","w");
	if(output_file == NULL)
	{
		fprintf(stderr,"File proj2.out can not be open!\n");
		exit(FAILURE);
	}

    // Create semaphores and handle errors
    if ((semaphore = sem_open(SEMAPHORE, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(EXIT_FAILURE);
	}

    // Create memory
	shared_memory = mmap(NULL, sizeof(state_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
	if(shared_memory == MAP_FAILED)
	{
		fprintf(stderr,"Shared memory was not created.\n");
		exit(EXIT_FAILURE);
	}
}

// End process
void end()
{
	// Destroy shared memory
	if(munmap(shared_memory, sizeof(state_t)) == ERROR)
	{
		fprintf(stderr,"Cannot destroy shared memory!\n");
		exit(EXIT_FAILURE);
	}

    // Close semaphores
	if((sem_close(semaphore)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}

    // Unlink semaphores
	if((sem_unlink(SEMAPHORE)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not unlink\n");
		exit(FAILURE);
	}

    // Close output file
	int file_close = fclose(output_file);
	if(file_close == EOF)
	{
		fprintf(stderr,"Output file was not closed!\n");
		exit(FAILURE);
	}
}

/** MAIN FUNCTION **/

int main (int argc, char *argv[])
{
    arguments_t arguments = check_arguments(argc, &(*argv));
    init();
    /* control */ printf("%d", arguments.NH);

    end();
    return SUCCESS; 
}