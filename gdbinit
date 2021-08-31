set arch arm
layout regs
target remote localhost:1234
b _start
winheight regs +3
set scheduler-locking on
thread 1
