#include "prod_cons.h"

circular_buffer cb;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;

int main() {
    // BUFFER SEM PROTEÇÃO
    printf("========== PRODUTOR x CONSUMIDOR - SEM PROTEÇÃO ==========\n\r");
    zera_buffer();
    sem_init(&empty, 0, BUF_LENGTH);
    sem_init(&full, 0, 0);

    pthread_t produtor[PRODUTOR];
    pthread_t consumidor[CONSUMIDOR];

    for (int i = 0; i < PRODUTOR; i++)
        pthread_create(&produtor[i], NULL, &produtor_puro, NULL);

    for (int i = 0; i < CONSUMIDOR; i++)
        pthread_create(&consumidor[i], NULL, &consumidor_puro, NULL);

    for (int i = 0; i < PRODUTOR; i++)
        pthread_join(produtor[i], NULL);

    for (int i = 0; i < CONSUMIDOR; i++)
        pthread_join(consumidor[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);

    // BUFFER COM PROTEÇÃO
    printf("\n========== PRODUTOR x CONSUMIDOR - COM PROTEÇÃO ==========\n\r");
    zera_buffer();
    sem_init(&empty, 0, BUF_LENGTH);
    sem_init(&full, 0, 0);

    for (int i = 0; i < PRODUTOR; i++)
        pthread_create(&produtor[i], NULL, &produtor_protegido, NULL);

    for (int i = 0; i < CONSUMIDOR; i++)
        pthread_create(&consumidor[i], NULL, &consumidor_protegido, NULL);

    for (int i = 0; i < PRODUTOR; i++)
        pthread_join(produtor[i], NULL);

    for (int i = 0; i < CONSUMIDOR; i++)
        pthread_join(consumidor[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}

void push(uint8_t data) {
    cb.buffer[cb.write_index] = data;
    cb.write_index = (cb.write_index + 1) % BUF_LENGTH;
    cb.count++;
    printf("[PRODUTOR] Inseriu: %d | buffer: %d/%d\n", data, cb.count,
           BUF_LENGTH);
}

uint8_t pop() {
    uint8_t data = cb.buffer[cb.read_index];
    cb.read_index = (cb.read_index + 1) % BUF_LENGTH;
    cb.count--;
    printf("[CONSUMIDOR] Removeu: %d | buffer: %d/%d\n", data, cb.count,
           BUF_LENGTH);
    return data;
}

void zera_buffer() {
    cb.write_index = 0;
    cb.read_index = 0;
    cb.count = 0;
}

void *produtor_puro(void *ptr) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        push(i);
    }

    return NULL;
}

void *produtor_protegido(void *ptr) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&lock);

        push(i);

        pthread_mutex_unlock(&lock);
        sem_post(&full);
    }

    return NULL;
}

void *consumidor_puro(void *ptr) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        pop();
    }

    return NULL;
}

void *consumidor_protegido(void *ptr) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&lock);

        pop();

        pthread_mutex_unlock(&lock);
        sem_post(&empty);
    }

    return NULL;
}