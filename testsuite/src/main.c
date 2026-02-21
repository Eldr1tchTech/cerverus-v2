// 1. Unit tests, if all succeed, proceed to stress testing.
    // darray
    // request
    // response
// 2. Start webserver, then client.
    // Figure out how to start in seperate processes, but still quit both at same time?

#include "framework/test_framework.h"
#include "tests/test_darray.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main(void) {
    // Unit tests
    register_darray_tests();
    return test_run_all();

    // Stress tests/benchmarking
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t server_pid = fork();

    if (server_pid == 0) {
        // --- SERVER CHILD PROCESS ---
        close(pipefd[0]); // Close unused read end
        
        // Redirect stdout to the pipe so the parent can read it
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char *args[] = {"./project", NULL};
        execv(args[0], args);
        perror("execv server");
        exit(1);
    } 

    // --- PARENT PROCESS ---
    close(pipefd[1]); // Close unused write end

    char buffer[128];
    int port = 0;
    FILE *stream = fdopen(pipefd[0], "r");

    // Wait for the server to output the port
    printf("[Runner] Waiting for server to signal port...\n");
    while (fgets(buffer, sizeof(buffer), stream)) {
        if (sscanf(buffer, "LISTENING_ON: %d", &port) == 1) {
            break;
        }
    }

    if (port == 0) {
        fprintf(stderr, "Failed to get port from server\n");
        kill(server_pid, SIGTERM);
        return 1;
    }
    printf("[Runner] Server is up on port: %d\n", port);

    char port_str[10];
    sprintf(port_str, "%d", port);

    // --- START CLIENT 1 ---
    printf("[Runner] Starting Client 1...\n");
    pid_t client1 = fork();
    if (client1 == 0) {
        execl("./client", "client", "localhost", port_str, "--test-a", NULL);
        exit(1);
    }
    waitpid(client1, NULL, 0); // Wait for Client 1 to finish
    printf("[Runner] Client 1 finished.\n");

    // --- START CLIENT 2 ---
    printf("[Runner] Starting Client 2...\n");
    pid_t client2 = fork();
    if (client2 == 0) {
        execl("./client", "client", "localhost", port_str, "--test-b", NULL);
        exit(1);
    }
    waitpid(client2, NULL, 0); // Wait for Client 2 to finish
    printf("[Runner] Client 2 finished.\n");

    // --- CLEANUP ---
    printf("[Runner] Tearing down server...\n");
    kill(server_pid, SIGTERM);
    waitpid(server_pid, NULL, 0);

    printf("[Runner] All tests passed.\n");
    return 0;
    
}