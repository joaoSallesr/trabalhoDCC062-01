#include "filosofos.h"

#define PHILOSOPHERS 5
#define LEFT (i + PHILOSOPHERS - 1) % PHILOSOPHERS
#define RIGHT (i + 1) % PHILOSOPHERS
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define MAX_LOOP 50
#define DEADLOCK_TIMEOUT 5

sem_t mutex;
sem_t philosophers_sem[PHILOSOPHERS];
sem_t forks_sem[PHILOSOPHERS];
int state[PHILOSOPHERS];
int meals[PHILOSOPHERS] = {0};

int main() {

    printf("========== DINNING PHILOSOPHERS - NO PROTECTION ==========\n\r");
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < PHILOSOPHERS; i++) {
        sem_init(&forks_sem[i], 0, 1);
    }

    pthread_t philosophers[PHILOSOPHERS];

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, &philosopher_pure,
                       (void *)(intptr_t)i);

    sleep(DEADLOCK_TIMEOUT);

    printf("\n>>>Timeout -> %ds<<<\n\n", DEADLOCK_TIMEOUT);

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_cancel(philosophers[i]);
    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);

    sem_destroy(&mutex);
    for (int i = 0; i < PHILOSOPHERS; i++)
        sem_destroy(&forks_sem[i]);

    for (int i = 0; i < PHILOSOPHERS; i++)
        printf("Philosopher %d ate: %d/%d\n", i, meals[i], MAX_LOOP);

    printf("========== DINNING PHILOSOPHERS - PROTECTED ==========\n\r");
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < PHILOSOPHERS; i++) {
        sem_init(&philosophers_sem[i], 0, 0);
    }

    for (int i = 0; i < PHILOSOPHERS; i++) {
        meals[i] = 0;
        state[i] = THINKING;
    }

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, &philosopher_protected,
                       (void *)(intptr_t)i);

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);

    sem_destroy(&mutex);
    for (int i = 0; i < PHILOSOPHERS; i++)
        sem_destroy(&philosophers_sem[i]);

    for (int i = 0; i < PHILOSOPHERS; i++)
        printf("Philosopher %d ate: %d/%d\n", i, meals[i], MAX_LOOP);
}

void *philosopher_pure(void *ptr) {
    int id = (intptr_t)ptr;
    for (int i = 0; i < MAX_LOOP; i++) {
        think();
        take_forks_pure(id);
        eat(id);
        put_forks_pure(id);
    }

    return NULL;
}

void *philosopher_protected(void *ptr) {
    int id = (intptr_t)ptr;
    for (int i = 0; i < MAX_LOOP; i++) {
        think();
        take_forks_protected(id);
        eat(id);
        put_forks_protected(id);
    }

    return NULL;
}

void take_forks_pure(int i) {
    sem_wait(&forks_sem[LEFT]);
    usleep(100000);
    sem_wait(&forks_sem[RIGHT]);
}

void put_forks_pure(int i) {
    sem_post(&forks_sem[LEFT]);
    usleep(100000);
    sem_post(&forks_sem[RIGHT]);
}

void take_forks_protected(int i) {
    sem_wait(&mutex);
    state[i] = HUNGRY;
    check(i);
    sem_post(&mutex);
    sem_wait(&philosophers_sem[i]);
}

void put_forks_protected(int i) {
    sem_wait(&mutex);
    state[i] = THINKING;
    check(LEFT);
    check(RIGHT);
    sem_post(&mutex);
}

void check(int i) {
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        sem_post(&philosophers_sem[i]);
    }
}

void think() { usleep(10000); }

void eat(int i) {
    meals[i]++;
    usleep(rand() % 100000);
}
