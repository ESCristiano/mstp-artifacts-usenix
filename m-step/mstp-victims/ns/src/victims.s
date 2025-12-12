.syntax unified
.thumb

# .section .cacheable_text, "ax"

// Define constants for use in the assembly code
.equ REPEAT_COUNT, 100

.global victim_normal_inst
.type victim_normal_inst, %function

victim_normal_inst:
    // Repeat the instruction N times
    .rept REPEAT_COUNT
        mov r0, r0
        ldrd r0, r1, [sp]
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
    // Repeat the instruction N times
    .rept REPEAT_COUNT
        ldrd r0, r1, [sp]
    .endr

    bx lr
