#include "benchmark_result.h"

#include <core/memory/cmem.h>

benchmark_result* create_benchmark_result() {
    benchmark_result* bm_r = cmem_alloc(memory_tag_benchmark, sizeof(benchmark_result));

    bm_r->conn_bm_result = darray_create(500, sizeof(connection_benchmark_result));
}

connection_benchmark_result parse_buffer_result(char* str) {
    // sets the URI and status code.
}