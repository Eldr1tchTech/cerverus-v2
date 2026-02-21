#pragma once

typedef struct client
{
    int socket_fd;
    const char* host;
    int port;
} client;

client* client_create(const char* host, const int port);
void client_destroy(client* c);

bool client_request(client* c, const char* URI);