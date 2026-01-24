#pragma once

//Use this in format string arguments paired with %s to print a boolean.
#define LOGGER_BOOL(expr) expr ? "true" : "false"

// ANSI codes
#define ANSI_RESET       "\x1b[0m"

#define ANSI_RED         "\x1b[38;5;196m"
#define ANSI_GREEN       "\x1b[38;5;76m"
#define ANSI_YELLOW      "\x1b[38;5;11m"
#define ANSI_BLUE        "\x1b[38;5;33m"
#define ANSI_MAGENTA     "\x1b[38;5;199m"
#define ANSI_TURQUOISE   "\x1b[38;5;86m"
#define ANSI_BOLD        "\x1b[1m"

#define LOG_LEVELS(X) \
    X(LOG_LEVEL_FATAL, ANSI_MAGENTA ANSI_BOLD, "[FATAL]") \
    X(LOG_LEVEL_ERROR, ANSI_RED ANSI_BOLD, "[ERROR]") \
    X(LOG_LEVEL_WARNING, ANSI_YELLOW, "[WARNING]") \
    X(LOG_LEVEL_INFO, ANSI_TURQUOISE, "[INFO]") \
    X(LOG_LEVEL_DEBUG, ANSI_BLUE, "[DEBUG]")

#define X_ENUM(name, color, tag) name,
typedef enum LOG_LEVEL {
    LOG_LEVELS(X_ENUM)
} LOG_LEVEL;
#undef X_ENUM

extern const char* log_colors[];
extern const char* log_tags[];

/// @brief This should not be called directly. The logger macros should be used instead.
void logger_log(LOG_LEVEL log_level, char* format, ...);

/// Logs a fatal error in magenta. If in debug mode, a breakpoint is signalled as well.
#define LOG_FATAL(fmt, ...)   logger_log(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
/// Logs an error in red. If in debug mode, a breakpoint is signalled as well.
#define LOG_ERROR(fmt, ...)   logger_log(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
/// Logs a warning in yellow.
#define LOG_WARNING(fmt, ...) logger_log(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
/// Logs an informational message in turquoise.
#define LOG_INFO(fmt, ...)    logger_log(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#ifdef _DEBUG
    /// Logs a debug message in blue. If not in debug mode, this does nothing.
    #define LOG_DEBUG(fmt, ...)   logger_log(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
    /// Logs a debug message in blue. If not in debug mode, this does nothing.
    #define LOG_DEBUG(fmt, ...)
#endif