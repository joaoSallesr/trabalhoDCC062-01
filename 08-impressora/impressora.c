#include "impressora.h"

circular_buffer cb;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;

int jobs_sent = 0;
int jobs_printed = 0;

int main() {
    printf("========== IMPRESSORA - SEM PROTEÇÃO ==========\n\r");
    zera_buffer();
    sem_init(&empty, 0, BUF_LENGTH);
    sem_init(&full, 0, 0);

    pthread_t processos[PROCESSOS];
    pthread_t impressora;

    pthread_create(&impressora, NULL, &impressora_pura, NULL);
    for (int i = 0; i < PROCESSOS; i++)
        pthread_create(&processos[i], NULL, &processo_puro,
                       (void *)(intptr_t)i);

    for (int i = 0; i < PROCESSOS; i++)
        pthread_join(processos[i], NULL);
    pthread_join(impressora, NULL);

    printf("Enviados: %d/%d | Impressos: %d/%d\n", jobs_sent, TOTAL_JOBS,
           jobs_printed, TOTAL_JOBS);

    printf("========== IMPRESSORA - COM PROTEÇÃO ==========\n\r");
    zera_buffer();
    jobs_sent = 0;
    jobs_printed = 0;
    sem_init(&empty, 0, BUF_LENGTH);
    sem_init(&full, 0, 0);

    pthread_create(&impressora, NULL, &impressora_protegida, NULL);
    for (int i = 0; i < PROCESSOS; i++)
        pthread_create(&processos[i], NULL, &processo_protegido,
                       (void *)(intptr_t)i);

    for (int i = 0; i < PROCESSOS; i++)
        pthread_join(processos[i], NULL);
    pthread_join(impressora, NULL);

    printf("Enviados: %d/%d | Impressos: %d/%d\n", jobs_sent, TOTAL_JOBS,
           jobs_printed, TOTAL_JOBS);
}

void push(print_job job) {
    cb.buffer[cb.write_index] = job;
    cb.write_index = (cb.write_index + 1) % BUF_LENGTH;
    cb.count++;
    printf("[PROCESSO %d] Enviou job %d (%d paginas) | fila: %d/%d\n",
           job.process_id, job.job_id, job.pages, cb.count, BUF_LENGTH);
}

print_job pop() {
    print_job job = cb.buffer[cb.read_index];
    cb.read_index = (cb.read_index + 1) % BUF_LENGTH;
    cb.count--;
    return job;
}

void zera_buffer() {
    cb.write_index = 0;
    cb.read_index = 0;
    cb.count = 0;
}

void imprimir(print_job job) { usleep(job.pages * 50000); }

void *processo_puro(void *ptr) {
    int id = (intptr_t)ptr;
    for (int i = 0; i < JOBS_PER_PROCESS; i++) {
        print_job job = {
            .process_id = id, .job_id = i, .pages = rand() % 5 + 1};
        push(job);
        jobs_sent++;
    }
    return NULL;
}

void *processo_protegido(void *ptr) {
    int id = (intptr_t)ptr;
    for (int i = 0; i < JOBS_PER_PROCESS; i++) {
        print_job job = {
            .process_id = id, .job_id = i, .pages = rand() % 5 + 1};
        sem_wait(&empty);
        pthread_mutex_lock(&lock);
        push(job);
        jobs_sent++;
        pthread_mutex_unlock(&lock);
        sem_post(&full);
    }
    return NULL;
}

void *impressora_pura(void *ptr) {
    for (int i = 0; i < TOTAL_JOBS; i++) {
        print_job job = pop();
        printf("[IMPRESSORA] Imprimindo job %d do processo %d (%d paginas)\n",
               job.job_id, job.process_id, job.pages);
        imprimir(job);
        jobs_printed++;
    }
    return NULL;
}

void *impressora_protegida(void *ptr) {
    for (int i = 0; i < TOTAL_JOBS; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&lock);
        print_job job = pop();
        jobs_printed++;
        pthread_mutex_unlock(&lock);
        sem_post(&empty);
        printf("[IMPRESSORA] Imprimindo job %d do processo %d (%d paginas)\n",
               job.job_id, job.process_id, job.pages);
        imprimir(job);
    }
    return NULL;
}