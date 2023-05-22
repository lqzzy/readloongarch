	#
        # code to switch between user and kernel space.
        #
        # this code is mapped at the same virtual address
        # (TRAMPOLINE) in user and kernel space so that
        # it continues to work when it switches page tables.
	#
	# kernel.ld.d causes this to be aligned
        # to a page boundary.
        #
#define TRAPFRAME 	0x7FFFFFFFF000
	.section trampsec
.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:    
	#
        # trap.c sets eentry to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        
	# SAVE0
	csrwr	$a0, 0x30
	csrrd	$a0, 0x5
	csrwr	$a0, 0x31
	li.d	$a0, TRAPFRAME

        # save the user registers in TRAPFRAME
        st.d	$ra, $a0, 40
        st.d	$tp, $a0, 48
        st.d	$sp, $a0, 56
        st.d	$a1, $a0, 72
        st.d	$a2, $a0, 80
        st.d	$a3, $a0, 88
        st.d	$a4, $a0, 96
        st.d	$a5, $a0, 104
        st.d	$a6, $a0, 112
        st.d	$a7, $a0, 120
        st.d	$t0, $a0, 128
        st.d	$t1, $a0, 136
        st.d	$t2, $a0, 144
        st.d	$t3, $a0, 152
        st.d	$t4, $a0, 160
        st.d	$t5, $a0, 168
        st.d	$t6, $a0, 176
        st.d	$t7, $a0, 184
        st.d	$t8, $a0, 192
        st.d	$r21, $a0, 200
        st.d	$fp, $a0, 208
        st.d	$s0, $a0, 216
        st.d	$s1, $a0, 224
        st.d	$s2, $a0, 232
        st.d	$s3, $a0, 240
        st.d	$s4, $a0, 248
        st.d	$s5, $a0, 256
        st.d	$s6, $a0, 264
        st.d	$s7, $a0, 272
        st.d	$s8, $a0, 280

	# save the user a0 in p->trapframe->a0
        csrrd 	$t0, 0x30
        st.d 	$t0, $a0, 64

        # restore kernel stack pointer from p->trapframe->kernel_sp
        ld.d	$sp, $a0, 8

        # make tp hold.d the current hartid, from p->trapframe->kernel_hartid
        ld.d	$tp, $a0, 32

        # load the address of usertrap(), p->trapframe->kernel_trap
        ld.d	$t0, $a0, 16

# restore kernel page table from p->trapframe->kernel_satp
#ld.d	$t1, $a0, 0
#csrwr	$t1, 0x19
#invtlb	0, $zero, $zero

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->tf.

        # jump to usertrap(), which does not return
	jirl	$zero, $t0, 0

.globl userret
userret:
        # userret(TRAPFRAME, pagetable)
        # switch from kernel to user.
        # usertrapret() calls here.
        # a0: TRAPFRAME, in user page table.
        # a1: user page table, for satp.

        # switch to the user page table.
	csrwr	$a1, 0x19
	invtlb	0, $zero, $zero

        # restore all but a0 from TRAPFRAME
        ld.d	$ra, $a0, 40
        ld.d	$tp, $a0, 48
        ld.d	$sp, $a0, 56
        ld.d	$a1, $a0, 72
        ld.d	$a2, $a0, 80
        ld.d	$a3, $a0, 88
        ld.d	$a4, $a0, 96
        ld.d	$a5, $a0, 104
        ld.d	$a6, $a0, 112
        ld.d	$a7, $a0, 120
        ld.d	$t0, $a0, 128
        ld.d	$t1, $a0, 136
        ld.d	$t2, $a0, 144
        ld.d	$t3, $a0, 152
        ld.d	$t4, $a0, 160
        ld.d	$t5, $a0, 168
        ld.d	$t6, $a0, 176
        ld.d	$t7, $a0, 184
        ld.d	$t8, $a0, 192
        ld.d	$r21, $a0, 200
        ld.d	$fp, $a0, 208
        ld.d	$s0, $a0, 216
        ld.d	$s1, $a0, 224
        ld.d	$s2, $a0, 232
        ld.d	$s3, $a0, 240
        ld.d	$s4, $a0, 248
        ld.d	$s5, $a0, 256
        ld.d	$s6, $a0, 264
        ld.d	$s7, $a0, 272
        ld.d	$s8, $a0, 280

	ld.d	$a0, $a0, 64
        
        # return to user mode and user pc.
        # usertrapret() set up prmd and era.
        ertn
