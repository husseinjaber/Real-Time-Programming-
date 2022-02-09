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


// protège l'accès au tampon
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {VIDE = 0, CUPCAKE, MUFFIN, PANCAKE, TOTAL} assiette;
assiette * tampon[BUFFER_SIZE];

sem_t * plein; // présence d'un tampon contenant un message ?
sem_t * vide;  // limite à BUFFER_SIZE le nombre de tampons

// REPONSE A LA QUESTION : Ce programme ne fonctionne pas comme le modèle
// producteur consommateur pour la simple raison que les sémaphores et le mutex
// sont déclarés, initialisés, mais pas utilisés !
// Le producteur ou le consommateur accèdent sans contrôle à la liste des
// tampons. Leur demande de tampon vide ou plein peut échouer et ils se retrouve
// avec un pointeur NULL ce qui met fin au programme.
// De plus ils font des accès concurents au tampon.


// retourne une chaine de caractère décrivant le contenu de l'assiette
const char* type(assiette * a) {
	switch (*a) {
		case VIDE:
			return "vide";
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

assiette * obtenir(assiette status) {
	assiette * a;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (tampon[i] != NULL && ((status == VIDE) == (*tampon[i] == VIDE))) {
			a = tampon[i];
			tampon[i] = NULL;
			return a;
		}
	}
	return NULL;
}

void placer(assiette * a) {
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (tampon[i] == NULL) {
			tampon[i] = a;
			return;
		}
	}
}

void consommer(assiette * a) {
	*a = VIDE;
}


// le parent représente le producteur
void* parent (void *arg) {
	int id = *((int*) arg);
	srand(time(NULL));
    assiette *tp;
    int i;
    for (i = 0; i < EAT_TIMES; i++) {
        printf("Le parent %d attend une assiette vide\n", id);
		// il récupère un tampon vide dans la liste des tampons
        tp = obtenir(VIDE);

		// en cas d'erreur, le tampon retourné est un pointeur NULL
        if (tp == NULL) {
        	fprintf(stderr, "Erreur, aucune assiette disponible pour le parent %d", id);
        	exit(EXIT_FAILURE); // on met fin au programme dans ce cas
        }

        // il tire un nombre au hasard pour représenter le plat préparé
        *tp = (rand() % (TOTAL - 1)) + 1;
        printf("Le parent %d a préparé un %s\n", id, type(tp));
        usleep(10000);

		// il replace le tampon rempli dans la liste des tampons
        placer(tp);
    }
    return NULL;
}

// l'enfant représente le consommateur
void* enfant (void *arg) {
	int id = *((int*) arg);
    assiette *tp;
    int i;
    for (i = 0; i < EAT_TIMES*NB_PARENT/NB_CHILD; i++) {
    	printf("L'enfant %d attend une assiette pleine\n", id);
		// il récupère un tampon non vide (plein) dans la liste des tampons
        tp = obtenir(!VIDE);

		// en cas d'erreur, le tampon retourné est un pointeur NULL
        if (tp == NULL) {
        	fprintf(stderr, "Erreur, aucune assiette disponible pour l'enfant %d\n", id);
        	exit(EXIT_FAILURE); // on met fin au programme dans ce cas
        }

        // il consomme le contenu du tampon
        printf("L'enfant %d mange un %s\n", id, type(tp));
        consommer(tp);
        usleep(10000);

		// il replace le tampon vidé dans la liste des tampons
        placer(tp);
    }
    return NULL;
}

int main() {
	int i;
    pthread_t parents[NB_PARENT];
    int parents_id[NB_PARENT];
    pthread_t enfants[NB_CHILD];
	int enfants_id[NB_CHILD];

	// on cree les assiettes vides
	for (i = 0; i < BUFFER_SIZE; i++) {
		tampon[i] = calloc(1, sizeof(assiette));
	}
	// creation des semaphores
	plein = sem_open("plein", O_CREAT | O_EXCL, 755,  0);
	vide = sem_open("vide", O_CREAT | O_EXCL, 755,  BUFFER_SIZE);
	if (plein == SEM_FAILED || vide == SEM_FAILED) {
		sem_unlink("plein");
		sem_unlink("vide");
		printf("semaphores existants éliminés !\n");
		return EXIT_FAILURE;
	}

	// creation des threads enfants
	for (i = 0; i < NB_CHILD; i++) {
		enfants_id[i] = i;
    	pthread_create(&enfants[i], NULL, enfant, (void*) &enfants_id[i]);
    }
    // creation des threads parents
    for (i = 0; i < NB_PARENT; i++) {
		parents_id[i] = i;
    	pthread_create(&parents[i], NULL, parent, (void*) &parents_id[i]);
    }

	// on attend les threads enfants
	for (i = 0; i < NB_CHILD; i++) {
    	pthread_join(enfants[i], NULL);
    }
    // on attend les threads parents
    for (i = 0; i < NB_PARENT; i++) {
    	pthread_join(parents[i], NULL);
    }
    // on libère les assiettes
    for (i = 0; i < BUFFER_SIZE; i++) {
    	free(tampon[i]);
    }
	// on detruit les semaphores et le mutex
	sem_close(plein);
	sem_close(vide);
	sem_unlink("plein");
	sem_unlink("vide");
    pthread_mutex_destroy(&mutex);
    return 0;
}
