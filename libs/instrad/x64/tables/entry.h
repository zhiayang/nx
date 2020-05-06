// entry.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "../ops.h"

namespace instrad::x64
{
	enum class OpKind
	{
		Reg8,
		Reg16,
		Reg32,
		Reg64,

		RegMem8,
		RegMem16,
		RegMem32,
		RegMem64,

		Imm8,
		Imm16,
		Imm32,
		Imm64,          // the 0xB8 mov is the only instruction that uses a real 8-byte immediate

		SignExtImm8,
		SignExtImm32,

		Memory,         // only used by LEA
		SegmentReg,     // only used by MOV
		ControlReg,
		DebugReg,

		ImplicitCS,
		ImplicitDS,
		ImplicitES,
		ImplicitFS,
		ImplicitGS,
		ImplicitSS,

		ImplicitCL,
		ImplicitCX,
		ImplicitECX,

		ImplicitDX,

		ImplicitAL,
		ImplicitAX,
		ImplicitEAX,
		ImplicitRAX,

		Rel8Offset,
		Rel16Offset,
		Rel32Offset,

		MemoryOfs8,
		MemoryOfs16,
		MemoryOfs32,
		MemoryOfs64,

		Mem8,
		Mem16,
		Mem32,
		Mem64,
		Mem80,
		Mem128,
		Mem256,

		RegMmx,
		RegMmxMem32,
		RegMmxMem64,

		RegXmm,
		RegXmmMem8,
		RegXmmMem16,
		RegXmmMem32,
		RegXmmMem64,
		RegXmmMem128,

		RegYmm,
		RegYmmMem256,

		Reg32_vvvv,
		Reg64_vvvv,
		RegXmm_vvvv,
		RegYmm_vvvv,

		RegXmm_TrailingImm8HighNib,
		RegYmm_TrailingImm8HighNib,

		// dumbness
		Reg32Mem8,
		Reg32Mem16,

		// implicit memory operands, for movsb/cmpsb/stos/scas/etc.

		// these always use ES segment, you can't override
		ImplicitMem8_ES_EDI,
		ImplicitMem32_ES_EDI,

		// these can use any segment you override with
		ImplicitMem8_ESI,
		ImplicitMem32_ESI,

		ImplicitXMM0,

		ImplicitST0,
		ImplicitST1,
		RegX87_Rm,

		// instructions living in extension tables, but taking a register operand (and not a reg/mem operand),
		// cannot use the modRM.reg bits (for obvious reasons), so they encode the register operand in modRM.rm
		// instead. use this so the decoder knows where to look for the operands.
		// we also need to use this for some of the SSE instructions, because they take two register operands;
		// instead of one reg and one reg/mem (all the "normal" instructions so far adhere to that pattern).
		Reg8_Rm,
		Reg16_Rm,
		Reg32_Rm,
		Reg64_Rm,
		RegMmx_Rm,
		RegXmm_Rm,
		RegYmm_Rm,

		VSIB_Xmm32,
		VSIB_Xmm64,
		VSIB_Ymm32,
		VSIB_Ymm64,

		Ptr16_16,       // for far calls and jumps
		Ptr16_32,

		None,
	};


	struct TableEntry
	{
	private:
		using __Foozle = OpKind[4];

	public:
		constexpr bool present() const { return this->m_op != ops::INVALID; }

		constexpr const TableEntry* extension() const { return this->m_exttable; }
		constexpr const __Foozle& operands() const { return this->m_operands; }
		constexpr bool isDirectRegisterIdx() const { return this->m_lowNibbleRegIdx; }

		constexpr bool extensionUsesRMBits() const { return this->m_extensionUsesRMBits; }
		constexpr bool extensionUsesModBits() const { return this->m_extensionUsesModBits; }
		constexpr bool extensionUsesRexWBit() const { return this->m_extensionUsesRexWBit; }
		constexpr bool extensionUsesPrefixByte() const { return this->m_extensionUsesPrefixByte; }

		constexpr int numOperands() const { return this->m_numOperands; }
		constexpr uint8_t opcode() const { return this->m_opcode; }
		constexpr const Op& op() const { return this->m_op; }
		constexpr bool needsModRM() const { return this->m_needsModRM; }

	private:
		Op m_op = ops::INVALID;
		uint8_t m_opcode = 0;
		int m_numOperands = 0;
		bool m_lowNibbleRegIdx = false;
		OpKind m_operands[4] = { OpKind::None, OpKind::None, OpKind::None, OpKind::None };

		bool m_needsModRM = false;
		bool m_extensionUsesRMBits = false;
		bool m_extensionUsesModBits = false;
		bool m_extensionUsesRexWBit = false;
		bool m_extensionUsesPrefixByte = false;
		const TableEntry* m_exttable = nullptr;

		friend constexpr TableEntry entry_none(uint8_t);
		friend constexpr TableEntry entry_0(uint8_t, const Op&);
		friend constexpr TableEntry entry_0_modrm(uint8_t, const Op&);
		friend constexpr TableEntry entry_1(uint8_t, const Op&, OpKind);
		friend constexpr TableEntry entry_2(uint8_t, const Op&, OpKind, OpKind);
		friend constexpr TableEntry entry_3(uint8_t, const Op&, OpKind, OpKind, OpKind);
		friend constexpr TableEntry entry_4(uint8_t, const Op&, OpKind, OpKind, OpKind, OpKind);
		friend constexpr TableEntry entry_lnri_1(uint8_t, const Op&, OpKind);
		friend constexpr TableEntry entry_lnri_2(uint8_t, const Op&, OpKind, OpKind);
		friend constexpr TableEntry entry_ext(uint8_t, const TableEntry*);
		friend constexpr TableEntry entry_ext_rm(uint8_t, const TableEntry*);
		friend constexpr TableEntry entry_ext_mod(uint8_t, const TableEntry*);
		friend constexpr TableEntry entry_ext_rexw(uint8_t, const TableEntry*);
		friend constexpr TableEntry entry_ext_prefix(uint8_t, const TableEntry*);

		friend constexpr TableEntry entry_1_no_modrm(uint8_t, const Op&, OpKind);
		friend constexpr TableEntry entry_2_no_modrm(uint8_t, const Op&, OpKind, OpKind);
	};

	constexpr TableEntry entry_none(uint8_t opcode)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		return ret;
	}

	// entry_0 takes no modrm by default.
	constexpr TableEntry entry_0(uint8_t opcode, const Op& op)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 0;
		ret.m_needsModRM = false;
		return ret;
	}

	// used for stuff in extension tables; takes no operands but still needs modrm.
	constexpr TableEntry entry_0_modrm(uint8_t opcode, const Op& op)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 0;
		ret.m_needsModRM = true;
		return ret;
	}

	constexpr TableEntry entry_1(uint8_t opcode, const Op& op, OpKind dst)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 1;
		ret.m_operands[0] = dst;
		ret.m_needsModRM = true;
		return ret;
	}

	constexpr TableEntry entry_2(uint8_t opcode, const Op& op, OpKind dst, OpKind src)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 2;
		ret.m_operands[0] = dst;
		ret.m_operands[1] = src;
		ret.m_needsModRM = true;
		return ret;
	}

	constexpr TableEntry entry_3(uint8_t opcode, const Op& op, OpKind dst, OpKind src, OpKind extra)
	{
		// only used by imul
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 3;
		ret.m_needsModRM = true;
		ret.m_operands[0] = dst;
		ret.m_operands[1] = src;
		ret.m_operands[2] = extra;
		return ret;
	}

	constexpr TableEntry entry_4(uint8_t opcode, const Op& op, OpKind dst, OpKind src, OpKind extra, OpKind extra2)
	{
		// only used by imul
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 4;
		ret.m_needsModRM = true;
		ret.m_operands[0] = dst;
		ret.m_operands[1] = src;
		ret.m_operands[2] = extra;
		ret.m_operands[3] = extra2;
		return ret;
	}

	constexpr TableEntry entry_1_no_modrm(uint8_t opcode, const Op& op, OpKind dst)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 1;
		ret.m_operands[0] = dst;
		ret.m_needsModRM = false;
		return ret;
	}

	constexpr TableEntry entry_2_no_modrm(uint8_t opcode, const Op& op, OpKind dst, OpKind src)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 2;
		ret.m_operands[0] = dst;
		ret.m_operands[1] = src;
		ret.m_needsModRM = false;
		return ret;
	}

	// so far, there aren't any one-operand LNRI opcodes that need a modRM byte.
	constexpr TableEntry entry_lnri_1(uint8_t opcode, const Op& op, OpKind dst)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 1;
		ret.m_operands[0] = dst;
		ret.m_needsModRM = false;
		ret.m_lowNibbleRegIdx = true;
		return ret;
	}

	// so far, there aren't any two-operand LNRI opcodes that need a modRM byte either.
	constexpr TableEntry entry_lnri_2(uint8_t opcode, const Op& op, OpKind dst, OpKind src)
	{
		auto ret = TableEntry();
		ret.m_op = op;
		ret.m_opcode = opcode;
		ret.m_numOperands = 2;
		ret.m_operands[0] = dst;
		ret.m_operands[1] = src;
		ret.m_needsModRM = false;
		ret.m_lowNibbleRegIdx = true;
		return ret;
	}

	// this one is the "normal" modRM.reg extension
	constexpr TableEntry entry_ext(uint8_t opcode, const TableEntry* ext)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		ret.m_exttable = ext;
		return ret;
	}

	// this one is for modRM.rm extensions... ugh.
	constexpr TableEntry entry_ext_rm(uint8_t opcode, const TableEntry* ext)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		ret.m_exttable = ext;
		ret.m_extensionUsesRMBits = true;
		return ret;
	}

	// this one only uses the mod bits... ughhhhhh
	constexpr TableEntry entry_ext_mod(uint8_t opcode, const TableEntry* ext)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		ret.m_exttable = ext;
		ret.m_extensionUsesModBits = true;
		return ret;
	}

	// this one only uses the rex.w bit... ughhhhhhhhhhhhhhhhh
	constexpr TableEntry entry_ext_rexw(uint8_t opcode, const TableEntry* ext)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		ret.m_exttable = ext;
		ret.m_extensionUsesRexWBit = true;
		return ret;
	}

	// extend based on no prefix, 0x66, 0xF3, or 0xF2
	constexpr TableEntry entry_ext_prefix(uint8_t opcode, const TableEntry* ext)
	{
		auto ret = TableEntry();
		ret.m_opcode = opcode;
		ret.m_exttable = ext;
		ret.m_extensionUsesPrefixByte = true;
		return ret;
	}




	// all the entry_N functions imply that the instructions accept a modRM byte, except entry_0
	// (but there is an entry_0_modrm to specify that a 0-operand opcode needs modRM).
	// there is entry_1_no_modrm and entry_2_no_modrm to specify 1 and 2-operand instructions that
	// don't take a modRM byte.



	constexpr TableEntry entry_blank = entry_none(0);

	template <typename T, size_t N>
	constexpr size_t ArrayLength(const T (&arr)[N]) { return N; }
}
