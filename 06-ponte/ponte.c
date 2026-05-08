#include "ponte.h"

int main() {
    pthread_t ponteA[PONTEA], ponteB[PONTEB];

    // PONTE SEM PROTEÇÃO
    printf("========== Ponte de mão única - SEM PROTEÇÃO ==========\n\r");

    for (int i = 0; i < PONTEA; i++)
        pthread_create(&ponteA[i], NULL, &ponteA_unprotected, (void*)(intptr_t)i);

    for (int i = 0; i < PONTEB; i++)
        pthread_create(&ponteB[i], NULL, &ponteB_unprotected, (void*)(intptr_t)i);

    for (int i = 0; i < PONTEA; i++)
        pthread_join(ponteA[i], NULL);

    for (int i = 0; i < PONTEB; i++)
        pthread_join(ponteB[i], NULL);

    for (int i = 0; i < PONTEA + PONTEB; i++)
        printf("%3d : ", ordem_passagem[i]);
    printf("\nResultado SEM protecao (A>=0, B<0):\n");
    printf("  Colisoes detectadas: %d\n", colisoes);

    // PONTE COM PROTEÇÃO
    printf("========== PONTE MÃO ÚNICA - PROTEGIDA ==========\n\r");

    for (int i = 0; i < PONTEA + PONTEB; i++)
        ordem_passagem[i] = 0;
    ponteA_count = 0;
    ponteB_count = 0;
    colisoes = 0;
    ordem_index = 0;
    
    sem_init(&fila_sem, 0, 1);
    sem_init(&mutex_sem, 0, 1);
    sem_init(&ponte_livre_sem, 0, 1);

    for (int i = 0; i < PONTEA; i++)
        pthread_create(&ponteA[i], NULL, &ponteA_protected, (void*)(intptr_t)i);
    
    for (int i = 0; i < PONTEB; i++)
        pthread_create(&ponteB[i], NULL, &ponteB_protected, (void*)(intptr_t)i);

    for (int i = 0; i < PONTEA; i++)
        pthread_join(ponteA[i], NULL);

    for (int i = 0; i < PONTEB; i++)
        pthread_join(ponteB[i], NULL);

    sem_destroy(&fila_sem);
    sem_destroy(&mutex_sem);
    sem_destroy(&ponte_livre_sem);

    for (int i = 0; i < PONTEA + PONTEB; i++)
        printf("%3d : ", ordem_passagem[i]);

    printf("\nResultado COM protecao (A>=0, B<0):\n");
    printf("Colisoes detectadas: %d\n", colisoes);
    return 0;
}

void cruzar_ponte(char sentido, int id)
{
    if (ponteA_count > 0 && ponteB_count > 0) {
        pthread_mutex_lock(&colisoes_mutex);
        colisoes++;
        pthread_mutex_unlock(&colisoes_mutex);
        printf("COLISAO! A=%d B=%d (carro %c-%d) ***\n",
               ponteA_count, ponteB_count, sentido, id);
    }
    printf("[%c-%02d] cruzando... (A=%d na ponte, B=%d na ponte)\n",
           sentido, id, ponteA_count, ponteB_count);
    pthread_mutex_lock(&ordem_mutex);
    ordem_passagem[ordem_index++] = (sentido == 'A' ? id : -1 * id);
    pthread_mutex_unlock(&ordem_mutex);
    usleep(30000 + rand() % 30000);
}

void *ponteA_unprotected(void *ptr) {
    int id = (intptr_t)ptr;
    ponteA_count++;
    cruzar_ponte('A', id);
    ponteA_count--;
    return NULL;
}

void *ponteB_unprotected(void *ptr) {
    int id = (intptr_t)ptr;
    ponteB_count++;
    cruzar_ponte('B', id);
    ponteB_count--;
    return NULL;
}

void *ponteA_protected(void *ptr) {
    int id = (intptr_t)ptr;

    sem_wait(&fila_sem);
    sem_post(&fila_sem);

    sem_wait(&mutex_sem);
    ponteA_count++;
    sem_post(&mutex_sem);

    if (ponteA_count == 1)
        sem_wait(&ponte_livre_sem);
    cruzar_ponte('A', id);

    sem_wait(&mutex_sem);
    ponteA_count--;
    if (ponteA_count == 0)
        sem_post(&ponte_livre_sem);
    sem_post(&mutex_sem);

    return NULL;
}

void *ponteB_protected(void *ptr) {
    int id = (intptr_t)ptr;

    sem_wait(&fila_sem);
    sem_post(&fila_sem);

    sem_wait(&mutex_sem);
    ponteB_count++;
    sem_post(&mutex_sem);

    if (ponteB_count == 1)
        sem_wait(&ponte_livre_sem);
    cruzar_ponte('B', id);

    sem_wait(&mutex_sem);
    ponteB_count--;
    if (ponteB_count == 0)
        sem_post(&ponte_livre_sem);
    sem_post(&mutex_sem);

    return NULL;
}

void cruzar_ponte(char sentido, int id) {
    if (ponteA_count > 0 && ponteB_count > 0) {
        colisoes++;
        printf("COLISAO! A=%d B=%d (carro %c-%d) ***\n",
               ponteA_count, ponteB_count, sentido, id);
    }
    printf("[%c-%02d] cruzando... (A=%d na ponte, B=%d na ponte)\n",
           sentido, id, ponteA_count, ponteB_count);
 
    pthread_mutex_lock(&ordem_mutex);
    ordem_passagem[ordem_index++] = (sentido == 'A') ? id : -id;
    pthread_mutex_unlock(&ordem_mutex);
 
    usleep(30000 + rand() % 30000);
}