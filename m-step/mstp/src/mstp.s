.syntax unified
.thumb

.section .cacheable_text, "ax"

.global TIM3_IRQHandler
.type TIM3_IRQHandler, %function

.global LABEL1
.global LABEL2
.global LABEL3

.balign (64)
TIM3_IRQHandler:
    //--------------------------------------------------------------------------
    // Get end timestamp
    //--------------------------------------------------------------------------
    // *tim3_CR1 &= ~(1<<0); // EN
    ldr    r3, =tim3_CR1  
    ldr    r3, [r3, #0]   
    mov    r2, #0x80      // ARPE 
    str    r2, [r3, #0]   

    ldr    r3, =tim3_SR  
    ldr    r3, [r3, #0]   
    mov    r2, #0x0      // ARPE 
    str    r2, [r3, #0]   

    //--------------------------------------------------------------------------
    // Save lr for later analysis
    //--------------------------------------------------------------------------
    ldr     r3, =lr_saved
    str     lr, [r3, #0]   

    //--------------------------------------------------------------------------
    // Call the single step function 
    //--------------------------------------------------------------------------
    push    {r7, lr}       
    add     r7, sp, #0     
    bl      single_step   	
    LABEL1:
    pop     {r7, lr}       

    //--------------------------------------------------------------------------
    // Fire BUSted Gadget to amplify the ldr latency
    //--------------------------------------------------------------------------
    // *tim2_CR1 |= (1<<0);
    ldr    r3, =tim2_CR1  
    ldr    r3, [r3, #0]   
    mov    r2, #0x8D      // URS + OPM + + ARPE + CEN
    str    r2, [r3, #0]   
    
    LABEL2:
    // delay to ensure the 1st collision happens in the victim 1st clock
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0
    mov    r0, r0

    LABEL3:
    # //--------------------------------------------------------------------------
    # // Get start timestamp & Fire Next It
    # //--------------------------------------------------------------------------
    // *tim3_CR1 |= (1<<0); // EN
    ldr    r3, =tim3_CR1  
    ldr    r3, [r3, #0]   
    mov    r2, #0x81     // ARPE + CEN 
    str    r2, [r3, #0]   

    // Return to the victim
    bx      lr
