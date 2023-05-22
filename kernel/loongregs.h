
#ifdef __ASSEMBLY__
#define _CONST64_(x)    x
#else
#define _CONST64_(x)    x ## L
#endif

#define DMW_PABITS      		48
#define TO_PHYS_MASK    		((1ULL << DMW_PABITS) - 1)

#define CSR_CRMD			0x0
#define CSR_CRMD_IE_SHIFT		2
#define CSR_CRMD_IE			(0x1 << CSR_CRMD_IE_SHIFT)

#define DMW_PABITS      48
/* Direct Map window 0/1 */
#define CSR_DMW0_PLV0			_CONST64_(1 << 0)
#define CSR_DMW0_VSEG			_CONST64_(0x8000)
#define CSR_DMW0_BASE			(CSR_DMW0_VSEG << DMW_PABITS)
#define CSR_DMW0_INIT			(CSR_DMW0_BASE | CSR_DMW0_PLV0)

#define CSR_DMW1_PLV0			_CONST64_(1 << 0)
#define CSR_DMW1_MAT 			_CONST64_(1 << 4)
#define CSR_DMW1_VSEG			_CONST64_(0x9000)
#define CSR_DMW1_BASE			(CSR_DMW1_VSEG << DMW_PABITS)
#define CSR_DMW1_INIT			(CSR_DMW1_BASE | CSR_DMW1_MAT | CSR_DMW1_PLV0)

#define TO_UNCAC(x)             	(CSR_DMW1_BASE | ((x) & TO_PHYS_MASK))
