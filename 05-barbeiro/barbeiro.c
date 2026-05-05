#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BARBEIROS 2
#define CADEIRAS 10

sem_t mutex;
sem_t clientes_sem;
sem_t barbeiros_sem;

int esperando = 0;

void *barbeiro_puro(void *ptr);
void *barbeiro_protegido(void *ptr);
void *cliente_puro(void *prt);
void *cliente_protegido(void *prt);

int main() {

    printf("========== BARBEIRO SONOLENTO - PURO ==========\n\r");
    sem_init(&mutex, 0, 1);
    sem_init(&clientes_sem, 0, 0);
    sem_init(&barbeiros_sem, 0, 0);

    pthread_t barbeiros[BARBEIROS];
    pthread_t cliente;

    for (int i = 0; i < BARBEIROS; i++) {
        pthread_create(&barbeiros[i], NULL, barbeiro_puro, (void *)(intptr_t)i);
    }
    pthread_create(&cliente, NULL, cliente_puro, NULL);

    for (int i = 0; i < BARBEIROS; i++) {
        pthread_join(&barbeiros[i], NULL);
    }
    pthread_join(&cliente, NULL);
}

void *barbeiro_puro(void *ptr) {
    int id = (intptr_t)ptr;
    sem_wait(&barbeiros_sem);
}

void *barbeiro_protegido(void *ptr) {}
