#include <network/request.h>
#include <network/server.h>
#include <network/response.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>
#include <network/route.h>

#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>

void random_image_provider_callback(request *req, int client_fd)
{
    char* file_name = asprintf("assets/images/img_%s.jpg", req->request_line.URI + 5);
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd != -1)
    {
        send_file_response(client_fd, file_fd, 200, "OK", ".html");
        return;
    }
    cmem_free(memory_tag_string, file_name);
}

void random_image_callback(request *req, int client_fd)
{
    LOG_DEBUG("called random image.");
    char *body = asprintf("<img src=\"/pic/%i\" width=200>", rand() % 3);

    response *res = response_create(strlen(body));
    res->status_line.version = http_version_1p1;
    res->status_line.status_code = 200;
    res->status_line.reason_phrase = "OK";
    res->body.data = body;
    res->body.body_size = strlen(body);

    header h;
    h.name = "Content-Type";
    h.value = "text/html";
    darray_add(res->headers, &h);

    h.name = "Content-Length";
    char* content_length_str = asprintf("%zu", strlen(body));
    h.value = content_length_str;
    darray_add(res->headers, &h);

    h.name = "Connection";
    h.value = "close";
    darray_add(res->headers, &h);

    char *raw = response_serialize(res);
    send(client_fd, raw, strlen(raw), MSG_NOSIGNAL);
    cmem_free(memory_tag_response, raw);
    cmem_free(memory_tag_string, content_length_str);
}

int main()
{
    server *s = server_create();
    srand(0);

    route *rt;

    rt = route_create(http_method_get, "/random_image", random_image_callback);
    server_add_route(s, rt);

    rt = route_create(http_method_get, "/pic/:id", random_image_provider_callback);
    server_add_route(s, rt);

    server_run(s);
}