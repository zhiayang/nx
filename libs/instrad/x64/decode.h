// decode.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "ops.h"
#include "regs.h"
#include "table.h"
#include "operands.h"

namespace instrad::x64
{
	struct Instruction
	{
		constexpr Instruction(Op op) : m_op(op) { }

		constexpr void setDst(const Operand& x) { this->m_dst = x; this->m_operandCount = 1; }
		constexpr void setSrc(const Operand& x) { this->m_src = x; this->m_operandCount = 2; }
		constexpr void setExt(const Operand& x) { this->m_ext = x; this->m_operandCount = 3; }
		constexpr void setOp4(const Operand& x) { this->m_op4 = x; this->m_operandCount = 4; }
		constexpr void setBytes(const uint8_t* bytes, size_t n) { this->m_bytes = bytes; this->m_byteCount = n; }

		constexpr void addLockPrefix() { this->m_prefixLock = true; }
		constexpr void addRepPrefix() { this->m_prefixRep = true; }
		constexpr void addRepNZPrefix() { this->m_prefixRepnz = true; }

		constexpr bool lockPrefix() const { return this->m_prefixLock; }
		constexpr bool repPrefix() const { return this->m_prefixRep; }
		constexpr bool repnzPrefix() const { return this->m_prefixRepnz; }


		constexpr const Operand& src() const { return this->m_src; }
		constexpr const Operand& dst() const { return this->m_dst; }
		constexpr const Operand& ext() const { return this->m_ext; }
		constexpr const Operand& op4() const { return this->m_op4; }
		constexpr const Op& op() const { return this->m_op; }

		constexpr int operandCount() const { return this->m_operandCount; }

		constexpr const uint8_t* bytes() const { return this->m_bytes; }
		constexpr size_t numBytes() const { return this->m_byteCount; }

	private:
		Op m_op;
		int m_operandCount = 0;

		bool m_prefixLock  = false;
		bool m_prefixRep   = false;
		bool m_prefixRepnz = false;

		// in "mov rax, $1", rax=dst, $1=src; ie. "mov $1, %rax"
		// for single-op instructions, eg. inc %rax, it is stored in dst.

		Operand m_dst = { };
		Operand m_src = { };
		Operand m_ext = { };
		Operand m_op4 = { };

		const uint8_t* m_bytes = 0;
		size_t m_byteCount = 0;
	};

	constexpr Instruction decode(Buffer& xs, InstrModifiers& mods, const TableEntry* table)
	{
		// check the main table first
		mods.opcode = xs.pop();
		auto entry = table[mods.opcode];

		// make a potential modRM
		bool usedModRM = false;

		// chase down the tables.
		while(!entry.present())
		{
			auto ext = entry.extension();
			if(ext == nullptr)
				break;

			usedModRM = true;
			auto modrm = ModRM(xs.peek());

			// see what kind
			if(entry.extensionUsesRMBits() || entry.extensionUsesModBits())
			{
				// ugh. we *already* indexed by modRM.reg, so we only need to index by modRM.rm now.
				// BUT, only if modRM.mod == 3; if modRM.mod != 3, then we only use the first entry.
				// there's a second class of entries that only index by modRM.mod, and disregards modRM.rm;
				// check for that as well.

				if(modrm.mod() != 3)
				{
					entry = ext[0];
				}
				else
				{
					if(entry.extensionUsesRMBits())
						entry = ext[1 + modrm.rm()];

					else
						entry = ext[1];
				}
			}
			else if(entry.extensionUsesRexWBit())
			{
				// if !rex.W, then use the first entry; if rex.W, use the second.
				if(mods.rex.W())    entry = ext[1];
				else                entry = ext[0];
			}
			else if(entry.extensionUsesPrefixByte())
			{
				if(mods.operandSizeOverride)    entry = ext[1];     // 0x66
				else if(mods.repnzPrefix)       entry = ext[2];     // 0xF2
				else if(mods.repPrefix)         entry = ext[3];     // 0xF3
				else                            entry = ext[0];     // none
			}
			else
			{
				entry = ext[modrm.reg()];
			}
		}

		// entry is not present; cry.
		if(!entry.present())
			return Instruction(ops::INVALID);

		mods.directRegisterIndex = entry.isDirectRegisterIdx();

		// if we need a modrm, then consume it for real.
		if(usedModRM || entry.needsModRM())
			mods.modrm = ModRM(xs.pop());

		// handle NOP specially.
		if(mods.opcode == 0x90)
		{
			// don't forget pause.
			if(mods.repPrefix)  return Instruction(ops::PAUSE);
			else                return Instruction(ops::NOP);
		}
		else
		{
			auto ret = Instruction(entry.op());
			if(entry.numOperands() > 0)
				ret.setDst(getOperand(xs, entry.operands()[0], mods));

			if(entry.numOperands() > 1)
				ret.setSrc(getOperand(xs, entry.operands()[1], mods));

			if(entry.numOperands() > 2)
				ret.setExt(getOperand(xs, entry.operands()[2], mods));

			return ret;
		}
	}

	constexpr Instruction decode_3dnow(Buffer& buf, InstrModifiers& mods)
	{
		// from the AMD manuals, it is evident that all the 3dnow instructions
		// take a modRM byte. so, we can parse that unconditionally:
		mods.modrm = ModRM(buf.pop());

		// furthermore, they all have the form OP mmx, mmx/mem64
		auto op1 = getOperand(buf, OpKind::RegMmx, mods);
		auto op2 = getOperand(buf, OpKind::RegMmxMem64, mods);

		// now, we should have the opcode at our disposal.
		auto opcode = buf.pop();

		auto instr = Instruction(tables::SecondaryOpcodeMap_0F_0F_3DNow[opcode].op());
		instr.setDst(op1);
		instr.setSrc(op2);

		return instr;
	}

	constexpr Instruction decode_VEX(Buffer& buf, InstrModifiers& mods)
	{
		// make a fake REX using the bits in the VEX. this ensures that all of our
		// old code continues to be able to work (and resolve register indices etc.)
		// for safety reasons we cannot modify the bits in the REX directly, so construct
		// a byte using the VEX bits.
		uint8_t rexByte = 0x40 | (mods.vex.B() << 0) | (mods.vex.X() << 1) | (mods.vex.R() << 2) | (mods.vex.W() << 3);
		mods.rex = RexPrefix(rexByte);

		// now, we have the opcode.
		auto opcode = buf.pop();

		// get the correct map:
		const VexEntry* map = nullptr;

		switch(mods.vex.map())
		{
			case 1:     map = tables::VEX_Map_1; break;
			case 2:     map = tables::VEX_Map_2; break;
			case 3:     map = tables::VEX_Map_3; break;
			default:    return Instruction(ops::INVALID);
		}

		// vex has less extension shenanigans
		bool usedModRM = false;
		auto vexEntry = map[opcode];
		while(!vexEntry.present())
		{
			auto ext = vexEntry.extension();
			if(ext == nullptr)
				return Instruction(ops::INVALID);

			usedModRM = true;
			auto modrm = ModRM(buf.peek());

			vexEntry = ext[modrm.reg()];
		}

		if(!vexEntry.present())
			return Instruction(ops::INVALID);

		if(usedModRM || vexEntry.needsModRM())
			mods.modrm = ModRM(buf.pop());

		auto decoder = VexEntryDecoder(vexEntry);
		decoder.setModNot3();

		if(mods.modrm.mod() == 3)                           decoder.setMod3();
		if(mods.vex.W())                                    decoder.setVexW();
		if(mods.vex.L())                                    decoder.setVexL();
		if(mods.operandSizeOverride || mods.vex.pp() == 1)  decoder.setPref66();
		if(mods.repPrefix || mods.vex.pp() == 2)            decoder.setPrefF3();
		if(mods.repnzPrefix || mods.vex.pp() == 3)          decoder.setPrefF2();

		auto entry = decoder.get();

		auto ret = Instruction(entry.op());

		if(entry.numOperands() > 0)
			ret.setDst(getOperand(buf, entry.operands()[0], mods));

		if(entry.numOperands() > 1)
			ret.setSrc(getOperand(buf, entry.operands()[1], mods));

		if(entry.numOperands() > 2)
			ret.setExt(getOperand(buf, entry.operands()[2], mods));

		if(entry.numOperands() > 3)
			ret.setOp4(getOperand(buf, entry.operands()[3], mods));

		return ret;
	}





	enum class ExecMode
	{
		Legacy,
		Compat,
		Long
	};

	// this is mostly a state machine; see figure 1-1 in the AMD manual, volume 3.
	constexpr Instruction read(Buffer& xs, ExecMode mode)
	{
		auto begin = xs.ptr();

		auto modifiers = InstrModifiers();
		switch(mode)
		{
			case ExecMode::Legacy:
				modifiers.legacyAddressingMode = true;
				break;

			case ExecMode::Compat:
				modifiers.compatibilityMode = true;
				break;

			case ExecMode::Long:
				break;
		}

		// first, check for legacy prefixes
		while(true)
		{
			if(xs.match(0x66))      modifiers.operandSizeOverride = true;
			else if(xs.match(0x67)) modifiers.addressSizeOverride = true;
			else if(xs.match(0x2E)) modifiers.segmentOverride = InstrModifiers::SEG_CS;
			else if(xs.match(0x3E)) modifiers.segmentOverride = InstrModifiers::SEG_DS;
			else if(xs.match(0x26)) modifiers.segmentOverride = InstrModifiers::SEG_ES;
			else if(xs.match(0x64)) modifiers.segmentOverride = InstrModifiers::SEG_FS;
			else if(xs.match(0x65)) modifiers.segmentOverride = InstrModifiers::SEG_GS;
			else if(xs.match(0x36)) modifiers.segmentOverride = InstrModifiers::SEG_SS;
			else if(xs.match(0xF0)) modifiers.lockPrefix = true;
			else if(xs.match(0xF3)) modifiers.repPrefix = true, modifiers.repnzPrefix = false;
			else if(xs.match(0xF2)) modifiers.repnzPrefix = true, modifiers.repPrefix = false;
			else if(xs.match(0xC4)) modifiers.vex = VexPrefix(xs.pop(), xs.pop());
			else if(xs.match(0xC5)) modifiers.vex = VexPrefix(xs.pop());
			else                    break;
		}

		// next, REX prefix.
		if(mode == ExecMode::Long && (xs.peek() & 0xF0) == 0x40)
			modifiers.rex = RexPrefix(xs.pop());

		auto table = tables::PrimaryOpcodeMap;
		bool is3dnow = false;

		// next, check for escape
		if(xs.match(0x0F))
		{
			if(xs.match(0x0F))          is3dnow = true;
			else if(xs.match(0x38))     table = tables::SecondaryOpcodeMap_0F_38;
			else if(xs.match(0x3A))     table = tables::SecondaryOpcodeMap_0F_3A;
			else                        table = tables::SecondaryOpcodeMap_0F;
		}

		auto ret = [&]() -> auto {
			if(modifiers.vex.present()) return decode_VEX(xs, modifiers);
			if(is3dnow)                 return decode_3dnow(xs, modifiers);
			else                        return decode(xs, modifiers, table);
		}();

		auto len = xs.ptr() - begin;

		ret.setBytes(begin, len);

		if(modifiers.lockPrefix)    ret.addLockPrefix();
		if(modifiers.repPrefix)     ret.addRepPrefix();
		if(modifiers.repnzPrefix)   ret.addRepNZPrefix();

		return ret;
	}
}
