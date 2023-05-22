#include "types.h"
#include "loongarch.h"
#include "memlayout.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

volatile static int started = 0;

struct spinlock spinlock;
int main()
{
	if(cpuid() == 0){
	consoleinit();
	printfinit();
	printf("cpu%d: xv6 booting... \n", cpuid());
	kinit();
	vminit();		// used for user process.
	procinit();
	trapinit();
	trapinithart();
	binit();
	iinit();
	fileinit();
	ramdiskinit();
	userinit();
	__sync_synchronize();
	started = 1;
	} else {
		while (started == 0)
			;
		printf("cpu%d starting\n", cpuid());
		trapinithart();
	}
	scheduler();
	return 0;
}
