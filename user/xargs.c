#include "kernel/types.h"
#include "user/user.h"

#define MAX_XARGS 10 // Max arguments from stdin
#define MAX_ARG_LEN 20 // Max length of a single argument from stdin

int main(int argc, char *argv[]) { // Use standard main signature
    if (argc <= 1) {
        fprintf(2, "usage: xargs <command> [initial_args...]\n");
        exit(1);
    }

    char buf[512];
    int n = read(0, buf, sizeof(buf) - 1); // Read at most 511 bytes to guarantee space for null
    if (n < 0) {
        fprintf(2, "xargs: read error\n");
        exit(1);
    }
    buf[n] = 0; // Null-terminate the buffer content

    // Use defines for clarity and safety
    char xargv[MAX_XARGS][MAX_ARG_LEN]; 
    int xargc = 0;
    char *p = buf;
    char *word_start = p;

    // Parse arguments from buffer
    while (p < buf + n + 1) { // Iterate safely up to the null terminator
        if (*p == ' ' || *p == '\n' || *p == '\t' || *p == '\0') { // Handle more whitespace and end
            if (p > word_start) { // If we have a non-empty word
                uint len = p - word_start;
                
                // Bounds check: Argument count
                if (xargc >= MAX_XARGS) {
                    fprintf(2, "xargs: too many arguments from input\n");
                    // Decide whether to execute with current args or exit
                    // For simplicity, we'll exit here. A real xargs might execute.
                    exit(1); 
                }
                
                // Bounds check: Argument length
                if (len >= MAX_ARG_LEN) {
                    fprintf(2, "xargs: argument too long\n");
                    exit(1);
                }

                memcpy(xargv[xargc], word_start, len);
                xargv[xargc][len] = 0; // Null-terminate the copied argument
                xargc++;
            }
            if (*p == '\0') { // Stop if we hit the end of the buffer
                break;
            }
            word_start = p + 1; // Start next word after the delimiter
        }
        p++;
    }

    // Debug print parsed args
    // printf("Parsed %d args from stdin: ", xargc);
    // for (int i = 0; i < xargc; i++) {
    //     printf("'%s' ", xargv[i]);
    // }
    // printf("\n");

    // Allocate space for: command + initial args (argc-1) + stdin args (xargc) + NULL terminator (1)
    int final_argc = (argc - 1) + xargc;
    char *final_argv[final_argc + 1]; // Need final_argc + 1 slots total
    
    // Copy command and initial arguments
    for (int i = 1; i < argc; i++) {
        final_argv[i - 1] = argv[i];
    }
    
    // Copy arguments parsed from stdin
    for (int i = 0; i < xargc; i++) {
        final_argv[argc - 1 + i] = xargv[i]; // Correct index
    }

    // Add the NULL terminator at the correct position
    final_argv[final_argc] = 0; // Index is final_argc (which is argc - 1 + xargc)
    
    // Debug print final args
    // printf("Final command: ");
    // for (int i = 0; i < final_argc; i++) {
    //     printf("%s ", final_argv[i]);
    // }
    // printf("\n");

    // Execute the command
    exec(final_argv[0], final_argv);

    // exec only returns on error
    fprintf(2, "xargs: exec %s failed\n", final_argv[0]);
    exit(1); 
}