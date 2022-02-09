Two groups compete for access to a resource. The members of a group can access the resource at the same time: we are close to a reader / writer model with two groups of readers.

The algo in pseudo code is the following:
- the reader part is duplicated by distinguishing the number of readers and
   the mutex of each group by numbers 1 and 2
- we replace the occurrences of the lock_write by the lock_read of
   the other group

int nb_lect1 = 0;
int nb_lect2 = 0;
pthread_mutex_t verrou_lecteur1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verrou_lecteur2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fifo = PTHREAD_MUTEX_INITIALIZER;


void* lecteur1(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Etudiant %d veut utiliser la salle\n", id);

		pthread_mutex_lock(&fifo);
		pthread_mutex_lock(&verrou_lecteur1);
		nb_lect1++;
		if (nb_lect1 == 1)
			pthread_mutex_lock(&verrou_lecteur2);
		pthread_mutex_unlock(&verrou_lecteur1);
		pthread_mutex_unlock(&fifo);

		printf("Etudiant %d utilise la salle\n", id);
		sleep(rand() % 2); // on simule un traitement long

		pthread_mutex_lock(&verrou_lecteur1);
		printf("Etudiant %d a fini d'utiliser la salle\n", id);
		nb_lect1--;
		if(nb_lect1 == 0)
			pthread_mutex_unlock(&verrou_lecteur2);
		pthread_mutex_unlock(&verrou_lecteur1);

		sleep(rand() % 3); // le lecteur fait une pause
	}
	printf("Etudiant %d : s'en va\n", id);
	return NULL;
}

void* lecteur2(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Etudiant %d veut utiliser la salle\n", id);

		pthread_mutex_lock(&fifo);
		pthread_mutex_lock(&verrou_lecteur2);
		nb_lect2++;
		if (nb_lect2 == 1)
			pthread_mutex_lock(&verrou_lecteur1);
		pthread_mutex_unlock(&verrou_lecteur2);
		pthread_mutex_unlock(&fifo);

		printf("Etudiant %d utilise la salle\n", id);
		sleep(rand() % 2); // on simule un traitement long

		pthread_mutex_lock(&verrou_lecteur2);
		printf("Etudiant %d a fini d'utiliser la salle\n", id);
		nb_lect2--;
		if(nb_lect2 == 0)
			pthread_mutex_unlock(&verrou_lecteur1);
		pthread_mutex_unlock(&verrou_lecteur2);

		sleep(rand() % 3); // le lecteur fait une pause
	}
	printf("Etudiant %d : s'en va\n", id);
	return NULL;
}

int main() {
	int i, nb[8];
	srand(time(NULL)); // initialisation de rand
	pthread_t tid[8];

	for (i = 0; i < 5; i++) { // 5 étudiants dans le groupe A
		nb[i] = 10 + i;
		pthread_create(&tid[i], NULL, lecteur1, (void*) &nb[i]);
	}
	for (; i < 8; i++) { // 3 étudiants dans le groupe B
		nb[i] = 20 + (i-5);
		pthread_create(&tid[i], NULL, lecteur2, (void*) &nb[i]);
	}

	for (i = 0; i < 8; i++) {
		pthread_join(tid[i], NULL);
	}
	puts("Utilisation de la salle par les deux groupe terminée !");

	pthread_mutex_destroy(&verrou_lecteur1);
	pthread_mutex_destroy(&verrou_lecteur2);
	pthread_mutex_destroy(&fifo);
	return 0;
}

/*
$ ./a.out
Etudiant 10 veut utiliser la salle     // 10 attend
Etudiant 11 veut utiliser la salle     // 11 attend
Etudiant 12 veut utiliser la salle     // 12 attend
Etudiant 13 veut utiliser la salle     // 13 attend
Etudiant 20 veut utiliser la salle     // 20 attend
Etudiant 14 veut utiliser la salle     // 14 attend
Etudiant 21 veut utiliser la salle     // 21 attend
Etudiant 22 veut utiliser la salle     // 22 attend
Etudiant 10 utilise la salle                   // 10 accède
Etudiant 11 utilise la salle                   // 11 accède
Etudiant 12 utilise la salle                   // 12 accède
Etudiant 13 utilise la salle                   // 13 accède
Etudiant 10 a fini d'utiliser la salle
Etudiant 12 a fini d'utiliser la salle
Etudiant 12 veut utiliser la salle                        // 12 attend
Etudiant 11 a fini d'utiliser la salle
Etudiant 10 veut utiliser la salle                        // 10 attend
Etudiant 13 a fini d'utiliser la salle
Etudiant 20 utilise la salle                  // 20 accède
Etudiant 11 veut utiliser la salle                         // 11 attend
Etudiant 20 a fini d'utiliser la salle
Etudiant 20 veut utiliser la salle                         // 20 attend
Etudiant 14 utilise la salle                   // 14 accède
Etudiant 13 veut utiliser la salle                         // 13 attend
Etudiant 14 a fini d'utiliser la salle
Etudiant 21 utilise la salle                   // 21 accède
Etudiant 22 utilise la salle                   // 22 accède
Etudiant 21 a fini d'utiliser la salle
Etudiant 22 a fini d'utiliser la salle
Etudiant 22 veut utiliser la salle                        // 22 attend
Etudiant 12 utilise la salle                                      // 12 accède
Etudiant 10 utilise la salle                                      // 10 accède
Etudiant 12 a fini d'utiliser la salle
Etudiant 10 a fini d'utiliser la salle
Etudiant 11 utilise la salle                                       // 11 accède
Etudiant 14 veut utiliser la salle                        // 14 attend
Etudiant 10 veut utiliser la salle
Etudiant 11 a fini d'utiliser la salle
Etudiant 20 utilise la salle                                       // 20 accède
Etudiant 20 a fini d'utiliser la salle
Etudiant 13 utilise la salle                                       // 13 accède
Etudiant 13 a fini d'utiliser la salle
Etudiant 22 utilise la salle                                       // 22 accède
Etudiant 22 a fini d'utiliser la salle
Etudiant 14 utilise la salle                                       // 14 accède
Etudiant 10 utilise la salle
Etudiant 10 a fini d'utiliser la salle
Etudiant 12 veut utiliser la salle
Etudiant 21 veut utiliser la salle
Etudiant 12 utilise la salle
Etudiant 11 veut utiliser la salle
Etudiant 22 veut utiliser la salle
Etudiant 20 veut utiliser la salle
Etudiant 10 veut utiliser la salle
Etudiant 14 a fini d'utiliser la salle
Etudiant 14 veut utiliser la salle
Etudiant 12 a fini d'utiliser la salle
Etudiant 12 veut utiliser la salle
Etudiant 21 utilise la salle
Etudiant 13 veut utiliser la salle
Etudiant 21 a fini d'utiliser la salle
Etudiant 11 utilise la salle
Etudiant 11 a fini d'utiliser la salle
Etudiant 22 utilise la salle
Etudiant 22 a fini d'utiliser la salle
Etudiant 22 veut utiliser la salle
Etudiant 20 utilise la salle
Etudiant 21 veut utiliser la salle
Etudiant 11 veut utiliser la salle
Etudiant 20 a fini d'utiliser la salle
Etudiant 10 utilise la salle
Etudiant 14 utilise la salle
Etudiant 14 a fini d'utiliser la salle
Etudiant 14 veut utiliser la salle
Etudiant 12 utilise la salle
Etudiant 13 utilise la salle
Etudiant 12 a fini d'utiliser la salle
Etudiant 13 a fini d'utiliser la salle
Etudiant 10 a fini d'utiliser la salle
Etudiant 22 utilise la salle
Etudiant 21 utilise la salle
Etudiant 21 a fini d'utiliser la salle
Etudiant 21 veut utiliser la salle
Etudiant 20 veut utiliser la salle
Etudiant 12 veut utiliser la salle
Etudiant 13 veut utiliser la salle
Etudiant 22 a fini d'utiliser la salle
Etudiant 10 veut utiliser la salle
Etudiant 22 veut utiliser la salle
Etudiant 11 utilise la salle
Etudiant 14 utilise la salle
Etudiant 11 a fini d'utiliser la salle
Etudiant 14 a fini d'utiliser la salle
Etudiant 14 veut utiliser la salle
Etudiant 21 utilise la salle
Etudiant 20 utilise la salle
Etudiant 21 a fini d'utiliser la salle
Etudiant 21 veut utiliser la salle
Etudiant 20 a fini d'utiliser la salle
Etudiant 20 veut utiliser la salle
Etudiant 12 utilise la salle
Etudiant 12 a fini d'utiliser la salle
Etudiant 13 utilise la salle
Etudiant 10 utilise la salle
Etudiant 10 a fini d'utiliser la salle
Etudiant 10 : s'en va
Etudiant 11 veut utiliser la salle
Etudiant 13 a fini d'utiliser la salle
Etudiant 22 utilise la salle
Etudiant 22 a fini d'utiliser la salle
Etudiant 14 utilise la salle
Etudiant 14 a fini d'utiliser la salle
Etudiant 14 : s'en va
Etudiant 21 utilise la salle
Etudiant 20 utilise la salle
Etudiant 12 : s'en va
Etudiant 22 : s'en va
Etudiant 21 a fini d'utiliser la salle
Etudiant 20 a fini d'utiliser la salle
Etudiant 20 : s'en va
Etudiant 11 utilise la salle
Etudiant 11 a fini d'utiliser la salle
Etudiant 11 : s'en va
Etudiant 13 veut utiliser la salle
Etudiant 13 utilise la salle
Etudiant 13 a fini d'utiliser la salle
Etudiant 21 : s'en va
Etudiant 13 : s'en va
Consultation et modifications terminées
 */
