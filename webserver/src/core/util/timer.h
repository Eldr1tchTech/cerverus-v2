#pragma once

#include <time.h>

typedef struct timer {
    struct timespec start;
} timer;

void timer_start(timer* t);

double timer_stop(timer* t);