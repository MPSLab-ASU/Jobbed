C_SOURCES = $(wildcard src/*.c src/**/*.c)
C_HEADERS = $(wildcard src/*.h src/**/*.h)
C_OBJECTS = ${C_SOURCES:.c=.co}
C_OBJECTD = ${subst src,obj,${C_OBJECTS}}
A_SOURCES = $(wildcard src/*.S src/**/*.S)
A_OBJECTS = ${A_SOURCES:.S=.ao}
A_OBJECTD = ${subst src,obj,${A_OBJECTS}}

CROSS = arm-none-eabi
CC = ${CROSS}-gcc
AS = ${CROSS}-as
OBJCOPY = ${CROSS}-objcopy
OBJDUMP = ${CROSS}-objdump
QEMU = /mnt/c/qemu/qemu-system-arm.exe
GDB = gdb-multiarch
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O3 -Wall -Wextra -nostdlib -g
AFLAGS = -mcpu=cortex-a7 -g
QFLAGS = -M raspi2 -cpu arm1176 -m 1G -serial mon:stdio -nographic

BSP ?= 2

ifeq ($(BSP),2)
	BSP23 = 1
	CFLAGS += -DBSP23
endif

CFLAGS += -DVERSION="\"0.0b\""

.PHONY: clean run run-debug debug export tree

default: clean build/kernel7.img

build/kernel7.img: build/kernel.elf
	@mkdir -p $(@D)
	${OBJCOPY} $< -O binary $@

build/kernel.list: build/kernel-g.elf
	@mkdir -p $(@D)
	${OBJDUMP} -D $< > $@

build/kernel-g.elf: ${A_OBJECTD} ${C_OBJECTD}
	@mkdir -p $(@D)
	${CC} -T linker.ld -o $@ ${CFLAGS} $^ -lgcc

build/kernel.elf: ${A_OBJECTD} ${C_OBJECTD}
	@mkdir -p $(@D)
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

export: build/kernel.list
	cp build/kernel-g.elf /mnt/c/Local/

obj/%.co: src/%.c
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

obj/%.ao: src/%.S
	@mkdir -p $(@D)
	${AS} ${AFLAGS} -c $< -o $@

run: build/kernel.elf
	@${QEMU} -kernel $< ${QFLAGS}

run-debug: build/kernel-g.elf
	@${QEMU} -kernel $< -s -S ${QFLAGS}

debug: build/kernel-g.elf build/kernel.list
	${GDB} $< -command=gdbinit

clean:
	rm -f obj/*.o build/*.elf build/*.list build/*.img obj/*.ao obj/*.co

tree:
	@tree -a -I obj\|build\|.git\|.gitignore
