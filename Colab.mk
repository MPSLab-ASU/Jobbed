include Common.mk
QEMU = qemu-system-arm
CROSS = ./toolchain/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi

init:
	@-mkdir toolchain
	@curl -o toolchain/colab.chain.tar.bz2 "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2"
	@tar xjvf toolchain/colab.chain.tar.bz2 -C toolchain/
