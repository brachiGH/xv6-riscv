#include "kernel/types.h"
#include "kernel/syscall.h"
#include "user/user.h"

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(2, "trace: unvalid number of arguments.\n");
        fprintf(2, "usage: trace [syscal mask] [executable]");
        exit(1);
    }


    int mask = atoi(argv[1]);
    trace(mask);

    exec(argv[2], argv+2);

    fprintf(2, "trace: exec %s failed\n", argv[2]);
    return 1;
}