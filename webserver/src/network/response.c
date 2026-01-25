#include "response.h"

#include "core/util/logger.h"
#include "core/memory/cmem.h"

#include <stdio.h>
#include <string.h>

char *serialize_http_version(http_version version)
{
    switch (version)
    {
    case http_version_1p1:
        return "HTTP/1.1";
    default:
        LOG_ERROR("Unable to serialize unknown http_version.");
        return NULL;
    }
}

char *response_serialize(response *res)
{
    char *raw_res = cmem_alloc(memory_tag_response, 8192 * sizeof(char));
    size_t offset = 0;
    size_t capacity = 8192;

    const char *version = serialize_http_version(res->status_line.version);
    if (!version)
    {
        LOG_ERROR("response_serialize - No version value.");
        version = "HTTP/1.1"; // or assert / log fatal
    }

    offset += snprintf(raw_res + offset, capacity - offset,
                       "%s %i %s\r\n",
                       version,
                       res->status_line.status_code,
                       res->status_line.reason_phrase);

    for (size_t i = 0; i < res->headers.header_count; i++)
    {
        offset += snprintf(raw_res + offset, capacity - offset,
                           "%s: %s\r\n",
                           res->headers.headers[i].name,
                           res->headers.headers[i].value);
    }

    offset += snprintf(raw_res + offset, capacity - offset, "\r\n");

    if (res->body.body_size > 0)
    {
        cmem_mcpy(raw_res + offset, res->body.data, res->body.body_size);
        offset += res->body.body_size;
    }
    return raw_res;
}

char *content_type_val_helper(const char *ext)
{
    if (ext)
    {
        if (strcmp(ext + 1, "html") == 0)
        {
            return "text/html";
        }
        else if (strcmp(ext + 1, "css") == 0)
        {
            return "text/css";
        }
        else
        {
            LOG_ERROR("content_type_val_helper - Currently unsuported file extension: %s. Returning null.", ext);
            return NULL;
        }
    }
    LOG_ERROR("content_type_val_helper - Please provide a valid char* for ext.");
    return NULL;
}