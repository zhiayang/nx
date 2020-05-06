// disasm.cpp
// Copyright (c) 2020, zhiayang, Apache License 2.0.

#include "nx.h"
#include "instrad/x64/decode.h"

namespace nx::disasm
{
	using instrad::Buffer;
	using namespace instrad;

	constexpr size_t DISASM_COUNT = 5;

	static void print_intel(const x64::Instruction& instr);

	void printDisassembly(addr_t rip)
	{
		serial::debugprintf("disassembly at %p\n", (void*) rip);

		auto buf = Buffer((uint8_t*) rip, /* some arbitrary number: */ 0x1000);
		for(size_t i = 0; i < DISASM_COUNT; i++)
		{
			auto instr = x64::read(buf, x64::ExecMode::Long);
			print_intel(instr);
		}

		serial::debugprintf("\n");
	}




	static void print_intel(const x64::Instruction& instr)
	{
		auto& printer = serial::debugprintf;

		auto print_operand = [&printer](const instrad::x64::Operand& op) {
			if(op.isRegister())
			{
				printer("%s", op.reg().name());
			}
			else if(op.isImmediate())
			{
				int64_t value = op.imm();
				if(op.immediateSize() == 8)  value = (uint8_t) value;
				if(op.immediateSize() == 16) value = (uint16_t) value;
				if(op.immediateSize() == 32) value = (uint32_t) value;

				printer("%#lx", value);
			}
			else if(op.isMemory())
			{
				auto& mem = op.mem();
				auto& base = mem.base();
				auto& idx = mem.index();

				switch(mem.bits())
				{
					case 8:     printer("BYTE PTR ");     break;
					case 16:    printer("WORD PTR ");     break;
					case 32:    printer("DWORD PTR ");    break;
					case 64:    printer("QWORD PTR ");    break;
					case 80:    printer("TWORD PTR ");    break;
					case 128:   printer("XMMWORD PTR ");  break;
					case 256:   printer("YMMWORD PTR ");  break;
					case 512:   printer("ZMMWORD PTR ");  break;
					default:    break;
				}

				if(mem.segment().present())
					printer("%s:", mem.segment().name());

				// you can't scale a displacement, so we're fine here.
				if(!base.present() && !idx.present())
				{
					printer("[%#x]", mem.displacement());
					return;
				}

				printer("[");
				if(base.present())
				{
					printer("%s", base.name());
					if(idx.present() || op.mem().displacement() != 0 || op.mem().scale() != 1)
						printer(" + ");
				}

				if(idx.present())
					printer("%s", idx.name());

				if(op.mem().scale() != 1)
					printer("*%d", op.mem().scale());

				if(op.mem().displacement() != 0)
				{
					if(idx.present() || op.mem().scale() != 1)
						printer(" + ");

					printer("%#x", op.mem().displacement());
				}

				printer("]");
			}
			else
			{
				serial::debugprintf("??");
			}
		};


		printer("    ");

		// print the bytes
		size_t col = 0;
		for(size_t i = 0; i < instr.numBytes(); i++)
		{
			printer("%02x ", instr.bytes()[i]);
			col += 3;
		}

		while(col++ < 25)
			printer(" ");

		if(instr.lockPrefix())  printer("lock ");
		if(instr.repPrefix())   printer("rep ");
		if(instr.repnzPrefix()) printer("repnz ");

		printer("%s ", instr.op().mnemonic());

		if(instr.operandCount() == 1)
		{
			print_operand(instr.dst());
		}
		else if(instr.operandCount() == 2)
		{
			print_operand(instr.dst()); printer(", ");
			print_operand(instr.src());
		}
		else if(instr.operandCount() == 3)
		{
			print_operand(instr.dst()); printer(", ");
			print_operand(instr.src()); printer(", ");
			print_operand(instr.ext());
		}

		printer("\n");
	}
}
