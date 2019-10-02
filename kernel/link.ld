ENTRY(kernel_entry)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
	KERNEL_VADDR = 0xFFFFFFFFF8000000;
	. = KERNEL_VADDR;

	.text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VADDR)
	{
		*(.text*)
		*(.gnu.linkonce.t*)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - KERNEL_VADDR)
	{
		*(.rodata*)
		*(.gnu.linkonce.r*)
	}

	.stuff ALIGN(0x1000) : AT(ADDR(.stuff) - KERNEL_VADDR)
	{
		CTORS_BEGIN = .;

		*(SORT(.ctors))
		*(SORT(.ctors*))

		CTORS_END   = .;
		DTORS_BEGIN = .;

		*(SORT(.dtors))

		DTORS_END   = .;
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VADDR)
	{
		*(.data*)
		*(.gnu.linkonce.d*)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VADDR)
	{
		*(COMMON)
		*(.bss*)
		*(.gnu.linkonce.b*)
	}
}
