#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#define THREADS 4
#define INCREMENTOS 100000

uint32_t contador;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *incrementar_puro(void *ptr);
void *incrementar_protegido(void *ptr);

int main() {
    pthread_t thread[THREADS];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_create(&thread[i], NULL, &incrementar_puro, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(thread[i], NULL);

    printf("PURO: Valor esperado do contador: %" PRIu32 "\n",
           THREADS * INCREMENTOS);
    printf("PURO: Valor do contador: %" PRIu32 "\n", contador);

    contador = 0;

    for (int i = 0; i < THREADS; i++)
        pthread_create(&thread[i], NULL, &incrementar_protegido, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(thread[i], NULL);

    printf("PROTEGIDO: Valor esperado do contador: %" PRIu32 "\n",
           THREADS * INCREMENTOS);
    printf("PROTEGIDO: Valor do contador: %" PRIu32 "\n", contador);

    return 0;
}

void *incrementar_puro(void *ptr) {
    for (int i = 0; i < INCREMENTOS; i++) {
        contador++;
    }
    return NULL;
}

void *incrementar_protegido(void *ptr) {
    for (int i = 0; i < INCREMENTOS; i++) {
        pthread_mutex_lock(&lock);
        contador++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
