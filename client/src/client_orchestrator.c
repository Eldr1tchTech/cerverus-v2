#include "client_orchestrator.h"

#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/timer.h>

client_orchestrator *client_orchestrator_create(const char *host, int port, int client_count)
{
    if (client_count < 1)
    {
        LOG_ERROR("client_orchestrator_create - client_count must be 1 or greater.");
        return NULL;
    }

    client_orchestrator *c_orch = cmem_alloc(memory_tag_client_orchestrator, sizeof(client_orchestrator));
    c_orch->host = host;
    c_orch->port = port;
    c_orch->client_count = client_count;
    c_orch->clients = cmem_alloc(memory_tag_client_orchestrator, sizeof(client_orchestrator *) * client_count);

    for (int i = 0; i < client_count; i++)
    {
        c_orch->clients[i] = client_create(host, port);
    }

    return c_orch;
}

void client_orchestrator_destroy(client_orchestrator *c_orch)
{
    for (int i = 0; i < c_orch->client_count; i++)
    {
        client_destroy(c_orch->clients[i]);
    }

    cmem_free(memory_tag_client_orchestrator, c_orch->clients);

    cmem_free(memory_tag_client_orchestrator, c_orch);
    c_orch = 0;
}

void c_orch_print_benchmark_result(benchmark_result *bm_rst)
{
}

void perform_benchmark_smoke(client_orchestrator *c_orch, benchmark_context bm_ctx, benchmark_result *bm_rst)
{
    timer t;
    timer_start(&t);

    for (int i = 0; i < bm_ctx.URI_count; i++)
    {
        double req0 = timer_stop(&t);

        client_request(c_orch->clients[0], bm_ctx.URIs[i]);

        double elapsed = timer_stop(&t) - req0;
    }

    double total_elapsed = timer_stop(&t);
}

void perform_benchmark_peak_load(client_orchestrator *c_orch, benchmark_context bm_ctx, benchmark_result *bm_rst)
{

}

void client_orchestrator_perform_benchmark(client_orchestrator *c_orch, benchmark_type bm_type, benchmark_context bm_ctx, benchmark_result *bm_rst)
{
    switch (bm_type)
    {
    case benchmark_type_smoke:
        perform_benchmark_smoke(c_orch, bm_ctx, bm_rst);
        break;
    case benchmark_type_peak_load:
        perform_benchmark_peak_load(c_orch, bm_ctx, bm_rst);
        break;   
    default:
        LOG_ERROR("client_orchestrator_perform_benchmark - Invalid benchmark type.");
        return;
        break;
    }

    c_orch_print_benchmark_result(bm_rst);
}