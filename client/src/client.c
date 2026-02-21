#include "client.h"

#include "core/memory/cmem.h"
#include "core/util/logger.h"

#include <sys/socket.h>
#include <arpa/inet.h>

client* client_create(const char* host, const int port) {
    client* new_client = cmem_alloc(memory_tag_client, sizeof(client));
    new_client->host = host;
    new_client->port = port;

    new_client->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    return new_client;
}

void client_destroy(client* c) {
    close(c->socket_fd);
    cmem_free(memory_tag_client, c);
    c = 0;
}

bool client_request(client* c, const char* URI) {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(c->port),
    };
    inet_pton(AF_INET, c->host, &addr.sin_addr);

    if (connect(c->socket_fd, &addr, sizeof(addr)) == -1) {
        LOG_ERROR("client_request - Failed to connect to: %s:%i", c->host, c->port);
    }

    char req[512];
    snprintf(req, sizeof(req),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", URI, c->host);

    send(c->socket_fd, req, strlen(req), 0);

    char buf[4];
    ssize_t n;
    while ((n = recv(c->socket_fd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[n] = '\0';
        if (atoi(buf) != 200)
        {
            return false;
        }
    }

    return true;
}