set arch arm
tui new-layout rtosasm regs 1 asm 1 status 0 cmd 1
tui new-layout rtossrc regs 1 src 1 status 0 cmd 1
layout rtosasm
focus asm
target remote localhost:1234
b _start
winheight regs +3
set scheduler-locking on
thread 1
