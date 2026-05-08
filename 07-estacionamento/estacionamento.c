#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VAGAS 100
#define CARROS 5000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t vagas_sem;
int ocupado = 0;
int entraram = 0;
int rejeitados = 0;
int max = 0;
int overflows = 0;

void *carro_puro(void *ptr);
void *carro_protegido(void *ptr);
void estacionar();

int main() {
    printf("========== ESTACIONAMENTO - SEM PROTEÇÃO ==========\n\r");
    sem_init(&vagas_sem, 0, VAGAS);
    pthread_t carros[CARROS];
    for (int i = 0; i < CARROS; i++) {
        usleep(rand() % 500);
        pthread_create(&carros[i], NULL, carro_puro, NULL);
    }
    for (int i = 0; i < CARROS; i++)
        pthread_join(carros[i], NULL);
    sem_destroy(&vagas_sem);
    printf("Entraram: %d/%d | Rejeitados: %d/%d | Ocupacao maxima: %d/%d | "
           "Overflows: %d\n",
           entraram, CARROS, rejeitados, CARROS, max, VAGAS, overflows);

    printf("\n========== ESTACIONAMENTO - COM PROTEÇÃO ==========\n\r");
    ocupado = 0;
    entraram = 0;
    rejeitados = 0;
    max = 0;
    sem_init(&vagas_sem, 0, VAGAS);
    for (int i = 0; i < CARROS; i++) {
        usleep(rand() % 500);
        pthread_create(&carros[i], NULL, carro_protegido, NULL);
    }
    for (int i = 0; i < CARROS; i++)
        pthread_join(carros[i], NULL);
    sem_destroy(&vagas_sem);
    printf("Entraram: %d/%d | Rejeitados: %d/%d | Ocupacao maxima: %d/%d\n",
           entraram, CARROS, rejeitados, CARROS, max, VAGAS);

    return 0;
}

void *carro_puro(void *ptr) {
    if (ocupado < VAGAS) {
        usleep(1000);
        ocupado++;
        entraram++;
        if (ocupado > max)
            max = ocupado;
        if (ocupado > VAGAS)
            overflows++;
        estacionar();
        ocupado--;
    } else {
        rejeitados++;
    }
    return NULL;
}

void *carro_protegido(void *ptr) {
    pthread_mutex_lock(&lock);
    if (ocupado < VAGAS) {
        usleep(1000);
        ocupado++;
        entraram++;
        if (ocupado > max)
            max = ocupado;
        pthread_mutex_unlock(&lock);
        estacionar();
        pthread_mutex_lock(&lock);
        ocupado--;
        pthread_mutex_unlock(&lock);
    } else {
        rejeitados++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void estacionar() { usleep(rand() % 500000); }