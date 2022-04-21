/************************************************/
/*	    Author: Adam Ližičiar (xlizic00)	    */
/************************************************/

#include "proj2.h"

#define SUCCESS 0
#define FAILURE 1
#define ERROR  -1

#define SEMAPHORE_OXIGEN            "/xlizic00-ios-semaphore-oxigen"
#define SEMAPHORE_HYDROGEN          "/xlizic00-ios-semaphore-hydrogen"
#define SEMAPHORE_WRITING           "/xlizic00-ios-semaphore-writing"
#define SEMAPHORE_MOLECULE          "/xlizic00-ios-semaphore-molecule"
#define SEMAPHORE_MUTEX             "/xlizic00-ios-semaphore-mutex"
#define SEMAPHORE_MOLECULE_CREATED  "/xlizic00-ios-semaphore-molecule_created"


FILE *output_file                   = NULL;
sem_t *semaphore_oxigen             = NULL;
sem_t *semaphore_hydrogen           = NULL;
sem_t *semaphore_writing            = NULL;
sem_t *semaphore_molecule           = NULL;
sem_t *semaphore_mutex              = NULL;
sem_t *semaphore_molecule_created   = NULL;
count_t *counter                    = NULL; 
bool running_oxigen                 = true;
bool running_hydrogen               = true;
pid_t atoms;


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
void write_down(char atom_symbol, int atom_id, char message[30], int molecule_num, char last_word[20]) {
    sem_wait(semaphore_writing);

    counter->line++;

    if(molecule_num != 0) {
        printf("%d: %c %d: %s %d %s\n", counter->line, atom_symbol, atom_id, message, molecule_num, last_word);
        fprintf(output_file, "%d: %c %d: %s %d %s\n", counter->line, atom_symbol, atom_id, message, molecule_num, last_word);
    }
    else {
        printf("%d: %c %d: %s %s\n", counter->line, atom_symbol, atom_id, message, last_word);
        fprintf(output_file, "%d: %c %d: %s %s\n", counter->line, atom_symbol, atom_id, message, last_word);
    }

    fflush(output_file);
    sem_post(semaphore_writing);
}

// Function wait int miliseconds and after
// continue.
void wait_max(int miliseconds) {
    usleep((rand() % miliseconds)*1000);
}

// Function will create molecule.
void create_H2O(char atom, int num, int time_wait) {
    sem_wait(semaphore_molecule);
    counter->molecules++;
    write_down(atom, num, "creating molecule", counter->molecules/3, "");
    sem_wait(semaphore_molecule_created);
    wait_max(time_wait);
    sem_post(semaphore_molecule_created);
    write_down(atom, num, "molecule", counter->molecules/3, "created");
    sem_post(semaphore_molecule);
}


/** MAIN FUNCTIONS **/ 

// Init of semaphores.
void init() {
    // Unlink semaphores when previous program crash.
    sem_unlink(SEMAPHORE_OXIGEN);
    sem_unlink(SEMAPHORE_HYDROGEN);
    sem_unlink(SEMAPHORE_WRITING);
    sem_unlink(SEMAPHORE_MOLECULE);
    sem_unlink(SEMAPHORE_MUTEX);
    sem_unlink(SEMAPHORE_MOLECULE_CREATED);

    // Opening/creating file for output.
	output_file = fopen("proj2.out","w");
	if(output_file == NULL)
	{
		fprintf(stderr,"File proj2.out can not be open!\n");
		exit(FAILURE);
	}

    // Create semaphores and handle errors
    if ((semaphore_oxigen = sem_open(SEMAPHORE_OXIGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((semaphore_hydrogen = sem_open(SEMAPHORE_HYDROGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((semaphore_writing = sem_open(SEMAPHORE_WRITING, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((semaphore_molecule = sem_open(SEMAPHORE_MOLECULE, O_CREAT | O_EXCL, 0666, 3)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((semaphore_mutex = sem_open(SEMAPHORE_MUTEX, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((semaphore_molecule_created = sem_open(SEMAPHORE_MOLECULE_CREATED, O_CREAT | O_EXCL, 0666, -2)) == SEM_FAILED)
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

    // Set counter->molecules to 2, because in function create_H2O
    // it will be divided by 3 to show number of molecules 
    // that was created.
    counter->molecules = 2;
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
	if((sem_close(semaphore_oxigen)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(semaphore_hydrogen)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(semaphore_writing)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(semaphore_molecule)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(semaphore_mutex)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(semaphore_molecule_created)) == ERROR)
	{
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}

    // Unlink semaphores
	if((sem_unlink(SEMAPHORE_OXIGEN)) == ERROR)
	{
		fprintf(stderr,"Semaphore oxigen was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEMAPHORE_HYDROGEN)) == ERROR)
	{
		fprintf(stderr,"Semaphore hydrogen was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEMAPHORE_WRITING)) == ERROR)
	{
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEMAPHORE_MOLECULE)) == ERROR)
	{
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEMAPHORE_MUTEX)) == ERROR)
	{
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEMAPHORE_MOLECULE_CREATED)) == ERROR)
	{
		fprintf(stderr,"Semaphore writing was not unlink\n");
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

int main(int argc, char *argv[])
{
    arguments_t arguments = check_arguments(argc, &(*argv));
    init();

    atoms = fork();

    // Oxigen subprocess
    if(atoms == 0) {
        for(unsigned int c_oxigen = 0;c_oxigen < arguments.NO; c_oxigen++)
        {
            if(fork() == 0)
            {
                int oxigen_number = ++counter->NO;
                   
                write_down('O', oxigen_number, "started", 0, "");
                wait_max(arguments.TI);
                write_down('O', oxigen_number, "going to queue", 0, "");
                   
                sem_wait(semaphore_mutex);
                counter->NO_used++;

                if(arguments.NO > counter->molecules/3 && arguments.NH / 2 > counter->molecules/3) {   
                    if(counter->NH_used >= 2) {
                        sem_post(semaphore_hydrogen);
                        sem_post(semaphore_hydrogen);
                        counter->NH_used = counter->NH_used - 2;
                        sem_post(semaphore_oxigen);
                        counter->NO_used--;
                    }
                    else {
                        sem_post(semaphore_mutex);
                    }
 
                    sem_wait(semaphore_oxigen);
                    create_H2O('O', oxigen_number, arguments.TB);
                    sem_post(semaphore_mutex);
                }
                else {
                    write_down('O', oxigen_number, "not enough H", 0, "");
                    sem_post(semaphore_mutex);
                }
                exit(SUCCESS); 
            }
        }
        for(unsigned int c_oxigen=0;c_oxigen<arguments.NO;c_oxigen++) 
            wait(NULL);
    }

    // Hydrogen subprocess        
    else if(atoms > 0) {
        for(unsigned int c_hydrogen = 0; c_hydrogen < arguments.NH; c_hydrogen++)
        {
            if(fork() == 0)
            {
                int hydrogen_number = ++counter->NH;
                
                write_down('H', hydrogen_number, "started", 0, "");
                wait_max(arguments.TI);
                write_down('H', hydrogen_number, "going to queue", 0, ""); 

                sem_wait(semaphore_mutex);
                counter->NH_used++;

                if(arguments.NO > counter->molecules/3 && arguments.NH / 2 > counter->molecules/3) {
                    if(counter->NH_used >= 2 && counter->NO_used >= 1) {
                        sem_post(semaphore_hydrogen);
                        sem_post(semaphore_hydrogen);
                        counter->NH_used = counter->NH_used - 2;
                        sem_post(semaphore_oxigen);
                        counter->NO_used--;
                    }
                    else {
                        sem_post(semaphore_mutex);
                    }

                    sem_wait(semaphore_hydrogen);
                    create_H2O('H', hydrogen_number, arguments.TB);
                }
                else {
                    write_down('H', hydrogen_number, "not enough O or H", 0, "");
                    sem_post(semaphore_mutex);
                }
                
                exit(SUCCESS);
            }
        }
        for(unsigned int c_hydrogen=0;c_hydrogen<arguments.NO;c_hydrogen++) 
            wait(NULL);
    }
    else if(atoms < 0) {
        end();
        fprintf(stderr,"Another process was not created\n");
        exit(EXIT_FAILURE);
    }

    return SUCCESS; 
}