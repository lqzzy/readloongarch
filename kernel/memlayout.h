#ifdef __ASSEMBLY__
#define _CONST64_(x)    x
#else
#define _CONST64_(x)    x ## L
#endif

#define DMW_PABITS	48

#define CSR_DMW1_PLV0	_CONST64_(1 << 0)
#define CSR_DMW1_MAT	_CONST64_(1 << 4)
#define CSR_DMW1_VSEG	_CONST64_(0x9000)
#define CSR_DMW1_BASE	(CSR_DMW1_VSEG << DMW_PABITS)
#define CSR_DMW1_INIT	(CSR_DMW1_BASE | CSR_DMW1_MAT | CSR_DMW1_PLV0)


#define PHYSTOP		0x98000000L
#define MAX_USED_PHYSADDR	0x100000000
// only use CSR.PGDL
#define MAXVA 		(1L << (8 + 9 + 9 + 9 + 12))

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAPFRAME	(MAXVA - PGSIZE)
#define USTACK		(MAXVA - PGSIZE)
#define USTACK_BASE	(USTACK - PGSIZE)

// USTACK - PGSIZE is userstack's guard page
#ifdef LAB_PGTBL
#define USYSCALL	(USTACK - 3*PGSIZE)
#endif

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) 	(TRAMPOLINE - ((p)+1)* 2*PGSIZE)

#define KERNBASE	(0x9000000000000000)
#define V2P(v)		(v-KERNBASE)
#define P2V(p)		(p+KERNBASE)

// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 02000000 -- CLINT
// 0C000000 -- PLIC
// 10000000 -- uart0 
// 10001000 -- virtio disk 
// 80000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 80000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// qemu puts UART registers here in physical memory.
//#define UART0 0x10000000L
#define UART0_IRQ 10

// virtio mmio interface
#define VIRTIO0 0x10001000
#define VIRTIO0_IRQ 1

// core local interruptor (CLINT), which contains the timer.
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot.

// qemu puts platform-level interrupt controller (PLIC) here.
#define PLIC 0x0c000000L
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.

#define INT		0x10000000
#define	INT_INT_MASK_L	(INT + 0x020)	
#define	INT_INT_MASK_H	(INT + 0x020)	

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
