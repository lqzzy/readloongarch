	# qemu -kernel loads the kernel at 0x0000000
        # and causes each CPU to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x9000000000000000.
#define		__ASSEMBLY__
#include "memlayout.h"

.section .text
.global _entry
_entry:
	# set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
	li.w	$t0, 0xa8		# PLV=0, IE=0, PG=0
	csrwr	$t0, 0x0
	li.w	$t0, 0x04		# PLV=0, PIE=1, PWE=0
	csrwr	$t0, 0x1
	li.w	$t0, 0x00		# FPE=0, SXE=0, ASXE=0, BTE=0
	csrwr	$t0, 0x2

	li.d	$t0, CSR_DMW1_INIT
	csrwr	$t0, 0x181
	li.w    $t0, 0x00		# PPLV=0, PPIE=0, PPWE=0
	csrwr	$t0, 0x8b
	li.d	$t0, CSR_DMW1_BASE
	pcaddi	$t1, 0x5		# ------|
	add.d	$t0, $t0, $t1		#       |
	ori	$t0, $t0, 0x01		#	|
	csrwr	$t0, 0x8a		#       |
	ertn				#       |
					#	|
	la.abs	$sp, stack0+4096	# <-----|
	csrrd	$t0, 0x20		# CPUID
	andi	$t0, $t0, 0x3ff
	slli.d	$t0, $t0, 12
	add.d	$sp, $sp, $t0

        bl main				# call main in main.c
spin:
	b spin
.global tlbrentry
tlbrentry:
	csrwr	$t0, 0x8b
	csrrd	$t0, 0x5
	csrrd	$t0, 0x1b
	lddir	$t0, $t0, 0x3
	lddir	$t0, $t0, 0x2
	lddir	$t0, $t0, 0x1
	ldpte	$t0, 0x0
	ldpte 	$t0, 0x1
	tlbfill
	csrrd	$t0, 0x8b
	ertn
