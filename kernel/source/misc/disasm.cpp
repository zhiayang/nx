// disasm.cpp
// Copyright (c) 2020, zhiayang, Apache License 2.0.

#include "nx.h"
#include "instrad/x64/decode.h"

namespace nx::disasm
{
	using instrad::Buffer;
	using namespace instrad;

	constexpr size_t DISASM_COUNT = 5;

	static void print_instr(const x64::Instruction& instr, addr_t ip);

	void printDisassembly(addr_t rip)
	{
		serial::debugprintf("disassembly at %p:\n", (void*) rip);

		auto buf = Buffer((uint8_t*) rip, /* some arbitrary number: */ 0x1000);

		auto ip = rip;
		for(size_t i = 0; i < DISASM_COUNT; i++)
		{
			auto instr = x64::read(buf, x64::ExecMode::Long);
			serial::debugprintf("  %8x  |  ", (uint32_t) ip);
			print_instr(instr, ip);
			serial::debugprintf("\n");

			ip += instr.numBytes();
		}

		serial::debugprintf("\n");
	}


	static void print_instr(const x64::Instruction& instr, uint64_t ip)
	{
		auto printer = serial::debugprintf;

		auto print_operand = [&printer, &ip](const x64::Operand& op) {
			if(op.isRegister())
			{
				printer("%%%s", op.reg().name());
			}
			else if(op.isImmediate())
			{
				int64_t value = op.imm();
				if(op.immediateSize() == 8)
					value = (uint8_t) value;

				else if(op.immediateSize() == 16)
					value = (uint16_t) value;

				else if(op.immediateSize() == 32)
					value = (uint32_t) value;

				else if(op.immediateSize() == 64)
					value = (uint64_t) value;

				printer("$%#lx", value);
			}
			else if(op.isRelativeOffset())
			{
				printer("%#lx", ip + op.ofs().offset());
			}
			else if(op.isMemory())
			{
				auto& mem = op.mem();
				auto& base = mem.base();
				auto& idx = mem.index();

				if(mem.segment().present())
					printer("%%%s:", mem.segment().name());

				if(op.mem().displacement() != 0 || (!base.present() && !idx.present()))
					printer("%#lx", op.mem().displacement());

				if(!base.present() && !idx.present())
					return;

				printer("(");

				if(base.present())
					printer("%%%s", base.name());

				if(idx.present())
					printer(", %%%s", idx.name());

				if(op.mem().scale() != 1)
					printer(", %d", op.mem().scale());

				printer(")");
			}
			else
			{
				printer("??");
			}
		};

		// print the bytes
		size_t col = 0;
		for(size_t i = 0; i < instr.numBytes(); i++)
		{
			printer("%02x ", instr.bytes()[i]);
			col += 3;
		}

		while(col++ < 24)
			printer(" ");

		if(instr.lockPrefix())  printer("lock ");
		if(instr.repPrefix())   printer("rep ");
		if(instr.repnzPrefix()) printer("repnz ");

		char size_suffix = 0;
		bool abs_suffix = false;

		auto inspect = [&size_suffix, &abs_suffix](const x64::Operand& op) {

			if(size_suffix != 0)
				return;

			if(op.isImmediate())
			{
				switch(op.immediateSize())
				{
					case 8:  size_suffix = 'b'; break;
					case 16: size_suffix = 'w'; break;
					case 32: size_suffix = 'l'; break;
					case 64: size_suffix = 'q'; abs_suffix = true; break;

					default:
						break;
				}
			}
			else if(op.isMemory())
			{
				auto& mem = op.mem();

				switch(mem.bits())
				{
					case 8:  size_suffix = 'b'; break;
					case 16: size_suffix = 'w'; break;
					case 32: size_suffix = 'l'; break;
					case 64: size_suffix = 'q'; break;

					default:
						break;
				}

				if(mem.isDisplacement64Bits())
					abs_suffix = true;
			}
		};

		// ugh. this is really dumb, but we don't want to use nx::string.
		if(instr.operandCount() > 0)
			inspect(instr.dst());

		if(instr.operandCount() > 1)
			inspect(instr.src());


		printer("%s%s%c ", instr.op().mnemonic(), abs_suffix ? "abs" : "", size_suffix);

		if(instr.operandCount() == 1)
		{
			print_operand(instr.dst());
		}
		else if(instr.operandCount() == 2)
		{
			print_operand(instr.src()); printer(", ");
			print_operand(instr.dst());
		}
		else if(instr.operandCount() == 3)
		{
			print_operand(instr.ext()); printer(", ");
			print_operand(instr.src()); printer(", ");
			print_operand(instr.dst());
		}
		else if(instr.operandCount() == 4)
		{
			print_operand(instr.op4()); printer(", ");
			print_operand(instr.ext()); printer(", ");
			print_operand(instr.src()); printer(", ");
			print_operand(instr.dst());
		}
	}
}
