#include <stdio.h>   // printf
#include <pthread.h> // pthread
#include <unistd.h>  // sleep
#include <stdlib.h>  // srand
#include <time.h>    // time

char data = 'A';
int nb_lect = 0;
pthread_mutex_t verrou_lecteur = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verrou_ecrivain = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fifo = PTHREAD_MUTEX_INITIALIZER;


void* lecteur(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Lecteur %d veut consulter la ressource\n", id);

		pthread_mutex_lock(&fifo);
		pthread_mutex_lock(&verrou_lecteur);
		nb_lect++;
		if (nb_lect == 1)
			pthread_mutex_lock(&verrou_ecrivain);
		pthread_mutex_unlock(&verrou_lecteur);
		pthread_mutex_unlock(&fifo);

		printf("Lecteur %d consulte la ressource : %c\n", id, data);
		sleep(rand() % 2); // on simule un traitement long

		pthread_mutex_lock(&verrou_lecteur);
		printf("Lecteur %d a fini de consulter la ressource : %c\n", id, data);
		nb_lect--;
		if(nb_lect == 0)
			pthread_mutex_unlock(&verrou_ecrivain);
		pthread_mutex_unlock(&verrou_lecteur);

		sleep(rand() % 3); // le lecteur fait une pause
	}
	printf("Lecteur %d : s'en va\n", id);
	return NULL;
}

void* ecrivain(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Ecrivain %d veut modifier la ressource\n", id);

		pthread_mutex_lock(&fifo);
		pthread_mutex_lock(&verrou_ecrivain);
		pthread_mutex_unlock(&fifo);

		printf("Ecrivain %d modifie la ressource : %c\n", id, data);
		sleep(rand() % 2); // on simule un traitement long
		data++;
		printf("Ecrivain %d : maintenant la ressource est %c\n", id, data);

		pthread_mutex_unlock(&verrou_ecrivain);

		sleep(rand() % 3); // l'écrivain fait une pause
	}
	printf("Ecrivain %d : s'en va\n", id);
	return NULL;
}

int main() {
	int i, nb[8];
	srand(time(NULL)); // initialisation de rand
	pthread_t tid[8];

	for (i = 0; i < 5; i++) {
		nb[i] = i;
		pthread_create(&tid[i], NULL, lecteur, (void*) &nb[i]);
	}
	for (i = 0; i < 3; i++) {
		nb[i+5] = i;
		pthread_create(&tid[i+5], NULL, ecrivain, (void*) &nb[i+5]);
	}

	for (i = 0; i < 8; i++) {
		pthread_join(tid[i], NULL);
	}
	puts("Consultation et modifications terminées");

	pthread_mutex_destroy(&verrou_lecteur);
	pthread_mutex_destroy(&verrou_ecrivain);
	pthread_mutex_destroy(&fifo);
	return 0;
}

