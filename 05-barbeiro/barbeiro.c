#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BARBEIROS 4
#define CADEIRAS 10
#define CLIENTES 200

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t clientes_sem;
sem_t barbeiros_sem;

int esperando = 0;
int atendidos = 0;
int dispensados = 0;

void *barbeiro(void *ptr);
void *cliente(void *prt);
void fazer_corte();
void receber_corte();

int main() {

    printf("========== BARBEIRO SONOLENTO ==========\n\r");
    sem_init(&clientes_sem, 0, 0);
    sem_init(&barbeiros_sem, 0, 0);

    pthread_t barbeiros[BARBEIROS];
    pthread_t clientes[CLIENTES];

    for (int i = 0; i < BARBEIROS; i++) {
        pthread_create(&barbeiros[i], NULL, barbeiro, (void *)(intptr_t)i);
    }
    for (int i = 0; i < CLIENTES; i++) {
        usleep(rand() % 20000);
        pthread_create(&clientes[i], NULL, cliente, (void *)(intptr_t)i);
    }
    for (int i = 0; i < CLIENTES; i++)
        pthread_join(clientes[i], NULL);

    for (int i = 0; i < BARBEIROS; i++)
        pthread_cancel(barbeiros[i]);
    for (int i = 0; i < BARBEIROS; i++)
        pthread_join(barbeiros[i], NULL);

    printf("Atendidos: %d/%d | Dispensados: %d/%d\n", atendidos, CLIENTES,
           dispensados, CLIENTES);
}

void *barbeiro(void *ptr) {
    while (true) {
        sem_wait(&clientes_sem);
        pthread_mutex_lock(&lock);
        esperando--;
        pthread_mutex_unlock(&lock);
        sem_post(&barbeiros_sem);
        fazer_corte();
    }
}

void *cliente(void *ptr) {
    pthread_mutex_lock(&lock);
    if (CADEIRAS > esperando) {
        esperando++;
        sem_post(&clientes_sem);
        pthread_mutex_unlock(&lock);
        sem_wait(&barbeiros_sem);
        receber_corte();
        pthread_mutex_lock(&lock);
        atendidos++;
        pthread_mutex_unlock(&lock);
    } else {
        dispensados++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void fazer_corte() { usleep(rand() % 100000); }

void receber_corte() { usleep(rand() % 100000); }
