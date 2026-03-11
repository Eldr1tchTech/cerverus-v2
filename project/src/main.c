#include <network/request.h>
#include <network/server.h>
#include <network/response.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>

#include <string.h>
#include <sys/socket.h>
#include <stdio.h>

void htmx_test_callback(request* req, int client_fd) {
    char* text = "<img src=\"/jakob_jumpscare.jpg\" width=200>";

    char* htmx_text = cmem_alloc(memory_tag_response, strlen(text) + 1);
    strcpy(htmx_text, text);

    response *res = response_create(strlen(htmx_text));

    res->status_line.version = http_version_1p1;
    res->status_line.status_code = 200;
    res->status_line.reason_phrase = "OK";

    res->body.data = htmx_text;
    res->body.body_size = strlen(text);

    header h;

    h.name = "Content-Type";
    h.value = "text/html";
    darray_add(res->headers, &h);

    h.name = "Content-Length";
    char *content_length_str;
    asprintf(&content_length_str, "%zu", strlen(htmx_text));
    h.value = content_length_str;
    darray_add(res->headers, &h);

    h.name = "Connection";
    h.value = "close";
    darray_add(res->headers, &h);

    char *raw = response_serialize(res);
    send(client_fd, raw, strlen(raw), 0);
}

int main() {
    server* s = server_create();

    route rt = {
        .method = http_method_get,
        .URI = "/htmx-test",
        .callback = htmx_test_callback,
    };

    server_add_route(s, &rt);

    server_run(s);
}