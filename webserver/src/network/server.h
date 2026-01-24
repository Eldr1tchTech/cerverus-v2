#pragma once

#include "network/router.h"

typedef struct server_config
{
    int port;
} server_config;

typedef struct server
{
    int port;
    int socket_fd;
    router* r;
} server;

server* server_create(server_config s_conf);

void server_run(server* s);