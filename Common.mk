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
QEMU = qemu-system-arm
GDB = gdb-multiarch
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O3 -Wall -Wextra -nostdlib -Iinclude -g
AFLAGS = -mcpu=cortex-a7 -Iinclude -g
QFLAGS = -M raspi2 -cpu arm1176 -m 1G -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0 -device usb-kbd
#QFLAGS += -trace events=events 
#QFLAGS += -nographic

BSP ?= 2

ifeq ($(BSP),2)
	BSP23 = 1
	CFLAGS += -DBSP23
endif

CFLAGS += -DVERSION="\"0.0s\""

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
	${CC} -T linker.ld -o $@ -ffreestanding -O3 -nostdlib $^ -lgcc

export: build/kernel.list
	cp build/kernel-g.elf /mnt/c/Local/

obj/%.co: src/%.c
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

obj/%.ao: src/%.S
	@mkdir -p $(@D)
	${AS} ${AFLAGS} -c $< -o $@

run: build/kernel.elf
	@echo Starting QEMU
	@${QEMU} -kernel $< ${QFLAGS}

run-debug: build/kernel-g.elf
	@echo Starting QEMU in Debug Mode
	@${QEMU} -kernel $< -s -S ${QFLAGS}

debug: build/kernel-g.elf build/kernel.list
	${GDB} $< -command=gdbinit

clean:
	rm -rf obj/* build/*

tree:
	@tree -a -I obj\|build\|.git\|.gitignore
