#include "leit_escr.h"

#define READERS 5
#define WRITERS 3
#define SHARED_SIZE 20000

sem_t mutex;
sem_t writers_sem;
int readers_count = 0;
int read_count = 0;
int write_count = 0;

int shared[SHARED_SIZE];

int main() {
    // ESCRITA SEM PROTEÇÃO
    printf("========== READERS x WRITERS - NO PROTECTION ==========\n\r");
    sem_init(&mutex, 0, 1);
    sem_init(&writers_sem, 0, 1);

    pthread_t reader[READERS];
    pthread_t writer[WRITERS];

    for (int i = 0; i < READERS; i++)
        pthread_create(&reader[i], NULL, &reader_pure, NULL);

    for (int i = 0; i < WRITERS; i++)
        pthread_create(&writer[i], NULL, &writer_pure, NULL);

    for (int i = 0; i < READERS; i++)
        pthread_join(reader[i], NULL);

    for (int i = 0; i < WRITERS; i++)
        pthread_join(writer[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&writers_sem);

    printf("Read: %d/%d | Written: %d/%d\n", read_count, SHARED_SIZE * READERS,
           write_count, SHARED_SIZE * WRITERS);

    // ESCRITA COM PROTEÇÃO
    printf("========== READERS x WRITERS - PROTECTED ==========\n\r");
    readers_count = 0;
    read_count = 0;
    write_count = 0;
    sem_init(&mutex, 0, 1);
    sem_init(&writers_sem, 0, 1);

    for (int i = 0; i < READERS; i++)
        pthread_create(&reader[i], NULL, &reader_protected, NULL);

    for (int i = 0; i < WRITERS; i++)
        pthread_create(&writer[i], NULL, &writer_protected, NULL);

    for (int i = 0; i < READERS; i++)
        pthread_join(reader[i], NULL);

    for (int i = 0; i < WRITERS; i++)
        pthread_join(writer[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&writers_sem);

    printf("Read: %d/%d | Written: %d/%d\n", read_count, SHARED_SIZE * READERS,
           write_count, SHARED_SIZE * WRITERS);

    return 0;
}

void *reader_pure(void *ptr) {
    readers_count++;
    int a = 0;

    for (int i = 0; i < SHARED_SIZE; i++) {
        a = shared[i];
        read_count++;
    }

    readers_count--;
}

void *reader_protected(void *ptr) {
    sem_wait(&mutex);
    readers_count++;
    if (readers_count == 1)
        sem_wait(&writers_sem);
    sem_post(&mutex);

    int a = 0;
    for (int i = 0; i < SHARED_SIZE; i++) {
        a = shared[i];
        sem_wait(&mutex);
        read_count++;
        sem_post(&mutex);
    }

    sem_wait(&mutex);
    readers_count--;
    if (readers_count == 0)
        sem_post(&writers_sem);
    sem_post(&mutex);
}

void *writer_pure(void *ptr) {
    for (int i = 0; i < SHARED_SIZE; i++) {
        shared[i] = i;
        write_count++;
    }
}
void *writer_protected(void *ptr) {
    sem_wait(&writers_sem);
    for (int i = 0; i < SHARED_SIZE; i++) {
        shared[i] = i;
        sem_wait(&mutex);
        write_count++;
        sem_post(&mutex);
    }
    sem_post(&writers_sem);
}