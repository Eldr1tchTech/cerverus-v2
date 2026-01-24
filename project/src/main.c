#include <network/request.h>
#include <network/server.h>
#include <network/router.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdarg.h>
#include <unistd.h>

#define FILE_CALLBACK(callback_name, file_path, status_code, reason) \
void callback_name(request* req, response* res) { \
    res->status_line.version = http_version_1p1; \
    res->status_line.status_code = status_code; \
    res->status_line.reason_phrase = reason; \
    \
    int file_fd = open(file_path, O_RDONLY); \
    \
    if (file_fd == -1) { \
        res->body.data = NULL; \
        res->body.body_size = 0; \
        return; \
    } \
    \
    struct stat file_stat; \
    fstat(file_fd, &file_stat); \
    \
    res->body.data = cmem_alloc(memory_tag_string, file_stat.st_size + 1); \
    \
    int header_count = 3; \
    header content_type = { \
        .name = "Content-Type", \
        .value = "text/html", \
    }; \
    header content_length = { \
        .name = "Content-Length", \
        .value = asprintf("%i", file_stat.st_size), \
    }; \
    header connection = { \
        .name = "Connection", \
        .value = "close", \
    }; \
    \
    res->headers.headers = cmem_alloc(memory_tag_response, sizeof(header) * header_count); \
    res->headers.headers[0] = content_type; \
    res->headers.headers[1] = content_length; \
    res->headers.headers[2] = connection; \
    \
    res->headers.header_count = header_count; \
    \
    if (res->body.data) { \
        ssize_t bytes_read = read(file_fd, res->body.data, file_stat.st_size); \
        res->body.data[file_stat.st_size] = '\0'; \
    } \
    \
    res->body.body_size = file_stat.st_size + 1; \
    \
    close(file_fd); \
}

void handle_sigpipe(int sig) {
    // Ignore SIGPIPE to prevent crashes when client disconnects
    fprintf(stderr, "Caught SIGPIPE, client disconnected unexpectedly\n");
}

void default_callback(request* req, response* res) {
    res->status_line.version = http_version_1p1;
    res->status_line.status_code = 404;
    res->status_line.reason_phrase = "Resource not found";

    int file_fd = open("assets/404.html", O_RDONLY);

    if (file_fd == -1) {
        res->body.data = NULL;
        res->body.body_size = 0;
        return;
    }

    struct stat file_stat;
    fstat(file_fd, &file_stat);

    res->body.data = cmem_alloc(memory_tag_string, file_stat.st_size + 1);

    int header_count = 3;
    header content_type = {
        .name = "Content-Type",
        .value = "text/html",
    };
    header content_length = {
        .name = "Content-Length",
        .value = asprintf("%i", file_stat.st_size),
    };
    header connection = {
        .name = "Connection",
        .value= "close",
    };

    res->headers.headers = cmem_alloc(memory_tag_response, sizeof(header) * header_count);
    res->headers.headers[0] = content_type;
    res->headers.headers[1] = content_length;
    res->headers.headers[2] = connection;

    res->headers.header_count = header_count;

    if (res->body.data)
    {
        ssize_t bytes_read = read(file_fd, res->body.data, file_stat.st_size);
        // Handle partial read or whatever.
        res->body.data[file_stat.st_size] = '\0';
    }
    
    res->body.body_size = file_stat.st_size + 1;

    close(file_fd);
}

FILE_CALLBACK(architecture_callback, "assets/architecture.html", 200, "OK");

void index_callback(request* req, response* res) {
    res->status_line.version = http_version_1p1;
    res->status_line.status_code = 200;
    res->status_line.reason_phrase = "OK";

    int file_fd = open("assets/index.html", O_RDONLY);

    if (file_fd == -1) {
        res->body.data = NULL;
        res->body.body_size = 0;
        return;
    }

    struct stat file_stat;
    fstat(file_fd, &file_stat);

    res->body.data = cmem_alloc(memory_tag_string, file_stat.st_size + 1);

    int header_count = 3;
    header content_type = {
        .name = "Content-Type",
        .value = "text/html",
    };
    header content_length = {
        .name = "Content-Length",
        .value = asprintf("%i", file_stat.st_size),
    };
    header connection = {
        .name = "Connection",
        .value= "close",
    };

    res->headers.headers = cmem_alloc(memory_tag_response, sizeof(header) * header_count);
    res->headers.headers[0] = content_type;
    res->headers.headers[1] = content_length;
    res->headers.headers[2] = connection;

    res->headers.header_count = header_count;

    if (res->body.data)
    {
        ssize_t bytes_read = read(file_fd, res->body.data, file_stat.st_size);
        // Handle partial read or whatever.
        res->body.data[file_stat.st_size] = '\0';
    }
    
    res->body.body_size = file_stat.st_size + 1;

    close(file_fd);
}

int main() {
    server_config s_conf = {
        .port = 8080,
    };

    server* s = server_create(s_conf);

    s->r = router_create(default_callback);

    route slash_route = {
        .method = http_method_get,
        .URI = "/",
        .callback = index_callback,
    };
    route index_route = {
        .method = http_method_get,
        .URI = "/index.html",
        .callback = index_callback,
    };
    route architecture_route = {
        .method = http_method_get,
        .URI = "/architecture.html",
        .callback = architecture_callback,
    };

    darray_add(s->r->routes, &slash_route);
    darray_add(s->r->routes, &index_route);
    darray_add(s->r->routes, &architecture_route);

    server_run(s);
}