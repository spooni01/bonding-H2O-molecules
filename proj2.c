/************************************************/
/*	    Author: Adam Ližičiar (xlizic00)	    */
/************************************************/

#include "proj2.h"

#define SUCCESS 0
#define FAILURE 1
#define ERROR  -1

#define SEM_MUTEX               "/xlizic00-ios-semaphore-mutex"
#define SEM_OXYGEN              "/xlizic00-ios-semaphore-oxygen"
#define SEM_HYDROGEN            "/xlizic00-ios-semaphore-hydrogen"
#define SEM_WRITING             "/xlizic00-ios-semaphore-writing"
#define SEM_MOLECULE            "/xlizic00-ios-semaphore-molecule"
#define SEM_MOLECULE_CREATED    "/xlizic00-ios-semaphore-molecule-created"
#define SEM_BARRIER             "/xlizic00-ios-semaphore-barrier"
#define SEM_MUTEX_BARRIER       "/xlizic00-ios-semaphore-mutex-barrier"

FILE *output_file               = NULL;
sem_t *sem_mutex                = NULL;
sem_t *sem_oxygen               = NULL;
sem_t *sem_hydrogen             = NULL;
sem_t *sem_writing              = NULL;
sem_t *sem_molecule             = NULL;
sem_t *sem_molecule_created     = NULL;
sem_t *sem_barrier              = NULL;
sem_t *sem_mutex_barrier        = NULL;

data_t *data;

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

    // Set arg.TI and arg.TB to 1, when it is 0
    if(arg.TI == 0) 
        arg.TI = 1;
    if(arg.TB == 0)
        arg.TB = 1;
        
    return arg;
}

// Function will print text to terminal
// and also write it to output_file.
void write_down(char atom_symbol, int atom_id, char message[30], int molecule_num, char last_word[20]) {
    sem_wait(sem_writing);

    data->line++;

    if(molecule_num != 0) {
        printf("%d: %c %d: %s %d %s\n", data->line, atom_symbol, atom_id, message, molecule_num, last_word);
        fprintf(output_file, "%d: %c %d: %s %d %s\n", data->line, atom_symbol, atom_id, message, molecule_num, last_word);
    }
    else {
        printf("%d: %c %d: %s %s\n", data->line, atom_symbol, atom_id, message, last_word);
        fprintf(output_file, "%d: %c %d: %s %s\n", data->line, atom_symbol, atom_id, message, last_word);
    }

    fflush(output_file);
    sem_post(sem_writing);
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
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_OXYGEN);
    sem_unlink(SEM_HYDROGEN);
    sem_unlink(SEM_WRITING);
    sem_unlink(SEM_MOLECULE);
    sem_unlink(SEM_MOLECULE_CREATED);
    sem_unlink(SEM_BARRIER);
    sem_unlink(SEM_MUTEX_BARRIER);

    // Opening/creating file for output.
	output_file = fopen("proj2.out","w");
	if(output_file == NULL) {
		fprintf(stderr,"File proj2.out can not be open!\n");
		exit(FAILURE);
	}

    // Create semaphores and handle errors
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_oxygen = sem_open(SEM_OXYGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_hydrogen = sem_open(SEM_HYDROGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_writing = sem_open(SEM_WRITING, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_molecule = sem_open(SEM_MOLECULE, O_CREAT | O_EXCL, 0666, 3)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_molecule_created = sem_open(SEM_MOLECULE_CREATED, O_CREAT | O_EXCL, 0666, -2)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_barrier = sem_open(SEM_BARRIER, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}
    if ((sem_mutex_barrier = sem_open(SEM_MUTEX_BARRIER, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
		fprintf(stderr,"Semaphore was not created:\n %s\n",strerror(errno));
        exit(FAILURE);
	}

    // Create memory
	data = mmap(NULL, sizeof(data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
	if(data == MAP_FAILED) {
		fprintf(stderr,"Shared memory was not created.\n");
		exit(FAILURE);
	}

    // Set data->molecules to 0
    data->bonding_atoms = 0;
    data->molecules = 1;
}

// End process
void end() {
	// Destroy shared memory
	if(munmap(data, sizeof(data_t)) == ERROR) {
		fprintf(stderr,"Cannot destroy shared memory!\n");
		exit(FAILURE);
	}

    // Close semaphores
    if((sem_close(sem_mutex)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_oxygen)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_hydrogen)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_writing)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_molecule)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_molecule_created)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_barrier)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}
    if((sem_close(sem_mutex_barrier)) == ERROR) {
		fprintf(stderr,"Semaphore was not closed\n");
		exit(FAILURE);
	}

    // Unlink semaphores
	if((sem_unlink(SEM_MUTEX)) == ERROR) {
		fprintf(stderr,"Semaphore oxygen was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_OXYGEN)) == ERROR) {
		fprintf(stderr,"Semaphore hydrogen was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_HYDROGEN)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_WRITING)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_MOLECULE)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_MOLECULE_CREATED)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_BARRIER)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}
    if((sem_unlink(SEM_MUTEX_BARRIER)) == ERROR) {
		fprintf(stderr,"Semaphore writing was not unlink\n");
		exit(FAILURE);
	}

    // Close output file
	int file_close = fclose(output_file);
	if(file_close == EOF) {
		fprintf(stderr,"Output file was not closed!\n");
		exit(FAILURE);
	}
}

// Function will create molecule.
void create_H2O(char atom, int num, int time_wait) {
    sem_wait(sem_molecule);
    sem_wait(sem_mutex_barrier);
    data->bonding_atoms++;
    
    // Molecule number
    if(data->bonding_atoms % 3 == 0) {
        data->molecules = data->bonding_atoms / 3;
    }
    sem_post(sem_mutex_barrier);

    write_down(atom, num, "creating molecule", data->molecules, "");

    if(data->bonding_atoms++ % 3 == 1)
        sem_post(sem_barrier);

    sem_wait(sem_barrier);
    sem_post(sem_barrier);    

    wait_max(time_wait);
    write_down(atom, num, "molecule", data->molecules, "created");
    sem_post(sem_molecule);
}


/** MAIN FUNCTION **/

int main(int argc, char *argv[]) {
    arguments_t arg = check_arguments(argc, &(*argv));

    init();
    
    // Count number of molecules that will be created
    unsigned int num_of_molecules = arg.NO;
    if(arg.NH/2 < num_of_molecules) {
        num_of_molecules = arg.NH / 2;
    }

    for(unsigned int count_oxygen = 1; count_oxygen <= arg.NO; count_oxygen++) {
        if(fork() == 0) {  
            srand(time(NULL) * getpid());
            write_down('O', count_oxygen, "started", 0, "");
            wait_max(arg.TI);
            write_down('O', count_oxygen, "going to queue", 0, "");
            
            sem_wait(sem_mutex);
            data->oxygen++;
            if(num_of_molecules >= data->oxygen) {
                data->act_oxygen++;

                if(data->act_hydrogen >= 2) {
                    sem_post(sem_hydrogen);
                    sem_post(sem_hydrogen);
                    data->act_hydrogen = data->act_hydrogen - 2;
                    sem_post(sem_oxygen);
                    data->act_oxygen--;
                }
                else {
                    sem_post(sem_mutex);
                } 
                sem_wait(sem_oxygen);
                create_H2O('O', count_oxygen, arg.TB);
                sem_wait(sem_barrier);
                sem_post(sem_mutex);
            }
            else {
                write_down('O', count_oxygen, "not enough H", 0, "");
                sem_post(sem_mutex);
            }
            
            exit(SUCCESS);       
        }
    }

    for(unsigned int count_hydrogen = 1; count_hydrogen <= arg.NH; count_hydrogen++)    {
        if(fork() == 0){
            srand(time(NULL) * getpid());
            write_down('H', count_hydrogen, "started", 0, "");
            wait_max(arg.TI);
            write_down('H', count_hydrogen, "going to queue", 0, "");
            
            sem_wait(sem_mutex);      

            data->hydrogen++;
            if(num_of_molecules * 2 >= data->hydrogen) {            
                data->act_hydrogen++;
                if(data->act_hydrogen >= 2 && data->act_oxygen >= 1) {
                    sem_post(sem_hydrogen);
                    sem_post(sem_hydrogen);
                    data->act_hydrogen = data->act_hydrogen - 2;
                    sem_post(sem_oxygen);
                    data->act_oxygen--;                
                }
                else {    
                    sem_post(sem_mutex);
                }  
                sem_wait(sem_hydrogen);
                create_H2O('H', count_hydrogen, arg.TB);
            }
            else {
                write_down('H', count_hydrogen, "not enough O or H", 0, "");
                sem_post(sem_mutex);
            }  

            exit(SUCCESS);         
        }
    }

    for(unsigned int i = 0; i < arg.NH+arg.NO; i++)
        wait(NULL);

    while(wait(NULL) > 0);
    return SUCCESS; 
}