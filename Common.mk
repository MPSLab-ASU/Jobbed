# Kernel Sources/Objects
C_SOURCEK = $(wildcard kernel/*.c kernel/**/*.c)
C_OBJECTk = $(C_SOURCEK:.c=.co)
C_OBJECTK = $(subst kernel/,obj/kernel/,$(C_OBJECTk))
A_SOURCEK = $(wildcard kernel/*.S kernel/**/*.S)
A_OBJECTk = $(A_SOURCEK:.S=.ao)
A_OBJECTK = $(subst kernel/,obj/kernel/,$(A_OBJECTk))
# User Sources/Objects
C_SOURCEU = $(wildcard usr/*.c usr/**/*.c)
C_OBJECTu = $(C_SOURCEU:.c=.co)
C_OBJECTU = $(subst usr/,obj/usr/,$(C_OBJECTu))
CXX_SOURCEU = $(wildcard usr/*.cpp usr/**/*.cpp)
CXX_OBJECTu = $(CXX_SOURCEU:.cpp=.cppo)
CXX_OBJECTU = $(subst usr/,obj/usr/,$(CXX_OBJECTu))
A_SOURCEU = $(wildcard usr/*.S usr/**/*.S)
A_OBJECTu = $(A_SOURCEU:.S=.ao)
A_OBJECTU = $(subst usr/,obj/usr/,$(A_OBJECTu))
# Combined Objects
K_OBJECTS = $(A_OBJECTK) $(C_OBJECTK)
U_OBJECTS = $(A_OBJECTU) $(C_OBJECTU) $(CXX_OBJECTU)

ATTACH_USB ?= 0
AUTO ?= 0
BSP ?= 2
GDEBUG ?= 0
DEBUG ?= 0
SILENT ?= 0
LARGE ?= 1

CROSS = arm-none-eabi
AR = $(CROSS)-ar
LD = $(CROSS)-ld
CC = $(CROSS)-gcc
CPP = $(CROSS)-g++
AS = $(CROSS)-as
OBJCOPY = $(CROSS)-objcopy
OBJDUMP = $(CROSS)-objdump
QEMU = qemu-system-arm
GDB = gdb-multiarch
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O3 -Wall -Wextra -nostdlib -Iinclude -g
CXXFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -fno-exceptions -O3 -Wall -Wextra -nostdlib -Iinclude -g
CFLAGS += -DVERSION="\"0.9z\""
AFLAGS = -mcpu=cortex-a7 -Iinclude -g
QFLAGS = -M raspi2b -cpu cortex-a7 -m 1G

# Attach USB if requested
ifneq ("$(ATTACH_USB)","0")
	QFLAGS += -device usb-kbd
	QFLAGS += -trace events=events
endif

# Use Automation Pipe if requested
ifneq ("$(AUTO)","0")
	QFLAGS += -chardev pipe,id=char0,mux=on,logfile=serial.log,path=guest -serial chardev:char0 -mon chardev=char0
else
	QFLAGS += -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0
endif

# Use Correct MMIO Address
ifeq ($(BSP),2)
	BSP23 = 1
	CFLAGS += -DBSP23
endif

# Pause and wait for GDB if requested
ifneq ($(GDEBUG),0)
	QFLAGS += -s -S
endif

ifeq ($(LARGE),1)
	CFLAGS += -DLARGE_LETTERS
endif

# Debugging Flag
ifneq ($(DEBUG),0)
	CFLAGS += -DDEBUG
endif

# Don't use screen if requested
ifneq ("$(SILENT)","0")
	QFLAGS += -nographic
endif

# Attach sd if exists
ifneq ("$(wildcard ./sd.hda)","")
	QFLAGS += -drive file=sd.hda,if=sd,format=raw
endif

.PHONY: clean debug disk dump run test tree

default: clean build/kernel7.img

build/kernel7.img: CFLAGS += -DRPI_BUILD
build/kernel7.img: CFLAGS := $(filter-out -g,$(CFLAGS))
build/kernel7.img: AFLAGS := $(filter-out -g,$(AFLAGS))
build/kernel7.img: build/kernel.elf
	@mkdir -p $(@D)
	@echo "IMAGE   BUILD  $@"
	@$(OBJCOPY) $< -O binary $@

build/kernel.list: build/kernel.elf
	@mkdir -p $(@D)
	@echo "IMAGE  LIST   $@"
	@$(OBJDUMP) -D $< > $@

dump: build/kernel.list

build/kernel.elf: build/libjobbed.a $(U_OBJECTS)
	@mkdir -p $(@D)
	@echo "IMAGE   LD     $@"
	@$(LD) -T linker.ld -o $@ -Lbuild -l jobbed -Map build/kernel.map -O3 -nostdlib $^

obj/kernel/%.co: kernel/%.c
	@mkdir -p $(@D)
	@echo "KERNEL  CC     $@"
	@$(CC) $(CFLAGS) -c $< -o $@

obj/kernel/%.ao: kernel/%.S
	@mkdir -p $(@D)
	@echo "KERNEL  AS     $@"
	@$(AS) $(AFLAGS) -c $< -o $@

obj/usr/%.co: usr/%.c
	@mkdir -p $(@D)
	@echo "USER    CC     $@"
	@$(CC) $(CFLAGS) -c $< -o $@

obj/usr/%.cppo: usr/%.cpp
	@mkdir -p $(@D)
	@echo "USER    CPP    $@"
	@$(CPP) $(CXXFLAGS) -c $< -o $@

obj/usr/%.ao: usr/%.S
	@mkdir -p $(@D)
	@echo "USER    AS     $@"
	@$(AS) $(AFLAGS) -c $< -o $@

run: build/kernel.elf
	@tput setaf 6 2> /dev/null || true; echo RUNNING IN QEMU; tput sgr0 2> /dev/null || true
	@$(QEMU) -kernel $< $(QFLAGS)

debug: build/kernel.list
	@tput setaf 6 2> /dev/null || true; echo STARTING GDB; tput sgr0 2> /dev/null || true
	@$(GDB) $< -command=gdbinit

sd.hda:
	@cp README.md sd.hda
	@dd if=/dev/zero of=sd.hda count=1 bs=1 seek=16383

build/libjobbed.a: $(K_OBJECTS)
	@mkdir -p $(@D)
	@-rm -f $@
	@echo "JOBBED  LIB    $@"
	@$(AR) rc $@ $^

disk: sd.hda

clean:
	@-rm -r obj/* build/*

tree:
	@tree -a -I obj\|build\|.git\|.gitignore

test: clean build/kernel.elf
	@tput setaf 6 2> /dev/null || true; echo Running Tests; tput sgr0 2> /dev/null || true
	@./tests/run.sh

ifndef DISK
copy:
	@tput setaf 1 2> /dev/null || true; echo ERROR: No disk specified!; tput sgr0 2> /dev/null || true
else
copy: clean build/kernel7.img
	sudo mount $(DISK) /mnt/sd0
	sudo cp build/kernel7.img /mnt/sd0
	sudo umount /mnt/sd0
endif
