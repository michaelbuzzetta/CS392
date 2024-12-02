#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


void signal_handler(int signum) 
{
    printf("Signal %d received\n", signum);
    exit(EXIT_SUCCESS);
}

int main() {
    FILE *file;
    char buffer[100];

    // Writing to a file
    file = fopen("example.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "Hello, File I/O!\n");
    fclose(file);

    // Reading from a file
    file = fopen("example.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fgets(buffer, sizeof(buffer), file);
    printf("Contents of file: %s", buffer);
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    fclose(file);

    FILE *file1;
    
    // Writing to a file
    file = fopen("buffer_example.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    fprintf(file, "Line 1\n");
    fprintf(file, "Line 2\n");
    fprintf(file, "Line 3\n");
    fclose(file);

    int file_descriptor;
    char buffer1[100];

    // Open a file
    file_descriptor = open("example.txt", O_RDONLY);
    if (file_descriptor == -1) {
        perror("open");
        return 1;
    }

    // Read from the file
    if (read(file_descriptor, buffer1, sizeof(buffer1)) == -1) {
        perror("read");
        return 1;
    }

    // Print the content read from the file
    printf("Content read from file: %s\n", buffer1);
    printf("\n");
    printf("\n");
    printf("\n");


    // Close the file
    close(file_descriptor);

    pid_t pid;

    // Create a new process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        // Child process
        printf("Child process: PID = %d\n", getpid());
        execlp("/bin/ls", "ls", NULL);
    } else {
        // Parent process
        printf("Parent process: PID = %d\n", getpid());
    }

    signal(SIGINT, signal_handler);

    printf("Waiting for SIGINT (Ctrl+C)...\n");
    while (1) 
    {
        sleep(1);
    }


    return 0;
}
