#pragma once
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_LENGTH 10
#define PROCESSOS 5
#define JOBS_PER_PROCESS 4
#define TOTAL_JOBS (PROCESSOS * JOBS_PER_PROCESS)

typedef struct {
    int process_id;
    int job_id;
    int pages;
} print_job;

typedef struct {
    print_job buffer[BUF_LENGTH];
    volatile int write_index;
    volatile int read_index;
    volatile int count;
} circular_buffer;

void push(print_job job);
print_job pop();
void zera_buffer();

void *processo_puro(void *ptr);
void *impressora_pura(void *ptr);

void *processo_protegido(void *ptr);
void *impressora_protegida(void *ptr);

void imprimir(print_job job);