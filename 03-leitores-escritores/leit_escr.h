#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>

void *reader_pure(void *ptr);
void *reader_protected(void *ptr);
void *writer_pure(void *ptr);
void *writer_protected(void *ptr);