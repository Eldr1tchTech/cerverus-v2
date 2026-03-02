// This is the entry point for the stress test part of the testing.
// This will start the project (bin/project) and then a client, and 
#include <stdio.h>

void run_stress_tests() {
    printf("Running stress tests.\n");
    printf("1. Start the server.\n");
    printf("2. Start the client.\n");
    printf("3. Wait until server is running.\n");
    printf("4. Send a request, get response, save info, repeat (for a set amount of time).\n");
    printf("6. Save results into a file.\n"); // NOTE: command line input?
}

// int main() {
//     pid_t pid;
    
//     printf("Parent process starting (PID: %d)\n", getpid());
    
//     // Create a child process
//     pid = fork();
    
//     if (pid < 0) {
//         // Fork failed
//         fprintf(stderr, "Fork failed!\n");
//         return 1;
//     }
//     else if (pid == 0) {
//         // Child process
//         printf("Child process created (PID: %d, Parent PID: %d)\n", 
//                getpid(), getppid());
//         printf("Child: About to exec into bad_at program...\n");
        
//         // Replace child process with bad_at program
//         execl("./bad_at", "bad_at", "Bob", "cooking", NULL);
        
//         // If exec succeeds, this code never runs
//         // If we get here, exec failed
//         fprintf(stderr, "exec failed!\n");
//         exit(1);
//     }
//     else {
//         // Parent process
//         printf("Parent process continuing (PID: %d, Child PID: %d)\n", 
//                getpid(), pid);
        
//         // Wait for child to complete
//         int status;
//         wait(&status);
//         printf("Parent: Child process has finished.\n");
//         printf("Parent process exiting...\n");
//     }
    
//     return 0;
// }