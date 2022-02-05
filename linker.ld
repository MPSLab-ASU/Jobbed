ENTRY(_start)
 
SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x8000;
    __start = .;
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        KEEP(*(.text.exceptions))
        KEEP(*(.text.kernel))
        *(.text*)
    }
    . = 0x208000; /* 2MiB Kernel/ Boot */
    __text_end = .;
 
    __data_start = .;
    .data :
    {
        *(.data*)
	__stacks_start = .;
	KEEP(*(.data.stacks))
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss :
    {
        bss = .;
	. = ALIGN(4096);
	KEEP(*(.bss.estacks))
	. = ALIGN(4096);
	KEEP(*(.bss.kmem))
	. = ALIGN(4096);
        *(.bss)
        *(.bss.*)
	KEEP(*(.bss.mmheap))
	KEEP(*(.bss.mmheap.base))
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
}
