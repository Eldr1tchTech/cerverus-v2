#include "client_manager.h"

#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/timer.h>

#include <sys/socket.h>
#include <unistd.h>

client_manager *client_manager_create()
{
    client_manager *c_man = cmem_alloc(memory_tag_client_manager, sizeof(client_manager));

    const char *host = "127.0.0.1";
    int port = 8080;

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    c_man->addr.sin_family = AF_INET;
    c_man->addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &c_man->addr.sin_addr);
}

void client_manager_destroy(client_manager *c_man)
{
    close(c_man->client_fd);
    cmem_free(memory_tag_client_manager, c_man);
    c_man = 0;
}

benchmark_result *client_manager_run(client_manager *c_man)
{
    if (connect(c_man->client_fd, (struct sockaddr *)&c_man->addr, sizeof(c_man->addr)) == -1)
    {
        perror("connect");
        return -1;
    }

    benchmark_result* bm_r = create_benchmark_result();
    bm_r->bm_v = benchmark_version_0p0p0;
    bm_r->benchmark_duration = 10;

    // Timer setup
    timer *t;
    timer_start(t);
    double duration = 10;
    double latency;

    // URI setup
    int i = 0;
    int i_size = sizeof(c_man->URIs) / sizeof(c_man->URIs[0]);

    // request setup
    char req[256];

    while (timer_stop(t) < duration)
    {
        snprintf(req, sizeof(req),
                 "GET %s HTTP/1.1\r\n"
                 /* "Host: %s\r\n" */
                 /* "Connection: close\r\n" */
                 "\r\n",
                 c_man->URIs[i]/* , host */);

        latency = timer_stop(t);
        send(c_man->client_fd, req, strlen(req), 0);

        char buf[256];
        ssize_t n;
        while ((n = recv(c_man->client_fd, buf, sizeof(buf) - 1, 0)) > 0)
        {
            buf[n] = '\0';
        }

        // Somehow read the data in now
        connection_benchmark_result conn_res = parse_buffer_result(buf);
        conn_res.duration = timer_stop(t) - latency;

        i++;
        i %= i_size;
    }
}