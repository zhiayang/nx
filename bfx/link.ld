mmio  = 0xfffffffff8000000;
fb    = 0xfffffffffc000000;
ENTRY(entry)

SECTIONS
{
	. = 0xffffffffffe00000;
	bootboot    = .; . += 4096;
	environment = .; . += 4096;
	.text : {
		KEEP(*(.text.boot)) *(.text .text.*)   /* code */
		*(.rodata .rodata.*)                   /* data */
		*(.data .data.*)
	}
	.bss (NOLOAD) : {                          /* bss */
		. = ALIGN(16);
		*(.bss .bss.*)
		*(COMMON)
	}

	/DISCARD/ : { *(.eh_frame) *(.comment) }
}
