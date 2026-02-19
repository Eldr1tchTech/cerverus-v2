#include "server.h"

#include "defines.h"

#include "core/memory/cmem.h"
#include "core/util/logger.h"
#include "core/util/profiler.h"
#include "core/util/util.h"
#include "network/request.h"
#include "network/response.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// TODO: Look into this.
// void handle_sigpipe(int sig) {
//     // Ignore SIGPIPE to prevent crashes when client disconnects
//     fprintf(stderr, "Caught SIGPIPE, client disconnected unexpectedly\n");
// }

server *server_create(server_config s_conf)
{
    server *s = cmem_alloc(memory_tag_server, sizeof(server));
    s->routes = darray_create(8, sizeof(route));
    s->port = s_conf.port;

    return s;
}

void server_add_route(server *s, route *rt)
{
    darray_add(s->routes, rt);
}

void server_handle_request(server *s, request *req, int client_fd)
{
    // 1. Check public directory
    if (req->request_line.method == http_method_get)
    {
        const char *ext = strrchr(req->request_line.URI, '.');
        if (ext)
        {
            int file_fd = open(asprintf("assets/public%s", req->request_line.URI), O_RDONLY);
            if (file_fd != -1)
            {
                // 1. Assemble response
                // TODO: Eventually use a pool for this
                response *res = response_create(0);

                res->status_line.version = http_version_1p1;
                res->status_line.status_code = 200;
                res->status_line.reason_phrase = "OK";
                char *content_type_value = "text/html";

                const char *ext = strrchr(req->request_line.URI, '.');

                if (ext && strcmp(ext + 1, "html") == 0)
                {
                    content_type_value = "text/html";
                }
                else if (ext && strcmp(ext + 1, "css") == 0)
                {
                    content_type_value = "text/css";
                }

                struct stat file_stat;
                fstat(file_fd, &file_stat);

                int header_count = 3;
                header h = {
                    .name = "Content-Type",
                    .value = content_type_value,
                };
                darray_add(res->headers, &h);

                h.name = "Content-Length";
                h.value = asprintf("%i", file_stat.st_size);
                darray_add(res->headers, &h);

                h.name = "Connection";
                h.value = "close";
                darray_add(res->headers, &h);

                // 2. Send response and file
                char* raw = response_serialize(res);
                send(client_fd, raw, strlen(raw), 0);
                sendfile(client_fd, file_fd, 0, file_stat.st_size);

                close(file_fd);
                return;
            }
        }
    }

    // 2. Check against dynamic registered routes

    // 3. Send 404 if you have made it to this point
    int file_fd = open("assets/404.html", O_RDONLY);
    // 1. Assemble response
    // TODO: Eventually use a pool for this
    response *res = response_create(0);

    res->status_line.version = http_version_1p1;
    res->status_line.status_code = 404;
    res->status_line.reason_phrase = "Not Found";
    char *content_type_value = "text/html";

    struct stat file_stat;
    fstat(file_fd, &file_stat);

    int header_count = 3;
    header h = {
        .name = "Content-Type",
        .value = content_type_value,
    };
    darray_add(res->headers, &h);

    h.name = "Content-Length";
    h.value = asprintf("%i", file_stat.st_size);
    darray_add(res->headers, &h);

    h.name = "Connection";
    h.value = "close";
    darray_add(res->headers, &h);

    // 2. Send response and file
    char* raw = response_serialize(res);
    send(client_fd, raw, strlen(raw), 0);
    sendfile(client_fd, file_fd, 0, file_stat.st_size);

    close(file_fd);

    // send(client_fd, response_serialize(res), 1982, 0);
}

void server_destroy(server *s)
{
    cmem_print_stats();
    cmem_free(memory_tag_server, s);
    darray_destroy(s->routes);
    s = 0;
}

void server_run(server *s)
{
    cmem_print_stats();

    LOG_INFO("Starting server...");

    s->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->socket_fd == -1)
    {
        LOG_FATAL("server_start - Socket creation failed.");
        return;
    }

    int opt = 1;
    if (setsockopt(s->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        LOG_FATAL("server_start - setsockopt failed.");
        close(s->socket_fd);
        return;
    }

    // TODO: Look into this...
    // Enable non-blocking mode (optional)
    // fcntl(server_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(s->port),
        .sin_addr.s_addr = INADDR_ANY};

    if (bind(s->socket_fd, &addr, sizeof(addr)) == -1)
    {
        LOG_FATAL("server_start - Bind failed.");
        return;
    }

    if (listen(s->socket_fd, 10) == -1)
    {
        LOG_FATAL("server_start - Listen failed.");
        return;
    }

    LOG_INFO("Server listening on port %i.", s->port);

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(s->socket_fd, 0, 0);
        if (client_fd == -1)
        {
            LOG_INFO("server_start - accept failed.");
            continue;
        }

        char buffer[8192] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, 8192, 0);

        if (bytes_received <= 0)
        {
            if (bytes_received == -1)
            {
                LOG_INFO("server_start - recv failed.");
            }
            else
            {
                LOG_INFO("Client disconnected.");
            }
            close(client_fd);
            continue;
        }

        request *req = request_parse(buffer);

        LOG_INFO("Request received.");

        profile_operation("routing", {
            server_handle_request(s, req, client_fd);
        });

        request_destroy(req);

        close(client_fd);
    }

    close(s->socket_fd);

    server_destroy(s);
}