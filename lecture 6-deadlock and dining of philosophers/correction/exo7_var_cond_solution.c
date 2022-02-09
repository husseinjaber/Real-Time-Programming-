#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NB_FOURNEE 4
#define NB_GATEAUX 4
#define NB_ENFANTS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gateaux_prets = PTHREAD_COND_INITIALIZER;
int gateaux = 0;
int stop = 0;

void* enfant(void *arg) { // mange les gateaux
	int id = *((int*) arg);
	int mange = 0;
    while(1) { // il fait une boucle infinie, qui s'arrête si stop = 1
    	// il vérrouille le mutex car il veut modifier gateaux (en manger un)
		pthread_mutex_lock(&mutex);

		/* ICI IL FAIT UNE ATTENTE ACTIVE
		 * il va boucler tant qu'il n'y a pas de gateaux et tester régulièrement
		 * si la situation a changé, au lieu de se mettre en sommeil et
		 * d'attendre un signal */
		while(gateaux == 0) { // tant qu'il n'y a pas de gateaux ...
			if (stop == 1) { // si stop = 1 il met fin à la boucle
				// il déverrouille le mutex avant de partir...
				pthread_mutex_unlock(&mutex);
				goto fin; // il sort
			}
			// pas de gateaux, il débloque le mutex pour que le parent puisse en
			// préparer...
            pthread_mutex_unlock(&mutex);
            usleep(100000); // il lui laisse le temps de les préparer...
            // il revérrouille le mutex pour retenter sa chance
            pthread_mutex_lock(&mutex);
        }
        /* FIN DE L'ATTENTE ACTIVE */

        if (gateaux > 0) { // normalement, il y a bien un gateau
			gateaux--;     // il le mange
			mange++;       // il indique qu'il a mangé un gateau de plus
			printf("L'enfant %d a mangé un gateau\n", id);
        } else {   // ça ne devrait pas arriver...
			printf("L'enfant %d n'a pas eu de gateau\n", id);
		}
		pthread_mutex_unlock(&mutex);
		sleep(1); // il digère un peu avant de repartir manger...
    }
    fin :
    return *((void**) &mange);
}

void* parent(void *arg) { // prépare les gateaux
    int i;
    for (i = 0; i < NB_FOURNEE; i++) { // il va en préparer plusieurs fournées
   		// il verrouille le mutex pour modifier gateaux
    	pthread_mutex_lock(&mutex);
    	gateaux += NB_GATEAUX; // il ajoute des gateaux
		printf("Le parent a préparé des gateaux\n");
		pthread_mutex_unlock(&mutex); // il déverrouille le mutex
		sleep(2); // il se repose un peu
    }
    pthread_mutex_lock(&mutex); // il verrouille le mutex pour modifier stop
    stop = 1;                   // il indique qu'il n'y aura plus de gateaux
    pthread_mutex_unlock(&mutex); // il déverrouille le mutex
    return NULL;
}

int main() {
	int i, nb[NB_ENFANTS];
	void* ret[NB_ENFANTS];
    pthread_t tid[NB_ENFANTS + 1];

	for (i = 0; i < NB_ENFANTS; i++) {
		nb[i] = i;
    	pthread_create(&tid[i], NULL, enfant, (void*) &nb[i]);
    }
    pthread_create(&tid[i], NULL, parent, NULL);

	for (i = 0; i < NB_ENFANTS; i++) {
    	pthread_join(tid[i], &ret[i]);
    	printf("L'enfant %d a mangé %d gateaux\n", i, *((int*) &ret[i]));
    }
    pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}
