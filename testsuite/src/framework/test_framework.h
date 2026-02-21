#pragma once

#include <stdio.h>
#include <string.h>

#define ANSI_RESET  "\x1b[0m"
#define ANSI_RED    "\x1b[38;5;196m"
#define ANSI_GREEN  "\x1b[38;5;76m"
#define ANSI_BOLD   "\x1b[1m"
#define ANSI_DIM    "\x1b[2m"

// ─── Result type ─────────────────────────────────────────────────────────────

typedef enum { TEST_PASS, TEST_FAIL } test_result;

// ─── Registry ────────────────────────────────────────────────────────────────

typedef struct {
    const char* suite;
    const char* name;
    test_result (*fn)(void);
} test_case;

#define TEST_REGISTRY_MAX 256
extern test_case _registry[TEST_REGISTRY_MAX];
extern int       _registry_count;

// ─── Failure context ─────────────────────────────────────────────────────────

extern char        _fail_msg[256];
extern const char* _fail_file;
extern int         _fail_line;

// ─── Macros ──────────────────────────────────────────────────────────────────

#define TEST(suite, name) \
    test_result test_##suite##_##name(void)

#define REGISTER_TEST(suite, name) \
    _registry[_registry_count++] = (test_case){ #suite, #name, test_##suite##_##name }

#define ASSERT(expr) \
    do { if (!(expr)) { \
        snprintf(_fail_msg, sizeof(_fail_msg), "ASSERT(" #expr ")"); \
        _fail_file = __FILE__; _fail_line = __LINE__; \
        return TEST_FAIL; \
    }} while(0)

#define ASSERT_EQ(a, b) \
    do { if ((a) != (b)) { \
        snprintf(_fail_msg, sizeof(_fail_msg), \
            #a " == " #b " | got %lld, expected %lld", (long long)(a), (long long)(b)); \
        _fail_file = __FILE__; _fail_line = __LINE__; \
        return TEST_FAIL; \
    }} while(0)

// ─── Runner ──────────────────────────────────────────────────────────────────

int test_run_all(void);