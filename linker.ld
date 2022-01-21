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
        KEEP(*(.text.kernel))
        *(.text*)
    }
    . = ALIGN(4096); /* align to page size */
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
        *(.bss.*)
	*(.bss.mmheap)
	*(.bss.mmheap.base)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
}
