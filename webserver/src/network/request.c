#include "request.h"

#include "core/memory/cmem.h"
#include "core/logger.h"

#include <string.h>

http_method parse_http_method(char* raw_method) {
    if (strcmp(raw_method, "GET") == 0)
    {
        return http_method_get;
    }
    LOG_DEBUG("parse_http_method - Unable to parse an http_method from the provided string: %s.", raw_method);
    return http_method_unknown;
}

http_version parse_http_version(char* raw_version) {
    if (strcmp(raw_version, "HTTP/1.1") == 0)
    {
        return http_version_1p1;
    }
    LOG_DEBUG("parse_http_version - Unable to parse an http_version from the provided string: %s.", raw_version);
    return http_version_unknown;
}

void parse_request_line(request* req, char* raw_req_lin) {
    req->request_line.method = parse_http_method(strtok(raw_req_lin, " "));
    req->request_line.URI = strtok(NULL, " ");
    req->request_line.version = parse_http_version(strtok(NULL, "\0"));
}

void parse_headers(request* req, char* raw_headers) {
    if (!raw_headers || strlen(raw_headers) == 0) {
        req->headers.headers = NULL;
        req->headers.header_count = 0;
        return;
    }
    
    // First pass: count the number of headers
    int header_count = 0;
    char* temp = raw_headers;
    while (*temp != '\0') {
        if (*temp == '\r' && *(temp + 1) == '\n') {
            header_count++;
            temp += 2;
        } else {
            temp++;
        }
    }
    
    // Allocate memory for headers
    req->headers.header_count = header_count;
    req->headers.headers = cmem_alloc(memory_tag_request, sizeof(header) * header_count);
    
    // Second pass: parse each header
    char* line = strtok(raw_headers, "\r\n");
    int index = 0;
    
    while (line != NULL && index < header_count) {
        // Find the colon separator
        char* colon = strchr(line, ':');
        if (colon != NULL) {
            // Split at the colon
            *colon = '\0';
            
            // Header name is everything before the colon
            req->headers.headers[index].name = line;
            
            // Header value is everything after the colon (skip leading space)
            char* value = colon + 1;
            while (*value == ' ') {
                value++;
            }
            req->headers.headers[index].value = value;
            
            index++;
        } else {
            LOG_DEBUG("parse_headers - Malformed header line: %s", line);
        }
        
        line = strtok(NULL, "\r\n");
    }
}

request* request_parse(char* raw_req) {
    request* req = cmem_alloc(memory_tag_request, sizeof(request));

    char* raw_req_cpy = cmem_alloc(memory_tag_request, sizeof(char) * 4096);
    strcpy(raw_req_cpy, raw_req);

    char* index = strstr(raw_req_cpy, "\r\n");
    *index = '\0';
    parse_request_line(req, raw_req_cpy);
    raw_req_cpy = index + 2;
    index = strstr(raw_req_cpy, "\r\n\r\n");
    *index = '\0';
    parse_headers(req, raw_req_cpy);
    raw_req_cpy = index + 4;
    req->body.body_size = strlen(raw_req_cpy);
    req->body.data = raw_req_cpy;

    return req;
}