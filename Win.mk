C_SOURCES = $(wildcard src/*.c)
C_HEADERS = $(wildcard src/*.h)
C_OBJECTS = ${C_SOURCES:.c=.o}
C_OBJECTD = ${subst src,obj,${C_OBJECTS}}
A_SOURCES = $(wildcard src/*.S)
A_OBJECTS = ${A_SOURCES:.S=.o}
A_OBJECTD = ${subst src,obj,${A_OBJECTS}}

CROSS = arm-none-eabi
CC = ${CROSS}-gcc
AS = ${CROSS}-as
OBJCOPY = ${CROSS}-objcopy
OBJDUMP = ${CROSS}-objdump
QEMU = /mnt/c/qemu/qemu-system-arm.exe
GDB = gdb-multiarch
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra -nostdlib -g
AFLAGS = -mcpu=cortex-a7
QFLAGS = -M raspi2 -cpu arm1176 -m 1G -serial mon:stdio -nographic

.PHONY: clean run run-debug debug export

default: clean build/kernel7.img

build/kernel7.img: build/kernel.elf
	${OBJCOPY} $< -O binary $@

build/kernel.list: build/kernel-g.elf
	${OBJDUMP} -D $< > $@

build/kernel-g.elf: ${A_OBJECTD} ${C_OBJECTD}
	${CC} -T linker.ld -o $@ ${CFLAGS} $^ -lgcc

build/kernel.elf: ${A_OBJECTD} ${C_OBJECTD}
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

export: build/kernel.list
	cp build/kernel-g.elf /mnt/c/Local/

obj/%.o: src/%.S
	${AS} ${AFLAGS} -g -c $< -o $@

run: build/kernel.elf
	${QEMU} -kernel $< ${QFLAGS}

run-debug: build/kernel-g.elf
	${QEMU} -kernel $< -s -S ${QFLAGS}

debug: build/kernel-g.elf build/kernel.list
	${GDB} $< -command=gdbinit

clean:
	rm -f obj/*.o build/*.elf build/*.list build/*.img
