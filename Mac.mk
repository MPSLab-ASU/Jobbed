include Common.mk
QEMU = qemu-system-arm
CROSS = ./toolchain/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi

.PHONY: init
init:
	@-mkdir toolchain
	@curl -o toolchain/mac.chain.tar.bz2 "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-mac.tar.bz2"
	@tar xjf toolchain/mac.chain.tar.bz2 -C toolchain/

