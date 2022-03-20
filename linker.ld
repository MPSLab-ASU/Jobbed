ENTRY(_start)
 
SECTIONS
{
	. = 0x4000;
	__ttbr = .;
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
    . = ALIGN(4096);
    __text_end = .;

    __data_start = .;
    .data :
    {
	    *(.data*)
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
	    KEEP(*(.bss.threadl))
	    . = ALIGN(4096);
	    KEEP(*(.bss.threads))
	    . = ALIGN(4096);
	    KEEP(*(.bss.threade))
	    . = ALIGN(4096);
	    KEEP(*(.bss.mutexl))
	    . = ALIGN(4096);
	    KEEP(*(.bss.mutexs))
	    . = ALIGN(4096);
	    *(.bss)
		    *(.bss.*)
		    KEEP(*(.bss.mmheap))
		    KEEP(*(.bss.mmheap.base))
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
    /DISCARD/ : {
	    *(.ARM.attributes*)
		    *(.comment*)
		    *(.debug*)
    }
}
