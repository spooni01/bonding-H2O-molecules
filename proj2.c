/************************************************/
/*	    Author: Adam Ližičiar (xlizic00)	    */
/************************************************/

#include "proj2.h"

#define SUCCESS 0
#define FAILURE 1
#define ERROR  -1

#define SEMAPHORE_MOLECULE "/xlizic00-ios-semaphore-molecule"

FILE *output_file = NULL;

sem_t *sem_molecule = NULL;
count_t *counter = NULL;

pid_t app;


/** HELP FUNCTIONS **/

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

// Function will print text to terminal
// and also write it to output_file.
void write_down(int line, char atom_symbol, int atom_id, char message[20]) {
    printf("%d: %c %d: %s\n", line, atom_symbol, atom_id, message);
    fprintf(output_file, "%d: %c %d: %s\n", line, atom_symbol, atom_id, message);
}

// Function wait int miliseconds and after
// continue.
void wait_max(int miliseconds) {
    usleep((rand() % miliseconds)*1000);
}


/** MAIN FUNCTIONS **/ 

// Init of semaphores.
void init() {
    // Unlink semaphores when previous program crash.
    sem_unlink(SEMAPHORE_MOLECULE);

    // Opening/creating file for output.
	output_file = fopen("proj2.out","w");
	if(output_file == NULL)
	{
		fprintf(stderr,"File proj2.out can not be open!\n");
		exit(FAILURE);
	}

    // Create semaphores and handle errors
    if ((sem_molecule = sem_open(SEMAPHORE_MOLECULE, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}

    // Create memory
	counter = mmap(NULL, sizeof(count_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
	if(counter == MAP_FAILED)
	{
		fprintf(stderr,"Shared memory was not created.\n");
		exit(FAILURE);
	}
}

// End process
void end()
{
	// Destroy shared memory
	if(munmap(counter, sizeof(count_t)) == ERROR)
	{
		fprintf(stderr,"Cannot destroy shared memory!\n");
		exit(FAILURE);
	}

    // Close semaphores
	if((sem_close(sem_molecule)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}

    // Unlink semaphores
	if((sem_unlink(SEMAPHORE_MOLECULE)) == ERROR)
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
     
    app = fork();

    // Making of molecules process
    if(app == 0) {

        
        
        exit(EXIT_SUCCESS);
    }

    // Main process
    else if(app > 0) {
        pid_t atoms = fork();

        // Subprocess oxygen
        if(atoms == 0) {
            for (unsigned int i = 0; i < arguments.NO; i++)
            {
                write_down(++counter->line, 'O', ++counter->NO, "started");
                wait_max(arguments.TI);

                write_down(++counter->line, 'O', counter->NO, "going to queue");
            }
        }

        // Subprocess hydrogen
        else if(atoms > 0) {
            for (unsigned int i = 0; i < arguments.NH; i++)
            {
                write_down(++counter->line, 'H', ++counter->NH, "started");
                wait_max(arguments.TI);

                write_down(++counter->line, 'H', counter->NH, "going to queue");      
            }
        }

        // Error
        else {
            end();
            fprintf(stderr,"Another process was not created.");
            exit(FAILURE);
        }
    }
    else {
        end();
        fprintf(stderr,"Another process was not created.");
        exit(FAILURE);
    }

    
    return SUCCESS; 
}