#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *philosopher_pure(void *ptr);
void *philosopher_protected(void *ptr);

void take_forks_pure(int i);
void put_forks_pure(int i);
void take_forks_protected(int i);
void put_forks_protected(int i);

void check(int i);
void think();
void eat(int i);