	.thumb
	.syntax unified
	.cpu cortex-m3
	.global gc_vectors
	.global ResetISR2
	.global SystemInit
	.thumb_func
	.global ResetHandler
	.type ResetHandler,%function

ResetHandler:
	ldr		r0, =0x20001000
	mov		sp,r0
	ldr		r0, = SystemInit
	blx		r0

	ldr		r0, = gc_vectors
	ldr		r0, [r0]
	mov		sp,r0
	ldr		r0,=ResetISR2
	bx		r0




