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
QFLAGS = -M raspi2b -cpu cortex-a7 -m 1G
#QFLAGS += -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0
QFLAGS += -chardev pipe,id=char0,mux=on,logfile=serial.log,path=guest -serial chardev:char0 -mon chardev=char0
QFLAGS += -device usb-kbd
ifneq ("$(wildcard ./sd.hda)","")
	QFLAGS += -drive file=sd.hda,if=sd,format=raw
endif
#QFLAGS += -trace events=events
#QFLAGS += -nographic

BSP ?= 2

ifeq ($(BSP),2)
	BSP23 = 1
	CFLAGS += -DBSP23
endif

CFLAGS += -DVERSION="\"0.1b\""

.PHONY: clean run run-debug debug export tree disk

default: clean build/kernel7.img

build/kernel7.img: build/kernel.elf
	@mkdir -p $(@D)
	${OBJCOPY} $< -O binary $@

build/kernel.list: build/kernel-g.elf
	@mkdir -p $(@D)
	${OBJDUMP} -D $< > $@

build/kernel-g.elf: ${A_OBJECTD} ${C_OBJECTD}
	@mkdir -p $(@D)
	${CC} -T linker.ld -o $@ ${CFLAGS} $^

build/kernel.elf: ${A_OBJECTD} ${C_OBJECTD}
	@tput setaf 6 2> /dev/null || true; echo Linking Kernel; tput sgr0 2> /dev/null || true
	@mkdir -p $(@D)
	${CC} -T linker.ld -o $@ -ffreestanding -O3 -nostdlib $^

export: build/kernel.elf sd.hda
	cp $^ /mnt/c/temp/

obj/%.co: src/%.c
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

obj/%.ao: src/%.S
	@mkdir -p $(@D)
	${AS} ${AFLAGS} -c $< -o $@

run: build/kernel.elf
	@tput setaf 6 2> /dev/null || true; echo Starting QEMU; tput sgr0 2> /dev/null || true
	@${QEMU} -kernel $< ${QFLAGS}

run-debug: build/kernel-g.elf
	@echo Starting QEMU in Debug Mode
	@${QEMU} -kernel $< -s -S ${QFLAGS}

debug: build/kernel-g.elf build/kernel.list
	${GDB} $< -command=gdbinit

sd.hda:
	cp README.md sd.hda
	dd if=/dev/zero of=sd.hda count=1 bs=1 seek=16383

disk: sd.hda

clean:
	rm -rf obj/* build/*

tree:
	@tree -a -I obj\|build\|.git\|.gitignore
