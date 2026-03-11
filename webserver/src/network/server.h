#pragma once

#include "network/network_types.inl"

#include "core/containers/darray.h"
#include "network/route_trie.h"

typedef struct server
{
    int socket_fd;
    // TODO: Change to use an trie
    trie* route_trie;
} server;

server* server_create();

void server_add_route(server* s, route* rt);
void server_handle_request(server* s, request* req, int client_fd);

void server_run(server* s);