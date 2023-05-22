#define CRMD_IE			(1UL << 2)

#define PGSIZE			4096
#define PGSHIFT			12

#define PTE_V			(1L << 0)
#define PTE_R			(0)
#define PTE_X			(0)
#define PTE_W			(1L << 8)
#define PTE_P			(1L << 7)
#define PTE_U			(0x03L << 2)
#define PTE_MAT_CC		(0x01L << 4)

#define PGROUNDUP(sz)		(((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a)		(((a)) & ~(PGSIZE-1))

#define PA2PTE(pa)		(((uint64)pa) & 0x0fffffffff000UL)	

#define PTE2PA(pte)		((pte) & 0x0fffffffff000UL)

#define PTE_FLAGS(pte)		((pte) & 0xe0000000000001ff)

#define PXMASK			0x1ff
#define PXSHIFT(level)		(PGSHIFT + (9*(level)))
#define PX(level, va)		((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

#define PRMD_PPIE		(0x4)
#define PRMD_PPLV_BITS_NUM	2
#define PRMD_PPLV_SHIFT		0
#define PRMD_PPLV_BIT_MASK	(((1 << PRMD_PPLV_BITS_NUM)-1) << PRMD_PPLV_SHIFT)
#define PRMD_PPLV(x)		((x) & PRMD_PPLV_BIT_MASK)

#define ECODE_SYS		0xb
#define ESTAT_ECODE_BITS_NUM	6
#define ESTAT_ECODE_SHIFT	16
#define ESTAT_ECODE_BIT_MASK	(((1 << ESTAT_ECODE_BITS_NUM)-1) << ESTAT_ECODE_SHIFT)
#define ESTAT_ECODE(x)		((x) >> 16)

#define ESTAT_HWI0		(0x4)
#define ESTAT_TIMER		(1 << 11)

typedef uint64 pte_t;
typedef uint64 *pagetable_t;

static inline void test()
{
	asm volatile (
	"	li.d	$t1, 0xffffffffffffffff\n"
	"	li.d	$t0, 0x1600\n"
	"	iocsrwr.d $t1, $t0\n"
	"	li.d	$t0, 0x1608\n"
	"	iocsrwr.d $t1, $t0\n"
	"	li.d	$t0, 0x1610\n"
	"	iocsrwr.d $t1, $t0\n"
	"	li.d	$t0, 0x1618\n"
	"	iocsrwr.d $t1, $t0\n"
	"	li.d	$t0, 0x14c0\n"
	"	li.d	$t1, 0x1\n"
	"	iocsrwr.b $t1, $t0\n"
	"	iocsrrd.b $t1, $t0\n"
	"	li.d	$t1, 0x140a\n"
	"	li.d	$t0, 1	\n"
	"	iocsrwr.b $t0, $t1\n"
	"	li.d	$t1, 0x1c08\n"
	"	iocsrwr.b $t0, $t1\n"
	);
}

static inline void _test()
{
	asm volatile (
	"	csrrd $t0, 0x19\n"
	"	csrrd $t0, 0x1c\n"
	"	csrrd $t0, 0x1d\n"
	"	csrrd $t0, 0xc\n"
	"	csrrd $t0, 0x420\n"
	"	csrrd $t0, 0x5\n"
	"	csrrd $t0, 0x4\n"
	"	li.d	$t1, 0xffff\n"
	"	li.d	$t0, 0x1600\n"
	"	iocsrwr.d $t1, $t0\n"
	"	li.d	$t0, 0x14c0\n"
	"	li.d	$t1, 0x1\n"
	"	iocsrwr.b $t1, $t0\n"
	"	iocsrrd.b $t1, $t0\n"
	"	li.d	$t1, 0x140a\n"
	"	li.d	$t0, 1	\n"
	"	iocsrwr.b $t0, $t1\n"
	"	li.d	$t1, 0x1c08\n"
	"	iocsrwr.b $t0, $t1\n"
	);
}

static inline int cpuid()
{
	int ret;
	
	asm volatile (
	"	csrrd	%0, 0x20	\n"
	"	andi	%0, %0, 0x3ff	\n"
	: "=&r" (ret));

	return ret;
}

static inline void csrwr_ecfg(uint64 x)
{
	asm volatile ("csrwr %z0, 0x4" :: "Jr" (x));
}

static inline void csrwr_tcfg(uint64 x)
{
	asm volatile ("csrwr %z0, 0x41" :: "Jr" (x));
}

static inline void csrwr_eentry(uint64 x)
{
	asm volatile ("csrwr %z0, 0xc" :: "Jr" (x));
}

static inline void csrwr_pgdl(uint64 x)
{
	asm volatile ("csrwr %z0, 0x19" :: "Jr" (x));
}

static inline uint64 csrrd_pgdl(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 0x19" : "=&r" (ret));

	return ret;
}

static inline uint64 csrrd_save1(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 0x31" : "=&r" (ret));

	return ret;
}

static inline uint64 r_sp(void)
{
	uint64 ret;

	asm volatile ("ori %0, $sp, 0" : "=&r" (ret));

	return ret;
}

static inline void vminit(void)
{
	asm volatile (
	"	li.d	$t0, 0x13e4d52c		\n"
	"	csrwr	$t0, 0x1c		\n"
	"	li.d	$t0, 0x227		\n"
	"	csrwr	$t0, 0x1d		\n"
	"	la.abs	$t0, tlbrentry		\n"
	"	csrwr	$t0, 0x88		\n"
	:::"t0");
}

static inline void csrwr_ticlr(uint64 x)
{
	asm volatile ("csrwr %z0, 0x44" :: "Jr" (x));
}

static inline uint64 csrrd_prmd(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 1" : "=&r" (ret));

	return ret;
}

static inline void csrwr_prmd(uint64 x)
{
	asm volatile ("csrwr %z0, 1" :: "Jr" (x));
}

static inline uint64 csrrd_era(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 6" : "=&r" (ret));

	return ret;
}

static inline void csrwr_era(uint64 x)
{
	asm volatile ("csrwr %z0, 0x6" :: "Jr" (x));
}

static inline uint64 csrrd_estat(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 5" : "=&r" (ret));

	return ret;
}

static inline void csrwr_estat(uint64 x)
{
	asm volatile ("csrwr %z0, 0x5" :: "Jr" (x));
}

static inline uint64 csrrd_crmd(void)
{
	uint64 ret;

	asm volatile ("csrrd %0, 0" : "=&r" (ret));

	return ret;
}

static inline void csrwr_crmd(uint64 x)
{
	asm volatile ("csrwr %z0, 0" :: "Jr" (x));
}

static inline void intr_on(void)
{
	csrwr_crmd(csrrd_crmd() | CRMD_IE);
}

static inline void intr_off(void)
{
	csrwr_crmd(csrrd_crmd() &~ CRMD_IE);
}

static inline int intr_get()
{
	return csrrd_crmd() & CRMD_IE;
}

static inline void iocsrwr_Intisr(uint64 x)
{
	asm volatile (
	"li.d	$t1, 0x1420\n"
	"iocsrwr.w %z0, $t1" 
	:: "Jr" (x));
}

static inline uint64 iocsrrd_Intisr(void)
{
	uint64 ret;

	asm volatile (
	"li.d   $t1, 0x1420\n"
	"iocsrrd.w %0, $t1"
	: "=&r" (ret));

	return ret;
}

static inline void iocsrwr_Inten(uint64 x)
{
	asm volatile (
	"li.d	$t1, 0x1424\n"
	"iocsrwr.w %z0, $t1" 
	:: "Jr" (x));
}

static inline uint64 iocsrrd_Inten(void)
{
	uint64 ret;

	asm volatile (
	"li.d   $t1, 0x1424\n"
	"iocsrrd.w %0, $t1"
	: "=&r" (ret));

	return ret;
}

static inline void iocsrwr_Intenset(uint64 x)
{
	asm volatile (
	"li.d	$t1, 0x1428\n"
	"iocsrwr.w %z0, $t1" 
	:: "Jr" (x));
}

static inline uint64 iocsrrd_Intenset(void)
{
	uint64 ret;

	asm volatile (
	"li.d   $t1, 0x1428\n"
	"iocsrrd.w %0, $t1"
	: "=&r" (ret));

	return ret;
}

static inline void iocsrwr_Intenclr(uint64 x)
{
	asm volatile (
	"li.d	$t1, 0x142c\n"
	"iocsrwr.w %z0, $t1" 
	:: "Jr" (x));
}

static inline uint64 iocsrrd_Intenclr(void)
{
	uint64 ret;

	asm volatile (
	"li.d   $t1, 0x142c\n"
	"iocsrrd.w %0, $t1"
	: "=&r" (ret));

	return ret;
}

static inline void iocsrwr_Intedge(uint64 x)
{
	asm volatile (
	"li.d	$t1, 0x1434\n"
	"iocsrwr.w %z0, $t1" 
	:: "Jr" (x));
}

static inline uint64 iocsrrd_Intedge(void)
{
	uint64 ret;

	asm volatile (
	"li.d   $t1, 0x1434\n"
	"iocsrrd.w %0, $t1"
	: "=&r" (ret));

	return ret;
}


/*
static inline void push(uint64 x)
{
	asm volatile (
	"	addi.d	$sp, $sp, -8	\n"
	"	st.d	%z0, $sp, 0	\n"
	:: "Jr" (x));
}

static inline uint64 pop()
{
	uint64 ret;

	asm volatile (
	"	ld.d	%0, $sp, 0	\n"
	"	addi.d	$sp, $sp, 8	\n"
	: "=&r" (ret));

	return ret;
}
*/
