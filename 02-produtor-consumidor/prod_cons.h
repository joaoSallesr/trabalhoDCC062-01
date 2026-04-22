#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>

#define BUF_LENGTH 5
#define NUM_ITEMS 10
#define PRODUTOR 1
#define CONSUMIDOR 1

typedef struct {
    uint8_t buffer[BUF_LENGTH];
    volatile uint8_t write_index;
    volatile uint8_t read_index;
    volatile uint8_t count;
} circular_buffer;

void push(uint8_t data);
uint8_t pop();
void zera_buffer();

void *produtor_puro(void *ptr);
void *produtor_protegido(void *ptr);
void *consumidor_puro(void *ptr);
void *consumidor_protegido(void *ptr);