#include "benchmark_result.h"

#include <core/memory/cmem.h>
#include <string.h>
#include <stdlib.h>

benchmark_result* create_benchmark_result() {
    benchmark_result* bm_r = cmem_alloc(memory_tag_benchmark, sizeof(benchmark_result));
    bm_r->conn_bm_result = darray_create(500, sizeof(connection_benchmark_result));
    
    // CRITICAL FIX: Return the created result
    return bm_r;
}

void destroy_benchmark_result(benchmark_result* bm_res) {
    if (!bm_res) return;
    
    if (bm_res->conn_bm_result) {
        darray_destroy(bm_res->conn_bm_result);
    }
    
    cmem_free(memory_tag_benchmark, bm_res);
}

connection_benchmark_result parse_buffer_result(char* str) {
    connection_benchmark_result result = {0};
    
    // Parse HTTP response to get status code
    // Format: "HTTP/1.1 200 OK\r\n..."
    if (str && strlen(str) > 12) {
        char* status_start = strchr(str, ' ');
        if (status_start) {
            result.status_code = atoi(status_start + 1);
        }
    }
    
    // URI will be set by the caller
    result.URI = "";
    result.duration = 0.0;
    
    return result;
}