@echo off
setlocal enabledelayedexpansion

REM Cross Compiler Location
set prefix="arm-none-eabi-"
set armpath="C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\11.2 2022.02\bin\"
set armrun=%armpath%%prefix%

REM Clean up build directories
rmdir /Q /S obj\
mkdir obj\
rmdir /Q /S build\
mkdir build\
REM Make Kernel Object Directories
for /f %%i in ('forfiles /S /P kernel\ /C "cmd /C if @isdir==TRUE echo @relpath"') do (
    mkdir obj\kernel\%%~i
)
REM Make User Object Directories
for /f %%i in ('forfiles /S /P usr\ /C "cmd /C if @isdir==TRUE echo @relpath"') do (
    mkdir obj\usr\%%~i
)

REM Kernel Object Variable
set kobj=
REM Assemble Assembly Kernel Source Files
for /f %%i in ('forfiles /S /M *.S /P kernel\ /C "cmd /C echo @relpath"') do (
    set src=kernel\%%~i
    set obj=!src!
    set obj=!obj:.S=.So!
    set obj=obj\!obj!
    @set kobj=!kobj! !obj!
    @echo KERNEL  AS  !src! -- !obj!
    %armrun%as.exe -mcpu=cortex-a7 -Iinclude -c !src! -o !obj!
)
REM Compile C Kernel Source Files
for /f %%i in ('forfiles /S /M *.c /P kernel\ /C "cmd /C echo @relpath"') do (
    set src=kernel\%%~i
    set obj=!src!
    set obj=!obj:.c=.co!
    set obj=obj\!obj!
    @set kobj=!kobj! !obj!
    @echo KERNEL  CC  !src! -- !obj!
    %armrun%gcc.exe -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O3 -Wall -Wextra -nostdlib -Iinclude -c !src! -o !obj!
)

REM Create Kernel Archive
@echo KERNEL  AR  build\libjobbed.a
%armrun%ar.exe rc build\libjobbed.a %kobj%

REM User Object Variable
set uobj=
REM Assemble Assembly User Source Files
for /f %%i in ('forfiles /S /M *.S /P usr\ /C "cmd /C echo @relpath"') do (
    set src=usr\%%~i
    set obj=!src!
    set obj=!obj:.S=.ao!
    set obj=obj\!obj!
    @set uobj=!uobj! !obj!
    @echo   USER  AS  !src! -- !obj!
    %armrun%as.exe -mcpu=cortex-a7 -Iinclude -c !src! -o !obj!
)
REM Compile C User Source Files
for /f %%i in ('forfiles /S /M *.c /P usr\ /C "cmd /C echo @relpath"') do (
    set src=usr\%%~i
    set obj=!src!
    set obj=!obj:.c=.co!
    set obj=obj\!obj!
    @set uobj=!uobj! !obj!
    @echo   USER  CC  !src! -- !obj!
    %armrun%gcc.exe -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O3 -Wall -Wextra -nostdlib -Iinclude -c !src! -o !obj!
)
REM Compile C++ User Source Files
for /f %%i in ('forfiles /S /M *.cpp /P usr\ /C "cmd /C echo @relpath"') do (
    set src=usr\%%~i
    set obj=!src!
    set obj=!obj:.cpp=.cppo!
    set obj=obj\!obj!
    @set uobj=!uobj! !obj!
    @echo   USER  CX  !src! -- !obj!
    %armrun%g++.exe -mcpu=cortex-a7 -fpic -ffreestanding -fno-exceptions -O3 -Wall -Wextra -nostdlib -Iinclude -c !src! -o !obj!
)

@echo  IMAGE  LD  build\kernel.elf
%armrun%ld.exe -T linker.ld -o build\kernel.elf -Lbuild -l jobbed -Map build\kernel.map -O3 -nostdlib !uobj! build\libjobbed.a