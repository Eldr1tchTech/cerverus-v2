#pragma once

#include "stress/benchmark_result.h"

#include <arpa/inet.h>

typedef struct client_manager
{
    int client_fd;
    char** URIs;
    struct sockaddr_in addr;
} client_manager;

client_manager *client_manager_create();
void client_manager_destroy(client_manager *c_man);

benchmark_result* client_manager_run(client_manager *c_man);