#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>   // pour les flags O_CREAT, O_EXCL, ..
#include <unistd.h>  // usleep
#include <time.h>

#define BUFFER_SIZE 3
#define NB_PARENT 1
#define NB_CHILD 1  // NON NUL
#define EAT_TIMES 5 * NB_CHILD


// protects the access to buffer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {EMPTY = 0, CUPCAKE, MUFFIN, PANCAKE, TOTAL} plate;
plate * tampon[BUFFER_SIZE];

sem_t * full; // presence of a buffer containing a message ?
sem_t * empty;  // limits to BUFFER_SIZE the number of buffers

// ANSWER TO QUESTION: This program does not work like the
// consumer producer model for the simple reason that semaphores and mutex
// are declared, initialized, but not used!
// The producer or the consumer has access without control to the list of
// buffers. Their request to an empty or full buffer may fail and they end up
// with a NULL pointer which ends the program.
// Moreover they make concurrent access to the buffer.

// returns a string describing the contents of the plate
const char* type(plate * a) {
	switch (*a) {
		case EMPTY:
			return "empty";
		case CUPCAKE:
			return "cupcake";
		case MUFFIN:
			return "muffin";
		case PANCAKE:
			return "pancake";
		default:
			return "unknown tric";
	}
}

plate * obtain(plate status) {
	plate * a;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (tampon[i] != NULL && ((status == EMPTY) == (*tampon[i] == EMPTY))) {
			a = tampon[i];
			tampon[i] = NULL;
			return a;
		}
	}
	return NULL;
}

void put(plate * a) {
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (tampon[i] == NULL) {
			tampon[i] = a;
			return;
		}
	}
}

void consum(plate * a) {
	*a = EMPTY;
}


// the parent represents the producer
void* parent (void *arg) {
	int id = *((int*) arg);
	srand(time(NULL));
    plate *tp;
    int i;
    for (i = 0; i < EAT_TIMES; i++) {
        printf("The parent %d waits for an empty plate\n", id);
		// it retrieves an empty buffer from the list of buffers
        tp = obtain(EMPTY);

		// in case of error, the returned buffer is a null pointer
        if (tp == NULL) {
        	fprintf(stderr, "Error, no plate available for parent %d", id);
        	exit(EXIT_FAILURE); // the program ends in this case
        }

        // he draws a random number to represent the prepared plate
        *tp = (rand() % (TOTAL - 1)) + 1;
        printf("The parent %d prepared a %s\n", id, type(tp));
        usleep(10000);

		// it puts the filled buffer in the buffer list
        put(tp);
    }
    return NULL;
}

// The kid represents the consumer
void* kid (void *arg) {
	int id = *((int*) arg);
    plate *tp;
    int i;
    for (i = 0; i < EAT_TIMES*NB_PARENT/NB_CHILD; i++) {
    	printf("The kid %d waits for a full plate\n", id);
		// it retrieves a non-empty (full) buffer from the buffer list
		tp = obtain(!EMPTY);

		// in case of error, the returned buffer is a null pointer
        if (tp == NULL) {
        	fprintf(stderr, "Error, no plate is available for the kid %d\n", id);
        	exit(EXIT_FAILURE); // the program ends in this case
        }

        // it consums the contents of buffer
        printf("The kid %d eats a %s\n", id, type(tp));
        consum(tp);
        usleep(10000);

		// it puts the emptied buffer in the buffer list
        put(tp);
    }
    return NULL;
}

int main() {
	int i;
    pthread_t parents[NB_PARENT];
    int parents_id[NB_PARENT];
    pthread_t kids[NB_CHILD];
	int kids_id[NB_CHILD];

	// we create empty plates
	for (i = 0; i < BUFFER_SIZE; i++) {
		tampon[i] = calloc(1, sizeof(plate));
	}
	// creation des semaphores
	full = sem_open("full", O_CREAT | O_EXCL, 755,  0);
	empty = sem_open("empty", O_CREAT | O_EXCL, 755,  BUFFER_SIZE);
	if (full == SEM_FAILED || empty == SEM_FAILED) {
		sem_unlink("full");
		sem_unlink("empty");
		printf("semaphores existants éliminés !\n");
		return EXIT_FAILURE;
	}

	// creation des threads kids
	for (i = 0; i < NB_CHILD; i++) {
		kids_id[i] = i;
    	pthread_create(&kids[i], NULL, kid, (void*) &kids_id[i]);
    }
    // creation des threads parents
    for (i = 0; i < NB_PARENT; i++) {
		parents_id[i] = i;
    	pthread_create(&parents[i], NULL, parent, (void*) &parents_id[i]);
    }

	// on attend les threads kids
	for (i = 0; i < NB_CHILD; i++) {
    	pthread_join(kids[i], NULL);
    }
    // on attend les threads parents
    for (i = 0; i < NB_PARENT; i++) {
    	pthread_join(parents[i], NULL);
    }
    // on libère les plates
    for (i = 0; i < BUFFER_SIZE; i++) {
    	free(tampon[i]);
    }
	// we destroy semaphores and mutex
	sem_close(full);
	sem_close(empty);
	sem_unlink("full");
	sem_unlink("empty");
    pthread_mutex_destroy(&mutex);
    return 0;
}
