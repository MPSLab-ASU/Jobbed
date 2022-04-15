# Jobbed

## Build Information
### Building (Debian, WSL)
 - Clone the repository.
 - Ensure you have the `gcc-arm-none-eabi` cross compiler installed `# apt install gcc-arm-none-eabi`.
 - From the root of the directory, i.e. in `Jobbed`, execute `make -f Unix.mk`
 - The built image is found in `build/kernel7.img` and can be copied to the root of the Raspberry Pi SD card's first parition
 - On WSL you may also need to run `# apt install build-essential git` to get `make` and `git`
 - For faster building, it is recommended to run `make clean;make -f Unix.mk -j4`, where 4 can be adjusted to the amount of parallel jobs that the system can perform

### Building MacOS
 - Clone the repository
 - Ensure you have the `gcc-arm-embedded` formulae installed `$ brew install gcc-arm-embedded`.
 - From the root of the directory, i.e. in `Jobbed`, execute `make -f Unix.mk`
 - The built image is found in `build/kernel7.img` and can be copied to the root of the Raspberry Pi SD card's first parition

### Building MacOS with Cross Compiler from ARM's website
 - Clone the repository
 - Ensure you have the `gcc-arm-embedded` installed from the website, 10-3 2021 July
 - From the root of the directory, i.e. in `Jobbed`, execute `make -f Mac.mk`
 - The built image is found in `build/kernel7.img` and can be copied to the root of the Raspberry Pi SD card's first parition

### Building (Google Colab)
 - `!git clone https://github.com/TerminalCursor/Jobbed.git`
 - `!apt install gcc-arm-none-eabi`
 - `!cd Jobbed; make -f Unix.mk`
 - The built image is found in `Jobbed/build/kernel7.img` and can be copied to the root of the Raspberry Pi SD card's first parition
 - Currently, it appears that the Ubuntu Distribution's cross compiler is not properly assembling the binaries. This issue is being looked into

### Creating a Bootable SD Card for the Raspberry Pi 2B
 - The Raspberry Pi SD Card Imager can be obtained at `https://www.raspberrypi.com/software/`
 - After imaging the SD card with the imaging software, delete all of the `kernel*.img` files in the root directory of the first partition of the SD card
 - After removing all of the image files, copy the built `build/kernel7.img` file to the root directory of the first partition of the SD card
 - Unmount the SD card, insert the SD into the Raspberry Pi, and apply power to run **Jobbed**

### Running in QEMU Emulator
 - Ensure you have QEMU's ARM package installed on your system
 - Minimum command line arguments to run: `qemu-system-arm -M raspi2b -cpu cortex-a7 -m 1G -kernel build/kernel.elf`
 - When building `build/kernel7.img`, `build/kernel.elf` will also be built and is used to emulate the Raspberry Pi 2B
 - If QEMU is installed, you can run **Jobbed** with `make -f Unix.mk clean run`
 - You can also disable the QEMU window from appearing if you only have UART communication in your application with `SILENT=1 make -f Unix.mk clean run`

### Running in QEMU Emulator on WSL
 - Download the QEMU ARM System Package and install it under `C:\qemu`
 - Run **Jobbed** in QEMU from WSL with `make -f Win.mk clean run`

### Getting Cross Compiler
 - https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads

## Creating RTOS Applications

**Jobbed**'s scheduler puts threads on FIFO queues of various priorities.
The number of priorities available can be changed in `include/sys/schedule.h`.

### RTOS Application Source Location
RTOS Applications are written in the `usr/` directory.
Jobbed, in `kernel/core.c` adds in the `main` function at the highest priority, expected to be defined in `usr/main.c`.
This `main` function, can serve as the initialization function for the RTOS application.

### RTOS Application Header Location
Headers for files in `usr/` are expected in `include/usr/`. They can then be included in source files with `#include <usr/path/to/file.h>`.

### Setting IRQ Callbacks
IRQ handles can be initialized with the `subscribe_irq(IRQ_NAME, callback_function, pointer to additional IRQ information)`.
Examples can be found in the provided `usr/main.c`.
 - `subscribe_irq(IRQ name, callback, pointer to information)` include `<cpu/irq.h>`
 - `unsubscribe_irq(IRQ name)` include `<cpu/irq.h>`

### Thread Functions
 - Allow another thread with the same priority to run (i.e. push this thread to the end of the FIFO queue): `sys0(SYS_YIELD)` include `<cpu.h>`
 - Add a function to a priority level: `add_thread(function, argument, priority)` include `<sys/schedule.h>`

### Mutex Functions
 - Create a mutex: `create_mutex(address of resource)` include `<util/mutex.h>`
 - Delete a mutex: `delete_mutex(address of mutex)` include `<util/mutex.h>`
 - Lock a mutex: `lock_mutex(address of mutex)` include `<util/mutex.h>`
 - Unlock a mutex: `unlock_mutex(address of mutex)` include `<util/mutex.h>`

### Lock Functions
Locks are like mutexes, but they don't carry any reference to a resource and the system does not manage references to them.
For the RTOS application designer, this means that there are conditions that locks can cause deadlocks.
Thus, it is recommended to use the Mutex objects instead of locks.
 - Lock: `lock(address of lock)` include `<util/lock.h>`
 - Unlock: `unlock(address of lock)` include `<util/lock.h>`

### Semaphore Functions
 - Signal a semaphore: `sys1(SYS_SEMAPHORE_V, address of semaphore)` include `<cpu.h>`
 - Wait on a semaphore: `sys1(SYS_SEMAPHORE_P, address of semaphore)` include `<cpu.h>`

### Other Functions
 - Get time since boot: `sys0_64(SYS_TIME, address of uint64 to store the time)` include `<cpu.h>`
 - Draw to the screen (include `<graphics/lfb.h>`)
   - Strings: `draw_string(x, y, string)`
   - 32-bit words: `draw_hex32(x, y, value)`
   - Pixel: `draw_cpixel(x, y, RGB color)`
   - Box: `draw_cbox(x, y, dx, dy, RGB color)`

### Default RTOS Application
Currently, if you build Jobbed without modifying anything, it will generate the RTOS core testing suite.
This testing suite outputs the tracing, thread switch, low and high priority thread creation, mutex creation, mutex destruction, mutex locking contention, mutex locking non-contention, mutex unlocking, semaphore waiting, semaphore signalling at zero, semaphore signalling non-zero timings to a 1920x1080 display output.
Currently the graphics driver expects this resolution of display.
If you have another resolution, this can be changed in `kernel/sys/core.c` by modifying `lfb_init(1920, 1080)` to `lfb_init(YOUR WIDTH, YOUR HEIGHT)`.

### C++
C++ sources in this directory are expected to work with a few missing features such as the `new` and `delete` keywords since memory is not dynamically allocated on the system.

## Misc Information

### Future
 - USB Driver
 - Ethernet Driver
 - Move UART Polling Write to another method
 - Make UART Write run as an event-based system
 - Error-Resistant Graphics
 - SPI Driver
 - GPIO Falling Edge IRQ

### SVC Calls (In order)
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


### Modes
 - From https://developer.arm.com/documentation/den0013/d/ARM-Processor-Modes-and-Registers
 - User: 10:10000 PL0
 - FIQ : 11:10001 PL1
 - IRQ : 12:10010 PL1
 - SVC : 13:10011 PL1
 - ABT : 17:10111 PL1
 - HVC : 1A:11010 PL1
 - UND : 1B:11011 PL1
 - SYS : 1F:11111 PL1

### Registers
 - From https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-core-registers?lang=en
 - User: r0-12_usr,sp_usr,lr_usr,CPSR
 - FIQ: r8-r12_fiq,sp_fiq,lr_fiq,SPSR_fiq
 - IRQ: sp_irq,lr_irq,SPSR_irq
 - SVC: sp_svc,lr_svc,SPSR_svc
 - ABT: sp_abt,lr_abt,SPSR_abt
 - UND: sp_und,lr_und,SPSR_und
 - SYS: Same as user

Note: SYS and USER share registers but differ in privilege

### Program Status Registers
 - https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/Program-Status-Registers--PSRs-?lang=en#CIHJBHJA

### Memory Layout
 - 0x00004000 - 0x00008000 MMU Entries
 - 0x00008000 - 0x0000F000 Code
 - 0x0000F000 - 0x00010000 Data
 - 0x00010000 - 0x000A3000 BSS
 - 0x18000000 - 0x20000000 Thread Stacks
 - 0x3F000000 - 0x40000000 MMIO
 - 0x40000000 - 0xFFFFFFFF Local Peripherals

### Supplemental Links
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
