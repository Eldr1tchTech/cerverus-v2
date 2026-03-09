#include "benchmark_saving.h"

#include <core/util/logger.h>

#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <errno.h>

void create_dir(char *date_str, size_t date_str_size)
{
    // Create benchmarks dir if it doesn't exist
    struct stat st = {0};
    if (stat("../benchmarks", &st) == -1) {
        if (mkdir("../benchmarks", 0755) == -1) {
            LOG_ERROR("create_dir - Failed to create benchmarks directory: %s", strerror(errno));
            return;
        }
    }

    // Create benchmark directory with timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    snprintf(date_str, date_str_size, "%04d_%02d_%02d_%02d%02d%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    
    char dirname[256];
    snprintf(dirname, sizeof(dirname), "../benchmarks/%s", date_str);

    if (mkdir(dirname, 0755) == -1) {
        LOG_ERROR("create_dir - Failed to create benchmark directory %s: %s", dirname, strerror(errno));
        return;
    }
    
    LOG_INFO("Created benchmark directory: %s", dirname);
}

void write_benchmark(benchmark_result *benchmark_result)
{
    char date_str[256];
    create_dir(date_str, sizeof(date_str));

    // Write .txt report
    char file_name[256];
    snprintf(file_name, sizeof(file_name), "../benchmarks/%s/report.txt", date_str);
    FILE *file = fopen(file_name, "w");
    if (!file)
    {
        LOG_ERROR("write_benchmark - Failed to open file for writing: %s", file_name);
        return;
    }

    printf("Please provide short summary of key differences between this and the last regarding results: ");
    fflush(stdout);
    char response[512] = {0};
    if (fgets(response, sizeof(response), stdin))
    {
        // Remove trailing newline
        response[strcspn(response, "\n")] = 0;
    }

    fprintf(file, "Report - v0.1.0\n");
    fprintf(file, "Summary:\n");
    fprintf(file, "%s\n", response);

    fclose(file);

    // Write .csv data
    snprintf(file_name, sizeof(file_name), "../benchmarks/%s/data.csv", date_str);
    file = fopen(file_name, "w");
    if (!file)
    {
        LOG_ERROR("write_benchmark - Failed to open CSV file for writing: %s", file_name);
        return;
    }

    // Write CSV header
    fprintf(file, "duration,URI,status_code\n");

    connection_benchmark_result* darr_data = benchmark_result->conn_bm_result->data;
    for (int i = 0; i < benchmark_result->conn_bm_result->length; i++)
    {
        fprintf(file, "%.6f,%s,%d\n", 
                darr_data[i].duration, 
                darr_data[i].URI, 
                darr_data[i].status_code);
    }

    fclose(file);

    printf("Benchmark results saved to: ../benchmarks/%s/\n", date_str);
    LOG_INFO("Benchmark complete: %d requests recorded", benchmark_result->conn_bm_result->length);
}