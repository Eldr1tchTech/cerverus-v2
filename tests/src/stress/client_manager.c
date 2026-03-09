#include "client_manager.h"

#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/timer.h>

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

client_manager *client_manager_create()
{
    client_manager *c_man = cmem_alloc(memory_tag_client_manager, sizeof(client_manager));

    const char *host = "127.0.0.1";
    int port = 8080;

    // Set up address structure (reused for each connection)
    c_man->addr.sin_family = AF_INET;
    c_man->addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &c_man->addr.sin_addr);
    
    // Initialize client_fd to -1 (will be created per request)
    c_man->client_fd = -1;

    return c_man;
}

void client_manager_destroy(client_manager *c_man)
{
    if (c_man->client_fd != -1) {
        close(c_man->client_fd);
    }
    cmem_free(memory_tag_client_manager, c_man);
    c_man = 0;
}

// Helper function to create and connect a new socket
static int connect_to_server(client_manager *c_man)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        LOG_ERROR("connect_to_server - socket creation failed: %s", strerror(errno));
        return -1;
    }

    if (connect(fd, (struct sockaddr *)&c_man->addr, sizeof(c_man->addr)) == -1)
    {
        LOG_DEBUG("connect_to_server - connection failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

benchmark_result *client_manager_run(client_manager *c_man)
{
    benchmark_result* bm_r = create_benchmark_result();
    bm_r->bm_v = benchmark_version_0p0p0;
    bm_r->benchmark_duration = 10.0;

    // Timer setup
    timer t;
    timer_start(&t);
    double duration = 10.0;
    
    // URI setup
    int uri_index = 0;
    int uri_count = 4;  // We know there are 4 URIs

    // Request buffer
    char req[512];
    char buf[8192];
    
    int request_count = 0;
    int failed_requests = 0;

    LOG_INFO("Starting benchmark for %.1f seconds...", duration);

    while (timer_stop(&t) < duration)
    {
        // Create new connection for each request (server closes after response)
        int fd = connect_to_server(c_man);
        if (fd == -1) {
            failed_requests++;
            continue;
        }

        // Record start time for this request
        double request_start = timer_stop(&t);

        // Build request
        snprintf(req, sizeof(req),
                 "GET %s HTTP/1.1\r\n"
                 "Host: 127.0.0.1\r\n"
                 "Connection: close\r\n"
                 "\r\n",
                 c_man->URIs[uri_index]);

        // Send request
        ssize_t sent = send(fd, req, strlen(req), 0);
        if (sent == -1)
        {
            LOG_DEBUG("send failed: %s", strerror(errno));
            close(fd);
            failed_requests++;
            continue;
        }

        // Receive response
        ssize_t n;
        int total_received = 0;
        int headers_complete = 0;
        
        while ((n = recv(fd, buf + total_received, sizeof(buf) - total_received - 1, 0)) > 0)
        {
            total_received += n;
            buf[total_received] = '\0';
            
            // Check if we've received complete headers
            if (!headers_complete && strstr(buf, "\r\n\r\n"))
            {
                headers_complete = 1;
                // For now, we'll just break after getting headers
                // In a real implementation, we'd parse Content-Length and read the body
                break;
            }
            
            // Safety check - don't overflow buffer
            if (total_received >= sizeof(buf) - 1) {
                break;
            }
        }

        // Calculate request duration
        double request_duration = timer_stop(&t) - request_start;

        // Close the connection
        close(fd);

        // Parse response and record result
        if (total_received > 0) {
            connection_benchmark_result conn_res;
            conn_res.duration = request_duration;
            conn_res.URI = c_man->URIs[uri_index];
            
            // Parse status code from response
            char* status_start = strchr(buf, ' ');
            if (status_start) {
                conn_res.status_code = atoi(status_start + 1);
            } else {
                conn_res.status_code = 0;
            }
            
            darray_add(bm_r->conn_bm_result, &conn_res);
            request_count++;
        } else {
            failed_requests++;
        }

        // Move to next URI
        uri_index = (uri_index + 1) % uri_count;
    }

    LOG_INFO("Benchmark complete: %d requests sent, %d failed", request_count, failed_requests);

    return bm_r;
}