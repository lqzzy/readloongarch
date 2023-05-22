#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "loongarch.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char uservec[], userret[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  csrwr_ecfg((uint64)(((1UL << 13)-1)));
  csrwr_tcfg((uint64)0x00800003);
  csrwr_ticlr(1);
  csrwr_eentry((uint64)kernelvec);
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  if(PRMD_PPLV(csrrd_prmd()) != 3)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  csrwr_eentry((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = csrrd_era();
  
  // estat stored in save1, /* uservec */
  // because tlbrentry changed estat.
  if(ESTAT_ECODE(csrrd_save1()) == ECODE_SYS){
    // system call

    if(p->killed)
      exit(-1);

    // era points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change prmd &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected estat %p pid=%d\n", csrrd_save1(), p->pid);
    printf("            era=%p stval=%p\n", csrrd_era(), ESTAT_ECODE(csrrd_save1()));
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}

//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to trampoline.S
  csrwr_eentry((uint64)uservec);

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = cpuid();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = csrrd_crmd();
  x |= PRMD_PPLV_BIT_MASK; // set PPLV to 3 for user mode
  x |= PRMD_PPIE; // enable interrupts in user mode
  csrwr_prmd(x);

  // set S Exception Program Counter to the saved user pc.
  csrwr_era(p->trapframe->epc);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with ertn.
  uint64 fn = (uint64)userret;
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, (uint64)p->pagetable);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  int which_dev = 0;
  uint64 era = csrrd_era();
  uint64 prmd = csrrd_prmd();
  uint64 estat = csrrd_save1();
  
  if(PRMD_PPLV(prmd) != 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    printf("estat %p\n", estat);
    printf("era=%p ecode=%p\n", csrrd_era(), ESTAT_ECODE(estat));
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's era instruction.
  csrwr_era(era);
  csrwr_prmd(prmd);
}

void
clockintr()
{
  acquire(&tickslock);
  ticks++;
  wakeup(&ticks);
  release(&tickslock);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 estat = csrrd_save1();

  if(estat & 0x3fc){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    ////////////////////int irq = plic_claim();
    printf("estat: %p\n", estat);
    csrwr_estat(estat &~ 0x3fc);
    int irq = UART0_IRQ;

    if(irq == UART0_IRQ){
      uartintr();
    //} else if(irq == VIRTIO0_IRQ){
    //  virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    /*
    if(irq)
      plic_complete(irq);
    */
    //printf("irq int\n");

    return 1;
  } else if(estat & ESTAT_TIMER){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
      csrwr_ticlr(1);
    }
    
    // acknowledge the software interrupt by clearing
    // the CLR bit in TICLR.
    uartintr();

    return 2;
  } else {
    return 0;
  }
}
