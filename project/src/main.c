#include <network/request.h>
#include <network/server.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

void handle_sigpipe(int sig) {
    // Ignore SIGPIPE to prevent crashes when client disconnects
    fprintf(stderr, "Caught SIGPIPE, client disconnected unexpectedly\n");
}

int main() {
    server_config s_conf = {
        .port = 8080,
    };

    server* s = server_create(s_conf);

    server_run(s);
}