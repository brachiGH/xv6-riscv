/*
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {
    int pipefd1[2];
    int d1 = pipe(pipefd1);
    int pipefd2[2];
    int d2 = pipe(pipefd2);
    if (d1 < 0 || d2 < 0) {
        exit(0);
    }

    int pid1 = fork();
    if (pid1 == 0) {
        close(pipefd1[0]);
        close(pipefd2[1]);

        write(pipefd1[1], "i", 1);

        char r[2];
        read(pipefd2[0], r, 1);
        if (strcmp(r, "o") == 0) {
            printf("%d: received pong\n", getpid());
        }

        exit(0);
    }

    close(pipefd1[1]);
    close(pipefd2[0]);

    char r[2];
    read(pipefd1[0], r, 1);
    if (strcmp(r, "i") == 0) {
        printf("%d: received ping\n", getpid());
    }

    write(pipefd2[1], "o", 1);

    exit(0);
}
*/

/* chat improvements */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd1[2]; // Pipe for parent to child communication
    int pipefd2[2]; // Pipe for child to parent communication

    // Create pipes
    if (pipe(pipefd1) < 0 || pipe(pipefd2) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(pipefd1[0]); // Close unused read end of pipefd1
        close(pipefd2[1]); // Close unused write end of pipefd2

        // Write "i" to parent
        write(pipefd1[1], "i", 1);

        // Read "o" from parent
        char r;
        read(pipefd2[0], &r, 1);
        if (r == 'o') {
            printf("%d: received pong\n", getpid());
        }

        // Close used file descriptors
        close(pipefd1[1]);
        close(pipefd2[0]);
        exit(0);
    }

    // Parent process
    close(pipefd1[1]); // Close unused write end of pipefd1
    close(pipefd2[0]); // Close unused read end of pipefd2

    // Read "i" from child
    char r;
    read(pipefd1[0], &r, 1);
    if (r == 'i') {
        printf("%d: received ping\n", getpid());
    }

    // Write "o" to child
    write(pipefd2[1], "o", 1);

    // Close used file descriptors
    close(pipefd1[0]);
    close(pipefd2[1]);

    // Wait for child process to finish
    wait(NULL);

    return 0;
}