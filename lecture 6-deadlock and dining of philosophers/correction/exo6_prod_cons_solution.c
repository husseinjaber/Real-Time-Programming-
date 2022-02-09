#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>   // pour les flags O_CREAT, O_EXCL, ..
#include <unistd.h>  // usleep
#include <time.h>

#define BUFFER_SIZE 3
#define EAT_TIMES 10
#define NB_PARENT 3
#define NB_CHILD 3

// The program can work with any number of producers and
// consumers.
// In case their number is greater than the size of the buffer list,
// they just might spend a lot of time waiting.

// protège l'accès au tampon
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {empty = 0, CUPCAKE, MUFFIN, PANCAKE, TOTAL} plate;
plate * tampon[BUFFER_SIZE];

sem_t * full; // presence of a buffer having a message ?
sem_t * empty;  // limitation to BUFFER_SIZE the numbre of buffers

// return a string describing the content of the plate
const char* type(plate * a) {
	switch (*a) {
		case empty:
			return "empty";
		case CUPCAKE:
			return "cupcake";
		case MUFFIN:
			return "muffin";
		case PANCAKE:
			return "pancake";
		default:
			return "truc inconnu";
	}
}

plate * obtain(plate status) {
	plate * a;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (tampon[i] != NULL && ((status == empty) == (*tampon[i] == empty))) {
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
	*a = empty;
}



void* parent (void *arg) { // prepare the cakes
	int id = *((int*) arg);
	srand(time(NULL));
    plate *tp;
    int i;
    for (i = 0; i < EAT_TIMES; i++) {
        printf("The parent %d waits for an empty plate\n", id);
        sem_wait(empty);   // is there a free buffer ?
        pthread_mutex_lock(&mutex);   // exclusive access to tampon/buffer
        tp = obtain(empty);           // obtainingg a buffer
        pthread_mutex_unlock(&mutex); // end of the exclusive access to buffer
        if (tp == NULL) {
        	fprintf(stderr, "Error, no plate available for the parent %d", id);
        	exit(EXIT_FAILURE);
        }
        *tp = (rand() % (TOTAL - 1)) + 1; // creation un nouvel élément;
        printf("The parent %d prepares a %s\n", id, type(tp));
        usleep(10000);
        pthread_mutex_lock(&mutex);   // acces exclusif au tampon
        put(tp);                   // met tampon dans liste des tampons
        pthread_mutex_unlock(&mutex); // fin de l'accès exclusif au tampon
        sem_post(full);   // signaling the presence of a full buffer/tampon
    }
    return NULL;
}

void* child (void *arg) { // eat the cakes
	int id = *((int*) arg);
    plate *tp;
    int i;
    for (i = 0; i < EAT_TIMES*NB_PARENT/NB_CHILD; i++) {
    	printf("L'child %d attend une plate fulle\n", id);
        sem_wait(full);   // Is there a message to consume ?
        pthread_mutex_lock(&mutex);   // acces exclusif au tampon
        tp = obtain(1);              // obtaining a full buffer
        pthread_mutex_unlock(&mutex); // fin de l'accès exclusif au tampon
        if (tp == NULL) {
        	fprintf(stderr, "Error, no plate is available to the child %d\n", id);
        	exit(EXIT_FAILURE);
        }
        printf("The child %d eats a %s\n", id, type(tp));
        consum(tp);
        usleep(10000);
        pthread_mutex_lock(&mutex);   // acces exclusif au tampon
        put(tp);                   // put buffer in the list of buffers
        pthread_mutex_unlock(&mutex); // fin de l'accès exclusif au tampon
        sem_post(empty);    // signaling the release of the consumed buffer
    }
    return NULL;
}

int main() {
	int i;
    pthread_t parents[NB_PARENT];
    int parents_id[NB_PARENT];
    pthread_t childs[NB_CHILD];
	int childs_id[NB_PARENT];

	// on cree les plates emptys
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

	// creation des threads childs
	for (i = 0; i < NB_CHILD; i++) {
		childs_id[i] = i;
    	pthread_create(&childs[i], NULL, child, (void*) &childs_id[i]);
    }
    // creation des threads parents
    for (i = 0; i < NB_PARENT; i++) {
		parents_id[i] = i;
    	pthread_create(&parents[i], NULL, parent, (void*) &parents_id[i]);
    }

	// on attend les threads childs
	for (i = 0; i < NB_CHILD; i++) {
    	pthread_join(childs[i], NULL);
    }
    // on attend les threads parents
    for (i = 0; i < NB_PARENT; i++) {
    	pthread_join(parents[i], NULL);
    }
    // on libère les plates
    for (i = 0; i < BUFFER_SIZE; i++) {
    	free(tampon[i]);
    }
	// on detruit les semaphores et le mutex
	sem_close(full);
	sem_close(empty);
	sem_unlink("full");
	sem_unlink("empty");
    pthread_mutex_destroy(&mutex);
    return 0;
}
