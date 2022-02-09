#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // pour les semaphores
#include <fcntl.h>     // pour les flags O_CREAT, O_EXCL, ...

sem_t * turnA;
sem_t * turnR;
sem_t * turnG;
sem_t * turnH;

void* a(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(turnA);
        sem_wait(turnA);
        sem_wait(turnA);
        printf("A");
        fflush(stdout);
        sem_post(turnR);
        sem_post(turnR);
    }
    return NULL;
}

void* r(void* p) {
    int i;
    for(i = 0; i < 200; i++) {
        sem_wait(turnR);
        printf("R");
        fflush(stdout);
        sem_post(turnG);
    }
    return NULL;
}

void* g(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(turnG);
        sem_wait(turnG);
        printf("G");
        fflush(stdout);
        sem_post(turnH);
        sem_post(turnH);
        sem_post(turnH);
    }
    return NULL;
}

void* h(void* p) {
    int i;
    for(i = 0; i < 300; i++) {
        sem_wait(turnH);
        printf("H");
        fflush(stdout);
        sem_post(turnA);
    }
    return NULL;
}

int main() {
    pthread_t ID[3];

    turnA = sem_open("turnA", O_CREAT, S_IRUSR | S_IWUSR, 3);
    turnR = sem_open("turnR", O_CREAT, S_IRUSR | S_IWUSR, 0);
    turnG = sem_open("turnG", O_CREAT, S_IRUSR | S_IWUSR, 0);
    turnH = sem_open("turnH", O_CREAT, S_IRUSR | S_IWUSR, 0);

    pthread_create(&ID[0], NULL, r, NULL);
    pthread_create(&ID[1], NULL, g, NULL);
    pthread_create(&ID[2], NULL, h, NULL);

    a(NULL);

    pthread_join(ID[0], 0);
    pthread_join(ID[1], 0);
    pthread_join(ID[2], 0);

    sem_close(turnA);
    sem_close(turnR);
    sem_close(turnG);
    sem_close(turnH);
    sem_unlink("turnA");
    sem_unlink("turnR");
    sem_unlink("turnG");
    sem_unlink("turnH");

    puts("");
    return 0;
}
