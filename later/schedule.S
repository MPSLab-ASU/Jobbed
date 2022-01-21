.section ".text"
.globl schedule
// TODO: Implement Scheduler for IRQ


// Save Context
// reg = struct cpu_context*
.macro save_context reg
	str r4,  [\reg, #0x00]
	str r5,  [\reg, #0x04]
	str r6,  [\reg, #0x08]
	str r7,  [\reg, #0x0c]
	str r8,  [\reg, #0x10]
	str r9,  [\reg, #0x14]
	str r10, [\reg, #0x18]
	str r11, [\reg, #0x1c]
	str r12, [\reg, #0x20]
	str lr,  [\reg, #0x24]
.endm
// Restore Context
// reg = struct cpu_context*
.macro restore_context reg
	ldr r4, [\reg, #0x00]
	ldr r5, [\reg, #0x04]
	ldr r6, [\reg, #0x08]
	ldr r7, [\reg, #0x0c]
	ldr r8, [\reg, #0x10]
	ldr r9, [\reg, #0x14]
	ldr r10, [\reg, #0x18]
	ldr r11, [\reg, #0x1c]
	ldr r12, [\reg, #0x20]
	ldr lr , [\reg, #0x24]
.endm

// Implemented the scheduler in Assembly since the C defined was messing around with the program stacks
//  This way, I can be confident that the stacks will be unchanged
schedule:
	ldr r3, =scheduler
	// Preserve context
	ldr r0, [r3, #4]
	// r0 = struct cpu_context*
	save_context r0
	// Get the next available thread
	push {r1-r3, lr}
	bl get_next_thread
	pop {r1-r3, lr}
	ldr r1,  [r3, #0] 
	// r3 = struct Scheduler*
	// r0 = struct LL* next_thread_ll
	// r1 = struct LL* current_thread_ll
	// Check if there is a valid currently running thread
	cmp r1, #0
	beq schedule.current_thread_nexists
schedule.current_thread_exists:
	cmp r0, r1
	beq schedule.run_current
	cmp r0, #0
	moveq r0, r1 // Make the current running thread the next running thread if no next running thread
	// Next is not the same as the current
	// Preserve stack of current
	ldr r2,  [r1, #0x8] // struct Thread* current
	ldrh r1, [r2, #0x0e]
	cmp r1, #2 // THREAD_WAITING
	beq schedule.temp_status
	cmp r1, #1 // THREAD_RUNNING
	bne schedule.dont_modify_status
schedule.temp_status:
	mov r1, #0 // THREAD_READY
	strh r1, [r2, #0x0e]
schedule.dont_modify_status:
	str sp,  [r2, #0x4] // void* stack // Preserve stack
	// Preserve program counter of current
	str lr,  [r2, #0x0] // void* thread // Preserve pc
	ldr r2,  [r0, #0x8] // struct Thread* next
	// Set new stack pointer
	ldr sp,  [r2, #0x4]
	// Set the thread as running
	mov r1, #1 // THREAD_RUNNING
	strh r1,  [r2, #0x0e] // unsigned short status
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
	ldr r2, =svcsp
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
	ldr r2, [r0, #0x8] // struct Thread* thread
	mov r0, #1 // THREAD_RUNNING
	strh r0, [r2, #0x0e]
	// Set context
	add r1, r1, #0x18 // struct cpu_context*
	str r1, [r3, #0x4] // store to scheduler.ctx
schedule.run_current:
	// Restore context
	ldr r2,  [r3, #0x4] // struct cpu_context* ctx // Set new context
	restore_context r2
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
	ldr r0, =svccpu
	str r0, [r3, #0x4] // Store context
	ldr r0, =svcsp
	ldr r1, [r0]
	cmp r1, #0
	beq schedule.exit
	mov sp, r1 // Restore stack pointer
	mov r1, #0
	str r1, [r0] // Clear stack pointer
schedule.exit:
	// Restore register context
	ldr r2,  [r3, #0x4] // struct cpu_context* ctx // Set new context
	restore_context r2
	bx lr
