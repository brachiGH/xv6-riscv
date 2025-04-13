/*

primes (moderate)/(hard)

Write a concurrent version of prime sieve using pipes. This idea is due to Doug McIlroy, inventor of Unix pipes. The picture halfway down this page and the surrounding text explain how to do it. Your solution should be in the file user/primes.c.

Your goal is to use pipe and fork to set up the pipeline.
The first process feeds the numbers 2 through 35 into the pipeline.
For each prime number, you will arrange to create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.

Some hints:

    Be careful to close file descriptors that a process doesn't need, because otherwise your program will run xv6 out of resources before the first process reaches 35.
    Once the first process reaches 35, it should wait until the entire pipeline terminates, including all children, grandchildren, &c. Thus the main primes process should only exit after all the output has been printed, and after all the other primes processes have exited.
    Hint: read returns zero when the write-side of a pipe is closed.
    It's simplest to directly write 32-bit (4-byte) ints to the pipes, rather than using formatted ASCII I/O.
    You should create the processes in the pipeline only as they are needed.
    Add the program to UPROGS in Makefile. 

Your solution is correct if it implements a pipe-based sieve and produces the following output:

    $ make qemu
    ...
    init: starting sh
    $ primes
    prime 2
    prime 3
    prime 5
    prime 7
    prime 11
    prime 13
    prime 17
    prime 19
    prime 23
    prime 29
    prime 31
    $
  

*/
/*
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXIMUM 35
#define MAX_FILE_DISCRIPTORS 35
int nums[MAXIMUM + 1];
int pipefd[MAXIMUM + 1][2];

void write_multiples(int a,int fd) {
    int i = 2;
    uint8_t s;
    while (i * a <= MAXIMUM) {
        s = i*a;
        write(fd, &s, 1);
        i++;
    }
    close(fd);
}

void read_bufs(int pipes_to_flush) {
    for (int i = 0; i <= pipes_to_flush; i++) {
        if (pipefd[i][0] == 0) {
            continue;
        }

        uint8_t noneprime[MAXIMUM];
        int n = read(pipefd[i][0], noneprime, MAXIMUM);
        for (int j = 0; j < n; j++) {
            nums[noneprime[j]] = -1;
        }
        close(pipefd[i][0]);
        pipefd[i][0] = 0;
    }
}

int main() {
    for (int i = 2; i <= MAXIMUM; i++) {
        if (nums[i] != -1) {
            int d = pipe(pipefd[i]);
            if (pipefd[i][1] > MAX_FILE_DISCRIPTORS) {
                read_bufs(i-1);
            }

            if (fork() == 0) {
                close(pipefd[i][0]);
                write_multiples(i, pipefd[i][1]);
                exit(0);
            }
            close(pipefd[i][1]);
        }
    }

    read_bufs(MAXIMUM);

    for(int i = 2; i <= MAXIMUM; i++) {
        if (nums[i] != -1) {
            printf("%d\n", i);
        }
    }
}

*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> // Include for wait()

#define MAXIMUM 35

// Function representing one stage of the sieve pipeline
// Reads numbers from left_fd, filters multiples of its prime 'p',
// and writes remaining numbers to a pipe connected to the next stage.
void sieve_stage(int left_fd) {
    int p; // The prime number for this stage
    int n; // Numbers read from the left pipe

    // Read the first number from the left pipe. This is our prime.
    if (read(left_fd, &p, sizeof(p)) <= 0) {
        // If read fails or pipe is empty/closed, exit.
        close(left_fd);
        exit(0);
    }

    // Print the prime number found by this stage
    printf("prime %d\n", p);

    // Create a pipe for the next stage (right pipe)
    int right_pipe[2];
    if (pipe(right_pipe) < 0) {
        perror("pipe");
        close(left_fd); // Close the input pipe before exiting
        exit(1);
    }

    // Fork a child process for the next stage of the sieve
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        close(left_fd);
        close(right_pipe[0]);
        close(right_pipe[1]);
        exit(1);
    }

    if (pid == 0) { // Child process (next sieve stage)
        close(right_pipe[1]); // Child doesn't write to the right pipe
        close(left_fd);       // Child doesn't read from the left pipe
        // Recursively call sieve_stage with the read end of the right pipe
        sieve_stage(right_pipe[0]);
        // The child exits within sieve_stage or here if read fails immediately
        exit(0);
    } else { // Parent process (current sieve stage)
        close(right_pipe[0]); // Parent doesn't read from the right pipe

        // Read subsequent numbers from the left pipe
        while (read(left_fd, &n, sizeof(n)) > 0) {
            // Filter: If n is not a multiple of p, pass it to the right pipe
            if (n % p != 0) {
                if (write(right_pipe[1], &n, sizeof(n)) < 0) {
                    perror("write to right pipe");
                    // Error writing, clean up and exit
                    close(left_fd);
                    close(right_pipe[1]);
                    wait(NULL); // Wait for child before exiting
                    exit(1);
                }
            }
        }

        // Close pipes when done reading/writing
        close(left_fd);
        close(right_pipe[1]);

        // Wait for the child process (next stage) to complete
        wait(NULL);
        exit(0); // Exit the current stage
    }
}

int main() {
    int initial_pipe[2];

    // Create the initial pipe
    if (pipe(initial_pipe) < 0) {
        perror("initial pipe");
        exit(1);
    }

    // Fork the first stage of the sieve
    int pid = fork();
    if (pid < 0) {
        perror("initial fork");
        exit(1);
    }

    if (pid == 0) { // First child process (start of the pipeline)
        close(initial_pipe[1]); // Child reads from initial pipe, doesn't write
        sieve_stage(initial_pipe[0]);
        exit(0); // Should exit within sieve_stage
    } else { // Parent process (feeds numbers into the pipeline)
        close(initial_pipe[0]); // Parent writes to initial pipe, doesn't read

        // Feed numbers 2 through MAXIMUM into the initial pipe
        for (int i = 2; i <= MAXIMUM; i++) {
            if (write(initial_pipe[1], &i, sizeof(i)) < 0) {
                perror("write to initial pipe");
                // Error writing, clean up and exit
                close(initial_pipe[1]);
                wait(NULL); // Wait for child before exiting
                exit(1);
            }
        }

        // Close the write end of the initial pipe to signal EOF to the first stage
        close(initial_pipe[1]);

        // Wait for the first child process (and subsequently all grandchildren) to finish
        wait(NULL);
        exit(0); // Main process exits successfully
    }
}