#include "timer.h"

void timer_start(timer* t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

double timer_stop(timer* t) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - t->start.tv_sec) +
           (end.tv_nsec - t->start.tv_nsec) / 1000000000.0;
}