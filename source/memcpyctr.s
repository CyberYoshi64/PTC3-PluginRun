.arm
.fpu vfp
.align(4);
.section .text

.global __ctr_memcpy_cache
.type __ctr_memcpy_cache, %function
__ctr_memcpy_cache:
	STMFD           SP!, {R4-R10,LR}
    SUBS            R2, R2, #0x20
    BCC             __ctr_memcpy_cache_exit
    LDMIA           R1!, {R3-R6}
    PLD             [R1,#0x40]
    LDMIA           R1!, {R7-R10}

	__ctr_memcpy_cache_loop:
		STMIA           R0!, {R3-R6}
		SUBS            R2, R2, #0x20
		STMIA           R0!, {R7-R10}
		LDMCSIA         R1!, {R3-R6}
		PLD             [R1,#0x40]
		LDMCSIA         R1!, {R7-R10}
		BCS             __ctr_memcpy_cache_loop

	__ctr_memcpy_cache_exit:
		MOVS            R12, R2,LSL#28
		LDMCSIA         R1!, {R3,R4,R12,LR}
		STMCSIA         R0!, {R3,R4,R12,LR}
		LDMMIIA         R1!, {R3,R4}
		STMMIIA         R0!, {R3,R4}
		LDMFD           SP!, {R4-R10,LR}
		MOVS            R12, R2,LSL#30
		LDRCS           R3, [R1],#4
		STRCS           R3, [R0],#4
		BXEQ            LR
		MOVS            R2, R2,LSL#31
		LDRCSH          R3, [R1],#2
		LDRMIB          R2, [R1],#1
		STRCSH          R3, [R0],#2
		STRMIB          R2, [R0],#1
		BX              LR

.global __ctr_memcpy
.type __ctr_memcpy, %function
__ctr_memcpy:
	CMP             R2, #3
    BLS             __byte_copy
    ANDS            R12, R0, #3
    BEQ             __ctr_memcpy_w
    LDRB            R3, [R1],#1
    CMP             R12, #2
    ADD             R2, R2, R12
    LDRLSB          R12, [R1],#1
    STRB            R3, [R0],#1
    LDRCCB          R3, [R1],#1
    STRLSB          R12, [R0],#1
    SUB             R2, R2, #4
    STRCCB          R3, [R0],#1

	__ctr_memcpy_w:
		ANDS            R3, R1, #3
		BEQ             __ctr_memcpy_cache
		SUBS            R2, R2, #8

	__u32_copy:                        
		BCC             __u32_copy_last
		LDR             R3, [R1],#4
		SUBS            R2, R2, #8
		LDR             R12, [R1],#4
		STMIA           R0!, {R3,R12}
		B               __u32_copy

	__u32_copy_last:                            
		ADDS            R2, R2, #4
		LDRPL           R3, [R1],#4
		STRPL           R3, [R0],#4
		NOP

	__byte_copy:
		MOVS            R2, R2,LSL#31
		LDRCSB          R3, [R1],#1
		LDRCSB          R12, [R1],#1
		LDRMIB          R2, [R1],#1
		STRCSB          R3, [R0],#1
		STRCSB          R12, [R0],#1
		STRMIB          R2, [R0],#1
		BX              LR


.global memcpy_ctr
.type memcpy_ctr, %function
memcpy_ctr:
	STMFD           SP!, {R0,LR}
	BL              __ctr_memcpy
	LDMFD           SP!, {R0,PC}