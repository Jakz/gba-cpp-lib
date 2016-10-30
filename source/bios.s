.text
.code 16

.align 4
.global bios_obj_affine_set
bios_obj_affine_set:
mov r2, #1
mov r3, #2
swi #0x0F
bx lr

.code 32
.global bios_cpu_fast_set
bios_cpu_fast_set:
ldr r4, =#0x000FFFFF
and r2, r2, r4
orr r2, r2, r3, lsl #24
swi #0x0C
bx lr