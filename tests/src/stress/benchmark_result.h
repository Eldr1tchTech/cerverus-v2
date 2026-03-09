#pragma once

#include <core/containers/darray.h>

typedef enum benchmark_version {
    benchmark_version_0p0p0,
    benchmark_version_0p1p0,
} benchmark_version;

typedef struct connection_benchmark_result
{
    double duration;
    char* URI;
    int status_code;
} connection_benchmark_result;

typedef struct benchmark_result
{
    benchmark_version bm_v;
    double benchmark_duration;
    darray* conn_bm_result;
} benchmark_result;

benchmark_result* create_benchmark_result();
void destroy_benchmark_result(benchmark_result* bm_res);

connection_benchmark_result parse_buffer_result(char* str);