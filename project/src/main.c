#include <network/request.h>
#include <network/server.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>

int main() {
    server_config s_conf = {
        .port = 8080,
    };

    server* s = server_create(s_conf);

    server_run(s);
}