#include <network/request.h>
#include <network/server.h>
#include <network/router.h>
#include <core/memory/cmem.h>
#include <core/logger.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdarg.h>
#include <unistd.h>

// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <sys/sendfile.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <signal.h>

void handle_sigpipe(int sig) {
    // Ignore SIGPIPE to prevent crashes when client disconnects
    fprintf(stderr, "Caught SIGPIPE, client disconnected unexpectedly\n");
}

// TODO: Move this to it's own file in the library
char* asprintf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        LOG_DEBUG("asprintf - Tried to return value of length 0.");
    }

    char* str = cmem_alloc(memory_tag_string, needed + 1);
    if (str)
    {
        vsnprintf(str, needed + 1, fmt, args);
    }

    va_end(args);
    return str;
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
    res->headers.headers[0] = content_length;
    res->headers.headers[0] = connection;

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
    res->headers.headers[0] = content_length;
    res->headers.headers[0] = connection;

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
        .URI = "index.html",
        .callback = index_callback,
    };

    darray_add(s->r->routes, &slash_route);
    darray_add(s->r->routes, &index_route);

    server_run(s);
}

// #define PORT 8080
// #define BUFFER_SIZE 4096

// void handle_sigpipe(int sig) {
//     // Ignore SIGPIPE to prevent crashes when client disconnects
//     fprintf(stderr, "Caught SIGPIPE, client disconnected unexpectedly\n");
// }

// int main() {
//     // Handle broken pipe signals
//     signal(SIGPIPE, handle_sigpipe);
    
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         perror("Socket failed");
//         return 1;
//     }

//     // Set socket options
//     int opt = 1;
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
//         perror("Setsockopt failed");
//         close(server_fd);
//         return 1;
//     }

//     // Enable non-blocking mode (optional)
//     // fcntl(server_fd, F_SETFL, O_NONBLOCK);

//     struct sockaddr_in addr = {
//         .sin_family = AF_INET,
//         .sin_port = htons(PORT),
//         .sin_addr.s_addr = INADDR_ANY  // Listen on all interfaces
//     };

//     if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
//         perror("Bind failed");
//         fprintf(stderr, "Make sure port %d is not in use and firewall allows it\n", PORT);
//         close(server_fd);
//         return 1;
//     }

//     if (listen(server_fd, 10) == -1) {
//         perror("Listen failed");
//         close(server_fd);
//         return 1;
//     }

//     // Get and print server IP addresses
//     printf("Server started successfully!\n");
//     printf("Listening on port %d\n", PORT);
//     printf("Connect via:\n");
//     printf("  Local: http://localhost:%d\n", PORT);
//     printf("  Network: http://<your-ip-address>:%d\n", PORT);
//     printf("\nWaiting for connections...\n");

//     while (1) {
//         struct sockaddr_in client_addr;
//         socklen_t client_len = sizeof(client_addr);
        
//         int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
//         if (client_fd == -1) {
//             perror("Accept failed");
//             continue;
//         }

//         // Get client IP address
//         char client_ip[INET_ADDRSTRLEN];
//         inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
//         printf("\nNew connection from: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

//         char buffer[BUFFER_SIZE] = {0};
//         ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        
//         if (bytes_received <= 0) {
//             if (bytes_received == 0) {
//                 printf("Client disconnected\n");
//             } else {
//                 perror("Recv failed");
//             }
//             close(client_fd);
//             continue;
//         }

//         printf("Received %ld bytes\n", bytes_received);
//         printf("Request:\n%.*s\n", (int)bytes_received, buffer);
//         request* req = request_parse(buffer);
//         printf("PATH: %s.\n", req->request_line.URI);

//         // Simple HTTP request parsing
//         if (strncmp(buffer, "GET ", 4) != 0) {
//             printf("Not a GET request\n");
//             const char *bad_request = "HTTP/1.1 400 Bad Request\r\n\r\n";
//             send(client_fd, bad_request, strlen(bad_request), 0);
//             close(client_fd);
//             continue;
//         }

//         // Find the requested file path
//         char *path_start = buffer + 4;  // Skip "GET "
//         char *path_end = strchr(path_start, ' ');
//         if (!path_end) {
//             printf("Malformed request\n");
//             const char *bad_request = "HTTP/1.1 400 Bad Request\r\n\r\n";
//             send(client_fd, bad_request, strlen(bad_request), 0);
//             close(client_fd);
//             continue;
//         }

//         // Extract filename
//         char filename[256] = {0};
//         size_t path_len = path_end - path_start;
        
//         if (path_len == 1 && path_start[0] == '/') {
//             // Root path, serve index.html
//             strcpy(filename, "assets/index.html");
//         } else if (path_len > 1) {
//             // Skip leading slash if present
//             const char *actual_path = (path_start[0] == '/') ? path_start + 1 : path_start;
//             size_t actual_len = (path_start[0] == '/') ? path_len - 1 : path_len;
            
//             // Security check
//             if (strstr(actual_path, "..") != NULL) {
//                 printf("Directory traversal attempt detected: %s\n", actual_path);
//                 const char *forbidden = "HTTP/1.1 403 Forbidden\r\n\r\n";
//                 send(client_fd, forbidden, strlen(forbidden), 0);
//                 close(client_fd);
//                 continue;
//             }
            
//             snprintf(filename, sizeof(filename), "assets/%.*s", (int)actual_len, actual_path);
//         } else {
//             // Empty or invalid path
//             strcpy(filename, "assets/index.html");
//         }

//         printf("Requested file: %s\n", filename);

//         // Try to open the file
//         int file_fd = open(filename, O_RDONLY);
        
//         if (file_fd == -1) {
//             printf("File not found: %s (%s)\n", filename, strerror(errno));
            
//             // Try to serve 404.html if it exists
//             int error_fd = open("assets/404.html", O_RDONLY);
//             if (error_fd != -1) {
//                 struct stat file_stat;
//                 fstat(error_fd, &file_stat);
                
//                 char header[512];
//                 int header_len = snprintf(header, sizeof(header),
//                     "HTTP/1.1 404 Not Found\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Content-Length: %ld\r\n"
//                     "Connection: close\r\n"
//                     "\r\n", file_stat.st_size);
                
//                 send(client_fd, header, header_len, 0);
//                 off_t offset = 0;
//                 sendfile(client_fd, error_fd, &offset, file_stat.st_size);
//                 close(error_fd);
//             } else {
//                 const char *not_found = 
//                     "HTTP/1.1 404 Not Found\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Content-Length: 48\r\n"
//                     "Connection: close\r\n"
//                     "\r\n"
//                     "<html><body><h1>404 Not Found</h1></body></html>";
//                 send(client_fd, not_found, strlen(not_found), 0);
//             }
//         } else {
//             // File found, serve it
//             struct stat file_stat;
//             fstat(file_fd, &file_stat);
            
//             // Determine content type based on file extension
//             const char *content_type = "text/html";
//             char *ext = strrchr(filename, '.');
//             if (ext) {
//                 if (strcmp(ext, ".css") == 0) content_type = "text/css";
//                 else if (strcmp(ext, ".js") == 0) content_type = "application/javascript";
//                 else if (strcmp(ext, ".png") == 0) content_type = "image/png";
//                 else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
//                     content_type = "image/jpeg";
//                 else if (strcmp(ext, ".gif") == 0) content_type = "image/gif";
//                 else if (strcmp(ext, ".json") == 0) content_type = "application/json";
//             }
            
//             char header[512];
//             int header_len = snprintf(header, sizeof(header),
//                 "HTTP/1.1 200 OK\r\n"
//                 "Content-Type: %s\r\n"
//                 "Content-Length: %ld\r\n"
//                 "Connection: close\r\n"
//                 "\r\n", content_type, file_stat.st_size);
            
//             // Send headers
//             ssize_t sent = send(client_fd, header, header_len, 0);
//             if (sent < 0) {
//                 perror("Failed to send headers");
//             } else {
//                 // Send file content
//                 off_t offset = 0;
//                 ssize_t file_sent = sendfile(client_fd, file_fd, &offset, file_stat.st_size);
//                 if (file_sent < 0) {
//                     perror("Failed to send file");
//                 } else {
//                     printf("File sent successfully: %s (%ld bytes)\n", filename, file_stat.st_size);
//                 }
//             }
//             close(file_fd);
//         }
        
//         close(client_fd);
//         printf("Connection closed\n");
//     }

//     close(server_fd);
//     return 0;
// }