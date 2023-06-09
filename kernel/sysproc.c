#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 first_addr;
  int number_to_take;
  uint64 user_addr_record;
  
  if(argint(1, &number_to_take) < 0)
    return -1;
  if(argaddr(0, &first_addr) < 0)
    return -1;
  if(argaddr(2, &user_addr_record) < 0)
    return -1;

  unsigned int ret_bits = 0;
  pagetable_t now_prco_pgtbl = myproc()->pagetable;

  for(int i=0;i<number_to_take;i++,first_addr+=PGSIZE)
    ret_bits |= ((unsigned int)walk4access(now_prco_pgtbl, first_addr))<<i;
  
  copyout(now_prco_pgtbl, user_addr_record, (char *)&ret_bits, 4);
  return 0;
}
#endif