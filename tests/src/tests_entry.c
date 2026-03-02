#include "unit/unit_entry.h"
#include "stress/stress_entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // 1. Run unit tests
    printf("Tests_entry.c now running.\nStarting unit tests.\n");
    run_unit_tests();

    // NOTE: Unit tests must succeed for the stress tests to run.

    // 2. Run stress tests
    printf("Starting stress tests.\n");
    run_stress_tests();
    return 0;
}