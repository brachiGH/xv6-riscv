
#include "types.h"
#include "param.h"
#include "sysinfo.h"
#include "riscv.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct sysinfo info;


void init_sysinfo()
{
    info.freemem = 4096;
}

struct sysinfo* current_sysinfo(void)
{
    return &info;
}

uint64 sys_sysinfo(void)
{
    uint64 addr;

    argaddr(0, &addr);

    struct proc *p = myproc();

    if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}