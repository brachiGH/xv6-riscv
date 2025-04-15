// System call numbers
#define SYS_fork       1
#define SYS_exit       2
#define SYS_wait       3
#define SYS_pipe       4
#define SYS_read       5
#define SYS_kill       6
#define SYS_exec       7
#define SYS_fstat      8
#define SYS_chdir      9
#define SYS_dup       10
#define SYS_getpid    11
#define SYS_sbrk      12
#define SYS_sleep     13
#define SYS_uptime    14
#define SYS_open      15
#define SYS_write     16
#define SYS_mknod     17
#define SYS_unlink    18
#define SYS_link      19
#define SYS_mkdir     20
#define SYS_close     21
#define SYS_trace     22
#define SYS_sysinfo   23


#define SYS_name(x) \
    ((x) == SYS_fork      ? "fork"    : \
     (x) == SYS_exit      ? "exit"    : \
     (x) == SYS_wait      ? "wait"    : \
     (x) == SYS_pipe      ? "pipe"    : \
     (x) == SYS_read      ? "read"    : \
     (x) == SYS_kill      ? "kill"    : \
     (x) == SYS_exec      ? "exec"    : \
     (x) == SYS_fstat     ? "fstat"   : \
     (x) == SYS_chdir     ? "chdir"   : \
     (x) == SYS_dup       ? "dup"     : \
     (x) == SYS_getpid    ? "getpid"  : \
     (x) == SYS_sbrk      ? "sbrk"    : \
     (x) == SYS_sleep     ? "sleep"   : \
     (x) == SYS_uptime    ? "uptime"  : \
     (x) == SYS_open      ? "open"    : \
     (x) == SYS_write     ? "write"   : \
     (x) == SYS_mknod     ? "mknod"   : \
     (x) == SYS_unlink    ? "unlink"  : \
     (x) == SYS_link      ? "link"    : \
     (x) == SYS_mkdir     ? "mkdir"   : \
     (x) == SYS_close     ? "close"   : \
     (x) == SYS_trace     ? "trace"   : \
     (x) == SYS_sysinfo   ? "sysinfo" : \
     "unknown")
