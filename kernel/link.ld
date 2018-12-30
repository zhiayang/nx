ENTRY(kernel_entry)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
	Kernel_VMA = 0xFFFFFFFF80000000;
	. = Kernel_VMA;

	.text ALIGN(0x1000) : AT(ADDR(.text) - Kernel_VMA)
	{
		*(.text*)
		*(.gnu.linkonce.t*)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - Kernel_VMA)
	{
		*(.rodata*)
		*(.gnu.linkonce.r*)
	}

	.stuff ALIGN(0x1000) : AT(ADDR(.stuff) - Kernel_VMA)
	{
		start_ctors = .;
		*(SORT(.ctors))
		*(SORT(.ctors*))
		end_ctors = .;

		start_dtors = .;
		*(SORT(.dtors))
		end_dtors = .;
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - Kernel_VMA)
	{
		*(.data*)
		*(.gnu.linkonce.d*)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - Kernel_VMA)
	{
		StartBSS = .;
		*(COMMON)
		*(.bss*)
		*(.gnu.linkonce.b*)
		EndBSS = .;
	}

	KernelEnd = .;
}