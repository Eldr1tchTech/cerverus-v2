// This is the entry point for the stress test part of the testing.
// This will start the project (bin/project) and then a client, and
#include "stress/client_manager.h"
#include "stress/benchmark_saving.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void run_stress_tests()
{
    printf("Running stress tests.\n");
    printf("1. Start the server.\n");

    pid_t pid;
    pid = fork();

    if (pid < 0)
    {
        printf("run_stress_tests - Fork failed.");
        return;
    }
    else if (pid == 0)
    {
        // Child process - run the server
        // cwd is bin/, so project is in current directory
        execl("./project", "project", NULL);

        // If we get here, exec failed
        printf("run_stress_tests - Exec failed.");
        _exit(1);  // Use _exit() in child process
    }
    else
    {
        printf("2. Start the client.\n");

        client_manager* c_man = client_manager_create();
        if (!c_man) {
            printf("run_stress_tests - Failed to create client manager.\n");
            return;
        }
        
        // Fix: Use proper array syntax
        char* URIs[] = {"/index.html", "/style.css", "/architecture.html", "/features.html"};
        c_man->URIs = URIs;

        printf("3. Wait until server is running.\n");
        sleep(5);  // TODO: Better synchronization

        printf("4. Send a request, get response, save info, repeat (for a set amount of time).\n");
        benchmark_result* bm_r = client_manager_run(c_man);
        
        if (bm_r) {
            printf("6. Save results into a file.\n");
            write_benchmark(bm_r);
        } else {
            printf("Benchmark failed - no results to save.\n");
        }
    }
}