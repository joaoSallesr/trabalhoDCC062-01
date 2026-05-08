#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PONTEA 10
#define PONTEB 8

void cruzar_ponte(char sentido, int id);
void reset_estado(void);
void *ponteA_unprotected(void *ptr);
void *ponteB_unprotected(void *ptr);
void *ponteA_protected(void *ptr);
void *ponteB_protected(void *ptr);

sem_t fila_sem;
sem_t mutex_sem;
sem_t ponte_livre_sem;
pthread_mutex_t colisoes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ordem_mutex = PTHREAD_MUTEX_INITIALIZER;
 
int ponteA_count = 0;
int ponteB_count = 0;
int colisoes = 0;
 
int ordem_passagem[PONTEA + PONTEB] = {0};
int ordem_index = 0;