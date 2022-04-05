# Jobbed
## Notice
 - Currently `make -f Unix.mk copy` has a default disk /dev/sdc1. If you run it, it will mount this, copy the built image, and then unmount it.
 - While this wouldn't overwrite any data, the unmounting of this disk may be errant on your computer configuration, causing you to lose access to files until you remount the disk.
 - This will be changed in a few releases, but as it stands currently, this warning is here to prevent any transient problems

## Building (Debian)
 - Clone the repository.
 - Ensure you have the `arm-none-eabi-gcc` cross compiler installed.
 - From the root of the directory, i.e. in `Jobbed`, execute `make -f Unix.mk`

## Getting Cross Compiler
 - https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads

## Todo
 - Finish Build Instructions
 - Deterministic Graphics
 - Move UART Polling Write to another method
 - Make UART Write run as an event-based system

## Future
 - USB Driver
 - Ethernet Driver
 - SPI Driver
 - GPIO Falling Edge IRQ

## SVC Calls (In order)
 - 0: Yield to threads in same priority
 - 1: System Clock Time
 - 2: Schedule
 - 3: Add thread
 - 4: Mutex Lock
 - 5: Mutex Unlock
 - 6: Semaphore Decrease
 - 7: Semaphore Increase
 - 8: Semaphore Increase by N
 - 9: Cycle Counter
 - A: Enable ARM Timer
 - B: Disable ARM Timer


Note: SYS and USER share registers but differ in privilege


# Modes
 - From https://developer.arm.com/documentation/den0013/d/ARM-Processor-Modes-and-Registers
 - User: 10:10000 PL0
 - FIQ : 11:10001 PL1
 - IRQ : 12:10010 PL1
 - SVC : 13:10011 PL1
 - ABT : 17:10111 PL1
 - HVC : 1A:11010 PL1
 - UND : 1B:11011 PL1
 - SYS : 1F:11111 PL1

# Registers
 - From https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-core-registers?lang=en
 - User: r0-12_usr,sp_usr,lr_usr,CPSR
 - FIQ: r8-r12_fiq,sp_fiq,lr_fiq,SPSR_fiq
 - IRQ: sp_irq,lr_irq,SPSR_irq
 - SVC: sp_svc,lr_svc,SPSR_svc
 - ABT: sp_abt,lr_abt,SPSR_abt
 - UND: sp_und,lr_und,SPSR_und
 - SYS: Same as user

# Memory Layout
 - 0x00004000 - 0x00008000 MMU Entries
 - 0x00008000 - 0x0000F000 Code
 - 0x0000F000 - 0x00010000 Data
 - 0x00010000 - 0x000A3000 BSS
 - 0x18000000 - 0x20000000 Thread Stacks
 - 0x3F000000 - 0x40000000 MMIO
 - 0x40000000 - 0xFFFFFFFF Local Peripherals

## Supplemental Links
 - https://www.raspberrypi.com/documentation/computers/processors.html
 - https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf
 - https://datasheets.raspberrypi.com/bcm2836/bcm2836-peripherals.pdf
 - https://github.com/eggman/raspberrypi/tree/master/qemu-raspi2
 - https://github.com/eggman/FreeRTOS-raspi3/blob/master/Demo/FreeRTOS_tick_config.c
 - https://developer.arm.com/documentation/dui0802/b/A64-General-Instructions/A64-general-instructions-in-alphabetical-order?lang=en
 - https://wiki.osdev.org/ARM_RaspberryPi
 - https://elinux.org/RPi_Framebuffer
 - https://github.com/bztsrc/raspi3-tutorial
 - https://github.com/rust-embedded/rust-raspberrypi-OS-tutorials

 - // PRIV LEVL: https://developer.arm.com/documentation/den0013/d/ARM-Processor-Modes-and-Registers
 - // MODES: https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-processor-modes?lang=en
 - // EXCEPTION HANDLING: https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/Exception-handling?lang=en
 - // MODE REGISTERS: https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-core-registers?lang=en

# Program Status Registers
 - https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/Program-Status-Registers--PSRs-?lang=en#CIHJBHJA
