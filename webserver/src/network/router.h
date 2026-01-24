#pragma once

#include "core/containers/darray.h"
#include "network_types.inl"

// TODO: Change this to use a command buffer of some sort for the server?
typedef void (*route_callback)(request* req, response* res);

typedef struct route
{
    http_method method;
    char* URI;
    route_callback callback;
} route;

typedef struct router
{
    darray* routes;
    route_callback default_route;    // 404 route for now.
} router;

router* router_create(route_callback default_route);

void router_handle_route(router* r, request* req, response* res);

void router_destroy(router* r);