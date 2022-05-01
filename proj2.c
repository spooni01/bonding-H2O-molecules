/************************************************
* Project:      Bonding H2O molecules          
* File:         proj2.c         
* Author:       Adam Ližičiar (xlizic00)	
* Description:  doc/zadanie.pdf    
************************************************/

#include "proj2.h"

#define SUCCESS                 0
#define FAILURE                 1
#define ERROR                   -1

#define ATOMS_IN_MOLECULE       3

#define SEM_MUTEX               "/xlizic00-ios-semaphore-mutex"
#define SEM_OXYGEN              "/xlizic00-ios-semaphore-oxygen"
#define SEM_HYDROGEN            "/xlizic00-ios-semaphore-hydrogen"
#define SEM_WRITING             "/xlizic00-ios-semaphore-writing"
#define SEM_BARRIER_TURNSTILE   "/xlizic00-ios-semaphore-barrier-turnstile"
#define SEM_BARRIER_TURNSTILE_2 "/xlizic00-ios-semaphore-barrier-turnstile-2"
#define SEM_BARRIER_MUTEX       "/xlizic00-ios-semaphore-barrier-mutex"

sem_t *sem_mutex                = NULL;
sem_t *sem_oxygen               = NULL;
sem_t *sem_hydrogen             = NULL;
sem_t *sem_writing              = NULL;
sem_t *sem_barrier_turnstile    = NULL;
sem_t *sem_barrier_turnstile_2  = NULL;
sem_t *sem_barrier_mutex        = NULL;

FILE *output_file               = NULL;

data_t *data;


/** HELP FUNCTIONS **/

// Check if text (string!) is made just from unsigned integers
bool isUnsignedNumber(char num[])
{
    for (int i = 0; num[i] != 0; i++)
        if (!isdigit(num[i]))
            return false;
    
    return true;
}

// Function will write text to stderr and will exit program with
// return code 1 (=ERROR)
void end_program_with_error(char str[100]) {
    fprintf(stderr,str);
    fprintf(stderr,"\n");
    exit(FAILURE);
}

// Calculates the current molecule number and returns
// it as an integer.
void calculate_molecule_number() {
    if(data->bonding_atoms > ATOMS_IN_MOLECULE - 1) {
        if(data->bonding_atoms % ATOMS_IN_MOLECULE == 1) 
            data->molecules = (data->bonding_atoms + 2) / ATOMS_IN_MOLECULE;        
        
        else if(data->bonding_atoms % ATOMS_IN_MOLECULE == 2) 
            data->molecules = (data->bonding_atoms + 1) / ATOMS_IN_MOLECULE;     
        
        else 
            data->molecules = data->bonding_atoms / ATOMS_IN_MOLECULE;      
    }
}

// Function will check, if there is right number of arguments
// and also if their format is correct.
// Correct format: ./proj2 {INT} {INT} {INT} {INT}
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

    // Check if third or fourth argument is in interval 0 <= TI/TB <= 1000.
    // Do not need to test, if arg.TI or arg.TB is less than 0,
    // because it is test in isUnsignedNumber() function.
    if(1000 < arg.TI || 1000 < arg.TB) {
        fprintf(stderr, "Second and third argument must be in interval [0 <= TI/TB <= 1000]\n");
        exit(FAILURE);
    }

    // Set arg.TI and arg.TB to 1, when it is 0,
    // because time must be minimum 1
    if(arg.TI == 0) 
        arg.TI = 1;
    if(arg.TB == 0)
        arg.TB = 1;
        

    return arg;
}

// Function will print text to terminal and also write it to output_file.
// If molecule_num is set to 0, function will print text in format for atoms,
// otherwise function will print text in format for molecules.
void write_down(char atom_symbol, int atom_id, char message[30], int molecule_num, char last_word[20]) {
    // This semaphore protects the correct row order
    sem_wait(sem_writing);

    data->line++;

    // Message for molecules
    if(molecule_num != 0) {
        printf("%d: %c %d: %s %d %s\n", data->line, atom_symbol, atom_id, message, molecule_num, last_word);
        fprintf(output_file, "%d: %c %d: %s %d %s\n", data->line, atom_symbol, atom_id, message, molecule_num, last_word);
    }
    // Message for atoms
    else {
        printf("%d: %c %d: %s %s\n", data->line, atom_symbol, atom_id, message, last_word);
        fprintf(output_file, "%d: %c %d: %s %s\n", data->line, atom_symbol, atom_id, message, last_word);
    }

    fflush(output_file);
    sem_post(sem_writing);
}

// Program will wait maximum 'int miliseconds' and after that
// continues.
void wait_max(int miliseconds) {
    usleep((rand() % miliseconds)*1000);
}

// Count number of molecules that will be created
int predicted_number_of_molecules(unsigned int NO, unsigned int NH) {
    unsigned int num_of_molecules = NO;
    if(NH/2 < num_of_molecules) {
        num_of_molecules = NH / 2;
    }

    return num_of_molecules;
}

/** MAIN FUNCTIONS **/ 

// Initialization function
void init() {
    // Unlink semaphores in case previous program crash
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_OXYGEN);
    sem_unlink(SEM_HYDROGEN);
    sem_unlink(SEM_WRITING);
    sem_unlink(SEM_BARRIER_TURNSTILE);
    sem_unlink(SEM_BARRIER_TURNSTILE_2);
    sem_unlink(SEM_BARRIER_MUTEX);

    // Opening/creating file for output text
	output_file = fopen("proj2.out","w");
	if(output_file == NULL) {
		fprintf(stderr,"File proj2.out can not be open!\n");
		exit(FAILURE);
	}

    // Create semaphores and handle errors
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        end_program_with_error("Semaphore was not created!");
    if ((sem_oxygen = sem_open(SEM_OXYGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) 
		end_program_with_error("Semaphore was not created!");
    if ((sem_hydrogen = sem_open(SEM_HYDROGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) 
		end_program_with_error("Semaphore was not created!");
    if ((sem_writing = sem_open(SEM_WRITING, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) 
		end_program_with_error("Semaphore was not created!");
    if ((sem_barrier_turnstile = sem_open(SEM_BARRIER_TURNSTILE, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
		end_program_with_error("Semaphore was not created!");
    if ((sem_barrier_turnstile_2 = sem_open(SEM_BARRIER_TURNSTILE_2, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) 
		end_program_with_error("Semaphore was not created!");
    if ((sem_barrier_mutex = sem_open(SEM_BARRIER_MUTEX, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) 
		end_program_with_error("Semaphore was not created!");

    // Create memory
	data = mmap(NULL, sizeof(data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
	if(data == MAP_FAILED) 
        end_program_with_error("Shared memory was not created.");
	
    // Preset data for proper operations
    data->bonding_atoms = 0;
    data->molecules = 1;
    data->queueing_atoms = 0;
}

// Final function
void end() {
	// Destroy shared memory
	if(munmap(data, sizeof(data_t)) == ERROR) 
		end_program_with_error("Cannot destroy shared memory.");

    // Close semaphores
    if((sem_close(sem_mutex)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_oxygen)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_hydrogen)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_writing)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_barrier_turnstile)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_barrier_turnstile_2)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");
    if((sem_close(sem_barrier_mutex)) == ERROR) 
		end_program_with_error("Semaphore was not closed.");

    // Unlink semaphores
	if((sem_unlink(SEM_MUTEX)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_OXYGEN)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_HYDROGEN)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_WRITING)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_BARRIER_TURNSTILE)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_BARRIER_TURNSTILE_2)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");
    if((sem_unlink(SEM_BARRIER_MUTEX)) == ERROR) 
		end_program_with_error("Semaphore was not unlink.");

    // Close output file
	int file_close = fclose(output_file);
	if(file_close == EOF) 
		end_program_with_error("Output file was not closed.");
}

// Function will create molecule from 
// two hydrogens and one oxygen.
void create_H2O(char atom, int num, int time_wait) {
    // Waiting for the arrival of two hydrogens
    // and one oxygen
    sem_wait(sem_barrier_mutex);
        data->queueing_atoms++;
        data->bonding_atoms++;

        if(data->queueing_atoms == ATOMS_IN_MOLECULE) {
            sem_wait(sem_barrier_turnstile_2);
            sem_post(sem_barrier_turnstile);
        }

        calculate_molecule_number();
 
        // Set static molecule number to avoid change of molecule ID
        int static_molecule_number = data->molecules;
    sem_post(sem_barrier_mutex);
    
    // Starting creating molecule, than waiting random time
    write_down(atom, num, "creating molecule", static_molecule_number, "");
    wait_max(time_wait);
    sem_wait(sem_barrier_turnstile);
    sem_post(sem_barrier_turnstile);

    // Molecule is created
    write_down(atom, num, "molecule", static_molecule_number, "created");

    // Waiting while all atoms are used in molecule
    sem_wait(sem_barrier_mutex);
        data->queueing_atoms--;
        if(data->queueing_atoms == 0) {
            sem_wait(sem_barrier_turnstile);
            sem_post(sem_barrier_turnstile_2);
        }
    sem_post(sem_barrier_mutex);

    // Semaphore for end of molecule creating
    sem_wait(sem_barrier_turnstile_2);
    sem_post(sem_barrier_turnstile_2);
}

// Create oxygen
void create_oxygen(unsigned int count_oxygen, unsigned int num_of_molecules, arguments_t arg) {
    srand(time(NULL) * getpid());
    write_down('O', count_oxygen, "started", 0, "");
    wait_max(arg.TI);
    write_down('O', count_oxygen, "going to queue", 0, "");

    // Semaphore to make the calculation of atoms in a queue correct        
    sem_wait(sem_mutex);
    data->oxygen++;
        
    // There is enough atoms to form a molecule
    if(num_of_molecules >= data->oxygen) {
        data->act_oxygen++;

        // Check if there is enought atoms to create molecule
        if(data->act_hydrogen >= 2) {
            sem_post(sem_hydrogen);
            sem_post(sem_hydrogen);
            data->act_hydrogen = data->act_hydrogen - 2;
            sem_post(sem_oxygen);
            data->act_oxygen--;
        }
        else 
            sem_post(sem_mutex);
             
        sem_wait(sem_oxygen);
        create_H2O('O', count_oxygen, arg.TB);
        sem_post(sem_mutex);
    }

    // There is not enough atoms to form a molecule
    else {
        write_down('O', count_oxygen, "not enough H", 0, "");
        sem_post(sem_mutex);
    }
            
    exit(SUCCESS); 
}

// Create hydrogen
void create_hydrogen(unsigned int count_hydrogen, unsigned int num_of_molecules, arguments_t arg) {
    srand(time(NULL) * getpid());
    write_down('H', count_hydrogen, "started", 0, "");
    wait_max(arg.TI);
    write_down('H', count_hydrogen, "going to queue", 0, "");

    // Semaphore to make the calculation of atoms in a queue correct             
    sem_wait(sem_mutex);      

    data->hydrogen++;
    if(num_of_molecules * 2 >= data->hydrogen) {            
        data->act_hydrogen++;

        // Check if there is enought atoms to create molecule
        if(data->act_hydrogen >= 2 && data->act_oxygen >= 1) {
            sem_post(sem_hydrogen);
            sem_post(sem_hydrogen);
            data->act_hydrogen = data->act_hydrogen - 2;
            sem_post(sem_oxygen);
            data->act_oxygen--;                
        }
        else    
            sem_post(sem_mutex);
              
        sem_wait(sem_hydrogen);
        create_H2O('H', count_hydrogen, arg.TB);
    }
    else {
        write_down('H', count_hydrogen, "not enough O or H", 0, "");
        sem_post(sem_mutex);
    }  

    exit(SUCCESS); 
}

/** MAIN FUNCTION **/

int main(int argc, char *argv[]) {
    // Check arguments, do initialization procedures
    // and count number of molecules that will be created
    arguments_t arg = check_arguments(argc, &(*argv));
    init();
    unsigned int num_of_molecules = predicted_number_of_molecules(arg.NO, arg.NH);


    // Create oxygens
    for(unsigned int count_oxygen = 1; count_oxygen <= arg.NO; count_oxygen++)
        if(fork() == 0)   
            create_oxygen(count_oxygen, num_of_molecules, arg);  

    // Create hydrogens
    for(unsigned int count_hydrogen = 1; count_hydrogen <= arg.NH; count_hydrogen++) 
        if(fork() == 0)
            create_hydrogen(count_hydrogen, num_of_molecules, arg);
                    

    // Wait while all forks are done
    for(unsigned int i = 0; i < arg.NH+arg.NO; i++)
        wait(NULL);
    while(wait(NULL) > 0);

    return SUCCESS; 
}