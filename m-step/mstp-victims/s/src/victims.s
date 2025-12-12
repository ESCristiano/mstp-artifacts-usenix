.syntax unified
.thumb

// Define constants for use in the assembly code
.equ REPEAT_COUNT, 5

.global victim_normal_inst
.type victim_normal_inst, %function

victim_normal_inst:
    // Repeat the instruction N times
    .rept REPEAT_COUNT
        mov r0, r0
    .endr

    bx lr

.global victim_ICI_inst
.type victim_ICI_inst, %function

victim_ICI_inst:
    // Repeat the instruction N times
    .rept (REPEAT_COUNT/2)
        push {r0, r1, r2}
        pop {r0, r1, r2}
    .endr

    bx lr

.global victim_IRI_inst
.type victim_IRI_inst, %function

victim_IRI_inst:
    mov r1, #0x1000
    mov r2, #0x7
    // Repeat the instruction N times
    .rept REPEAT_COUNT
        udiv r0, r1, r2
    .endr

    bx lr
