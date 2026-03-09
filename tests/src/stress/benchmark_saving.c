#include "benchmark_saving.h"

#include <core/util/logger.h>

#include <sys/stat.h>
#include <time.h>

void create_dir(char *date_str)
{
    // Create benchmarks dir worst case scenario
    mkdir("../benchmarks", 0755);

    // Create benchmark directory
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(date_str, sizeof(date_str), "%04d_%02d_%02d_%02d%02d%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    char dirname[256];
    snprintf(dirname, sizeof(dirname),
             "../benchmarks/%s",
             date_str);

    mkdir(dirname, 0755);
}

void write_benchmark(benchmark_result *benchmark_result)
{
    char date_str[256];
    create_dir(date_str);

    // Write .txt
    char file_name[256];
    snprintf(file_name, sizeof(file_name), "benchmarks/%s/report.txt", date_str);
    FILE *file = fopen(file_name, "w");
    if (!file)
    {
        LOG_ERROR("write_benchmark - Failed to open file for writing");
        return;
    }

    printf("Please provide short summary of key differences between this and the last regarding results.");
    char response[512] = {0};
    fgets(response, sizeof(response), stdin);

    fprintf(file, "Report - v0.1.0\n");
    fprintf(file, "Summary:\n");
    fprintf(file, response);

    fclose(file);

    // Write .csv
    snprintf(file_name, sizeof(file_name), "benchmarks/%s/data.csv", date_str);
    file = fopen("benchmarks//data.csv", "a"); // "a" for append

    connection_benchmark_result* darr_data = benchmark_result->conn_bm_result->data;
    for (int i = 0; i < benchmark_result->conn_bm_result->length; i++)
    {
        fprintf(file, "%.6d,%s,%d\n", darr_data[i].duration, darr_data[i].URI, darr_data[i].status_code);
    }

    fclose(file);

    // Free everything...
    
}