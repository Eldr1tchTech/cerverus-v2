#include "test_framework.h"

test_case _registry[TEST_REGISTRY_MAX];
int       _registry_count = 0;

char        _fail_msg[256] = {0};
const char* _fail_file     = NULL;
int         _fail_line     = 0;

int test_run_all(void) {
    int passed = 0, failed = 0;
    const char* current_suite = NULL;

    for (int i = 0; i < _registry_count; i++) {
        test_case* tc = &_registry[i];

        if (!current_suite || strcmp(current_suite, tc->suite) != 0) {
            printf("\n" ANSI_BOLD "  %s\n" ANSI_RESET, tc->suite);
            current_suite = tc->suite;
        }

        _fail_msg[0] = '\0';
        _fail_file   = NULL;
        _fail_line   = 0;

        test_result result = tc->fn();

        if (result == TEST_PASS) {
            printf("    " ANSI_GREEN "✓" ANSI_RESET " %s\n", tc->name);
            passed++;
        } else {
            printf("    " ANSI_RED "✗" ANSI_RESET " %s\n", tc->name);
            printf("      " ANSI_RED "%s" ANSI_RESET "\n", _fail_msg);
            printf("      " ANSI_DIM "%s:%d\n" ANSI_RESET, _fail_file, _fail_line);
            failed++;
        }
    }

    printf("\n" ANSI_DIM "  ────────────────────────────\n" ANSI_RESET);
    printf("  " ANSI_GREEN "%d passed" ANSI_RESET "  ");
    if (failed) printf(ANSI_RED "%d failed" ANSI_RESET, failed);
    else        printf(ANSI_DIM "%d failed" ANSI_RESET, failed);
    printf("\n\n");

    return failed;
}