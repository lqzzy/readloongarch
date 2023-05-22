#include "types.h"
#include "loongarch.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"
#include "defs.h"

extern int freemem;
extern int nproc;

uint64 trace(int mask)
{
	myproc()->trace_mask = mask;

	return 0;
}

uint64 sysinfo(struct sysinfo *usysinfo)
{
	struct sysinfo sysinfo = {
		.freemem = freemem,
		.nproc = nproc,
	};
	return copyout(myproc()->pagetable, (uint64)usysinfo, 
			(char*)&sysinfo, sizeof(struct sysinfo));
}

