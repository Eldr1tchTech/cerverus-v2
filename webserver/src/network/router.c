#include "router.h"

#include "core/util/logger.h"
#include "core/memory/cmem.h"
#include "core/util/profiler.h"

#include <string.h>

router* router_create(route_callback default_route) {
    router* r = cmem_alloc(memory_tag_router, sizeof(router));

    r->default_route = default_route;
    r->routes = darray_create(8, sizeof(route));

    return r;
}

// This is a bad way to do it if sendfile is to be used.
void router_handle_route(router* r, request* req, response* res) {
    if (!r)
    {
        LOG_ERROR("router_handle_route - Please provide a valid router argument.");
        return;
    }
    
    route* routes_data = (route*)r->routes->data;
    for (int i = 0; i < r->routes->length; i++)
    {
        LOG_DEBUG("attempting to match route");
        route rt = routes_data[i];
        if (rt.method == req->request_line.method)
        {
            LOG_DEBUG("method matches.");
            if (strcmp(rt.URI, req->request_line.URI) == 0)
            {
                LOG_DEBUG("URI matches.");

                profile_operation("callback", rt.callback(req, res));

                return;
            }
        }
    }
    
    r->default_route(req, res);
}

void router_destroy(router* r) {
    darray_destroy(r->routes);
    cmem_free(memory_tag_router, r);
    r = 0;
}