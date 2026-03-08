#include "darray_tests.h"

#include <core/containers/darray.h>

#include <stdio.h>

void darray_tests() {
    // Initialization
    int successes = 0;
    darray* darr = darray_create(2, sizeof(int));
    if (darr->length == 0)
    {
        successes++;
    } else {
        printf("FAIL: length initialization\n");
    }
    if (darr->size == 2)
    {
        successes++;
    } else {
        printf("FAIL: size initialization\n");
    }
    if (darr->stride == sizeof(int))
    {
        successes++;
    } else {
        printf("FAIL: stride initialization\n");
    }

    // Data Initialization
    int* darr_data = darr->data;
    if (darr_data[0] == 0)
    {
        successes++;
    } else {
        printf("FAIL: data initialization\n");
    }

    // Darray add
    int num = 67;
    darray_add(darr, &num);
    if (darr_data[0] == 67)
    {
        successes++;
    } else {
        printf("FAIL: darray add value\n");
    }
    if (darr->length == 1)
    {
        successes++;
    } else {
        printf("FAIL: darray add length\n");
    }

    // Darray add resize
    darray_add(darr, &num);
    darray_add(darr, &num);
    darr_data = darr->data;
    if (darr_data[2] == 67)
    {
        successes++;
    } else {
        printf("FAIL: darray resize value\n");
    }
    if (darr->length == 3)
    {
        successes++;
    } else {
        printf("FAIL: darray resize length\n");
    }
    if (darr->size == 4)
    {
        successes++;
    } else {
        printf("FAIL: darray resize size\n");
    }

    darray_destroy(darr);
    
    printf("(%i/9) passed.\n", successes);
}