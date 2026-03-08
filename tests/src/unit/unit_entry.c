// This is the entry point for the unit test part of the testing.
// The this will be called as a method from the tests_entry.c
#include "darray_tests.h"

#include <stdio.h>

// TODO: Eventually extend the test suite to be more professional and have more of a structure.

void run_unit_tests() {
    printf("Running unit tests.\n");

    darray_tests();
    
    printf("Unit tests finished.\n");
}