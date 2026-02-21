#include <core/util/logger.h>

#include <stdio.h>
#include <string.h>

typedef enum test_type {
    test_type_smoke,
    test_type_peak_load,
    test_type_soak,
    test_type_step,
    test_type_undefined,
} test_type;

const char* routes = {
    "/",
    "/index.html",
    "/style.css",
    "/features.html",
    "/architecture.html",
};

test_type parse_test_type(char* str) {
    if (strcmp("smoke", str) == 0)
    {
        return test_type_smoke;
    } else if (strcmp("peak_load", str) == 0)
    {
        return test_type_peak_load;
    } else if (strcmp("soak", str) == 0)
    {
        return test_type_soak;
    } else if (strcmp("step", str) == 0)
    {
        return test_type_step;
    } else {
        LOG_ERROR("Undefined test type passed: %s.", str);
        return test_type_undefined;
    }
}

void send_request();
int validate_response();

/* TODO: Maybe add these
- Concurrency ceiling test
- Mixed request test
- thundering herd test
*/

int main (int argc, char** argv) {
    if (strcmp(argv[1], "help") == 0)
    {
        LOG_INFO("arg0: IP address.");
        LOG_INFO("arg1: Port number.");
        LOG_INFO("arg2: Test type.");
        LOG_INFO("Supported test types for the application:");
        LOG_INFO("- smoke: Quick sanity checks to verify basic functionality");
        LOG_INFO("- peak_load: Stress tests that push the system to maximum capacity");
        LOG_INFO("- soak: Long-duration tests to detect memory leaks and stability issues");
        LOG_INFO("- step: Gradual load increase tests to find performance degradation points");
        LOG_INFO("For more information check the official documentation in the README.md");
    }
    
    // Parse arguments.
    if (argc < 4)
    {
        LOG_INFO("%i", argc);
        LOG_FATAL("Called with bad arguments.\n  Call with argument 'help' for more details.");
        return 1;
    }

    

    return 0;
}