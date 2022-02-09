#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // pour les semaphores

// compile avec
// gcc -Wall abc_2.c -lpthread

sem_t turnA;
sem_t turnR;
sem_t turnG;

void* a(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(&turnA);
        printf("A");
        fflush(stdout);
        sem_post(&turnR);
    }
    return NULL;
}

void* r(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(&turnR);
        printf("R");
        fflush(stdout);
        sem_post(&turnG);
    }
    return NULL;
}

void* g(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(&turnG);
        printf("G");
        fflush(stdout);
        sem_post(&turnA);
    }
    return NULL;
}

int main() {
    pthread_t ID[2];

    //sem_init(&turnA, 0, 0);
    sem_init(&turnA, 0, 1);
    sem_init(&turnR, 0, 0);
    sem_init(&turnG, 0, 0);

    pthread_create(&ID[0], NULL, r, NULL);
    pthread_create(&ID[1], NULL, g, NULL);

    //sem_post(&turnA); // si turnA initialisé à zéro
    a(NULL);

    pthread_join(ID[0], NULL);
    pthread_join(ID[1], NULL);

    sem_destroy(&turnA);
    sem_destroy(&turnR);
    sem_destroy(&turnG);

    return 0;
}
