.section .text
.globl schedule
// Implemented the scheduler in Assembly since the C defined was messing around with the program stacks
//  This way, I can be confident that the stacks will be unchanged
//
// TODO: Mark threads as READY and RUNNING
//
schedule:
	ldr r3, =scheduler
	// r3 = struct Scheduler*
	// Preserve context
	//add r0, r3, #4 // struct cpu_context* ctx
	ldr r0, [r3, #4]
	// r0 = struct cpu_context*
	str r4,  [r0, #0x00]
	str r5,  [r0, #0x04]
	str r6,  [r0, #0x08]
	str r7,  [r0, #0x0c]
	str r8,  [r0, #0x10]
	str r9,  [r0, #0x14]
	str r10, [r0, #0x18]
	str r11, [r0, #0x1c]
	str r12, [r0, #0x20]
	str lr,  [r0, #0x24]
	// Get the next available thread
	push {r3, lr}
	bl get_next_thread
	// r0 = struct LL* next_thread_ll
	pop {r3, lr}
	ldr r1,  [r3, #0] 
	// r1 = struct LL* current_thread_ll
	// Check if there is a valid currently running thread
	cmp r1, #0
	beq schedule.current_thread_nexists
schedule.current_thread_exists:
	cmp r0, r1
	beq schedule.run_current
	// Next is not the same as the current
	// Preserve stack of current
	ldr r2,  [r1, #0x8] // struct Thread* current
	str sp,  [r2, #0x4] // void* stack // Preserve stack
	// Preserve program counter of current
	str lr,  [r2, #0x0] // void* thread // Preserve pc
	ldr r2,  [r0, #0x8] // struct Thread* next
	// Set new stack pointer
	ldr sp,  [r2, #0x4]
	add r2,  r2, #0x18
	// Set new running thread
	str r0,  [r3, #0x0] // struct LL* next_thread_ll // Set new running thread
	// Set new context
	str r2,  [r3, #0x4] // struct cpu_context* ctx // Set new context
	b schedule.run_current
schedule.current_thread_nexists:
	// r0 = struct LL* next_thread_ll
	// r1 = 0 = struct LL* current_thread_ll
	cmp r0, #0
	beq schedule.no_next_thread
	ldr r1, [r0, #0x8]
	// r1 =  struct Thread* next_thread
	// Store system stack pointer
	ldr r2, =syssp
	push {r1}
	ldr r1, [r2]
	cmp r1, #0
	pop {r1}
	bne schedule.dont_overwrite_sys_stack
	// Store if zero system stack
	str sp, [r2]
schedule.dont_overwrite_sys_stack:
	// Move stack to next thread's stack pointer
	ldr sp, [r1, #0x4] // void* stack
	// Store the running thread ll entry
	str r0, [r3, #0x0] // struct LL* rthread_ll
	// Set context
	add r1, r1, #0x18 // struct cpu_context*
	str r1, [r3, #0x4] // store to scheduler.ctx
schedule.run_current:
	// Restore context
	ldr r2,  [r3, #0x4] // struct cpu_context* ctx // Set new context
	ldr r4,  [r2, #0x00]
	ldr r5,  [r2, #0x04]
	ldr r6,  [r2, #0x08]
	ldr r7,  [r2, #0x0c]
	ldr r8,  [r2, #0x10]
	ldr r9,  [r2, #0x14]
	ldr r10, [r2, #0x18]
	ldr r11, [r2, #0x1c]
	ldr r12, [r2, #0x20]
	ldr lr,  [r2, #0x24]
	// Run
	ldr r1, [r3, #0]
	// r1 = struct LL* rthread_ll
	ldr r1, [r1, #0x8]
	// r1 = struct Thread* rthread
	ldr r0, [r1, #0x0]
	// r0 = void* thread
	bx r0
schedule.no_next_thread:
	// r0 = 0 = struct LL* next_thread_ll
	// r1 = 0 = struct LL* current_thread_ll
	// No thread to run
	// Restore sys context
	ldr r0, =syscpu
	str r0, [r3, #0x4] // Store context
	ldr r0, =syssp
	ldr r1, [r0]
	cmp r1, #0
	beq schedule.exit
	mov sp, r1 // Restore stack pointer
	mov r1, #0
	str r1, [r0] // Clear stack pointer
schedule.exit:
	// Restore context
	ldr r2,  [r3, #0x4] // struct cpu_context* ctx // Set new context
	// Restore register context
	ldr r4,  [r2, #0x00]
	ldr r5,  [r2, #0x04]
	ldr r6,  [r2, #0x08]
	ldr r7,  [r2, #0x0c]
	ldr r8,  [r2, #0x10]
	ldr r9,  [r2, #0x14]
	ldr r10, [r2, #0x18]
	ldr r11, [r2, #0x1c]
	ldr r12, [r2, #0x20]
	ldr lr,  [r2, #0x24]
	bx lr
