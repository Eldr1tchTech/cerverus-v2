#include <network/request.h>
#include <network/server.h>
#include <core/memory/cmem.h>
#include <core/util/logger.h>
#include <core/util/util.h>

int main() {
    server* s = server_create();

    server_run(s);
}