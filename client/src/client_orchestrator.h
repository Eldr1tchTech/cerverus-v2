#pragma once

#include "client.h"

typedef struct client_orchestrator
{
    const char* host;
    int port;
    client** clients;
    int client_count;
} client_orchestrator;

typedef enum benchmark_type
{
    benchmark_type_peak_load,
    benchmark_type_soak,
    benchmark_type_smoke,
    benchmark_type_step,
    benchmark_type_undefined,
} benchmark_type;

// Describes a single step in a step benchmark
typedef struct step_entry
{
    int requests_per_second;
    int duration_seconds;
} step_entry;

typedef struct benchmark_context
{
    union
    {
        // Hammer the server as hard as possible for a short burst
        struct
        {
            int duration_seconds;
            int concurrency;
        } peak_load_context;
    };

    char** URIs;
    int URI_count;
} benchmark_context;

// Per-URI breakdown, aggregated into benchmark_result
typedef struct uri_result
{
    char* URI;
    int requests_sent;
    int failures;
    double avg_latency_ms;
    double min_latency_ms;
    double max_latency_ms;
    double p95_latency_ms;
    double p99_latency_ms;
} uri_result;

typedef struct benchmark_result
{
    benchmark_type type;

    // Aggregate totals
    int total_requests;
    int total_failures;
    double total_duration_seconds;
    double avg_requests_per_second;

    // Latency across all URIs
    double avg_latency_ms;
    double min_latency_ms;
    double max_latency_ms;
    double p95_latency_ms;
    double p99_latency_ms;

    // Per-URI breakdown (mirrors URIs from benchmark_context)
    uri_result* uri_results;
    int uri_result_count;
} benchmark_result;

client_orchestrator* client_orchestrator_create(const char* host, int port, int client_count);
void client_orchestrator_destroy(client_orchestrator* c_orch);

void client_orchestrator_perform_benchmark(client_orchestrator* c_orch, benchmark_type bm_type, benchmark_context bm_ctx, benchmark_result* bm_rst);