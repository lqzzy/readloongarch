#include "types.h"
#include "loongarch.h"
#include "defs.h"
#include "spinlock.h"
#include "param.h"
#include "proc.h"


/*
static unsigned long csrstack[NCPU][10];
int top[NCPU] = {0};

#define push(x)	(csrstack[cpuid()][top[cpuid()]++] = x)
#define pop()	(csrstack[cpuid()][--top[cpuid()]])
*/

static inline int cmpxch(int *m, int old, int new)
{
	int ret;

	asm volatile (
	"1:	ll.w	%0, %2		\n"
	"	bne	%0, %z3, 2f	\n"
	"	or	$t0, %z4, $zero	\n"
	"	sc.w	$t0, %1		\n"
	"	beq	$zero, $t0, 1b	\n"
	"2:				\n"
	"	dbar	0		\n"
	: "=&r" (ret), "=ZB" (*m)
	: "ZB"  (*m), "Jr" (old), "Jr" (new)
	: "t0", "memory");

	return ret;
}

int holding(struct spinlock *lk)
{
	int r;
	r = (lk->locked && lk->cpu == mycpu());
	return r;
}

void initlock(struct spinlock *sl, char *name)
{
	sl->locked = 0;
	sl->cpu = mycpu();
	sl->name = name;
}

static inline void spin_lock(struct spinlock *spinlock)
{
	if (holding(spinlock))
		panic("spin_lock: already hold\n");

	while (cmpxch(&spinlock->locked, 0, 1) != 0)
		;

	__sync_synchronize();

	spinlock->cpu = mycpu();
}

static inline void spin_unlock(struct spinlock *spinlock)
{
	if (!holding(spinlock))
		panic("spin_unlock: not holding\n");

	__sync_synchronize();

	cmpxch(&spinlock->locked, 1, 0);
}

void acquire(struct spinlock *spinlock)
{
	push_off();
	spin_lock(spinlock);
}

void release(struct spinlock *spinlock)
{

	spin_unlock(spinlock);
	pop_off();
}

void push_off(void)
{
	int old = intr_get();

	intr_off();

	if(mycpu()->noff == 0)
		mycpu()->intena = old;
	mycpu()->noff += 1;
}

void pop_off(void)
{
	struct cpu *c = mycpu();
	if(intr_get())
		panic("pop_off - interruptible");
	if(c->noff < 1)
		panic("pop_off");
	c->noff -= 1;
	if(c->noff == 0 && c->intena)
		intr_on();
}
