// x64.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "regs.h"
#include "immediates.h"

namespace instrad::x64
{
	struct MemoryRef
	{
		constexpr MemoryRef() { }

		constexpr explicit MemoryRef(int bits, int64_t d) : m_bits(bits), m_displacement(d) { }
		constexpr explicit MemoryRef(int bits, const Register& b) : m_bits(bits), m_base(b) { }

		constexpr MemoryRef(int bits, const Register& b, int64_t d) : m_bits(bits), m_displacement(d), m_base(b) { }
		constexpr MemoryRef(int bits, const Register& b, const Register& i) : m_bits(bits), m_base(b), m_index(i) { }

		constexpr MemoryRef(int bits, const Register& b, const Register& i, int s, int64_t d)
			: m_bits(bits), m_scale(s), m_displacement(d), m_base(b), m_index(i) { }

		constexpr int bits() const { return this->m_bits; }
		constexpr int scale() const { return this->m_scale; }
		constexpr const Register& segment() const { return this->m_segment; }
		constexpr int64_t displacement() const { return this->m_displacement; }
		constexpr const Register& base() const { return this->m_base; }
		constexpr const Register& index() const { return this->m_index; }

		constexpr MemoryRef& setBits(int b) { this->m_bits = b; return *this; }
		constexpr MemoryRef& setScale(int s) { this->m_scale = s; return *this; }
		constexpr MemoryRef& setDisplacement(int64_t d) { this->m_displacement = d; return *this; }
		constexpr MemoryRef& setIndex(const Register& i) { this->m_index = i; return *this; }
		constexpr MemoryRef& setBase(const Register& b) { this->m_base = b; return *this; }
		constexpr MemoryRef& setSegment(const Register& b) { this->m_segment = b; return *this; }

	private:
		int m_bits = 0;
		Register m_segment = regs::NONE;

		// base + (index * scale) + displacement

		int m_scale = 1;
		int64_t m_displacement = 0;
		Register m_base = regs::NONE;
		Register m_index = regs::NONE;
	};

	struct Operand
	{
		constexpr Operand() { }
		constexpr Operand(const MemoryRef& mem) : m_type(TYPE_MEM), m_mem(mem) { }
		constexpr Operand(const Register& reg) : m_type(TYPE_REG), m_reg(reg) { }

		constexpr Operand(int64_t imm) : m_type(TYPE_IMM), m_imm(imm), m_immbits(64) { }
		constexpr Operand(int32_t imm) : m_type(TYPE_IMM), m_imm(imm), m_immbits(32) { }
		constexpr Operand(int16_t imm) : m_type(TYPE_IMM), m_imm(imm), m_immbits(16) { }
		constexpr Operand(int8_t imm) : m_type(TYPE_IMM), m_imm(imm), m_immbits(8) { }

		constexpr bool isImmediate() const { return this->m_type == TYPE_IMM; }
		constexpr bool isRegister() const { return this->m_type == TYPE_REG; }
		constexpr bool isMemory() const { return this->m_type == TYPE_MEM; }

		constexpr const MemoryRef& mem() const { return this->m_mem; }
		constexpr const Register& reg() const { return this->m_reg; }
		constexpr uint64_t imm() const { return this->m_imm; }

		constexpr MemoryRef& mem() { return this->m_mem; }
		constexpr Register& reg() { return this->m_reg; }

		constexpr int immediateSize() const { return this->m_immbits; }

	private:
		constexpr static int TYPE_REG = 0;
		constexpr static int TYPE_IMM = 1;
		constexpr static int TYPE_MEM = 2;

		int m_type = TYPE_IMM;
		union {
			Register m_reg;
			MemoryRef m_mem;
			int64_t m_imm = 0;
		};

		int m_immbits = 0;
	};

	struct RexPrefix
	{
		constexpr RexPrefix(uint8_t b) : byte(b) { }

		// don't use unions here, because constexpr is too weak
		constexpr uint8_t B() const { return (this->byte & 0x1) >> 0; }
		constexpr uint8_t X() const { return (this->byte & 0x2) >> 1; }
		constexpr uint8_t R() const { return (this->byte & 0x4) >> 2; }
		constexpr uint8_t W() const { return (this->byte & 0x8) >> 3; }

		constexpr bool present() const { return this->byte != 0; }
		constexpr static RexPrefix none() { return RexPrefix(0); }

	private:
		uint8_t byte;
	};

	struct ModRM
	{
		constexpr ModRM(uint8_t b) : byte(b) { }

		constexpr uint8_t rm()  const { return (this->byte & 0x07) >> 0; }
		constexpr uint8_t reg() const { return (this->byte & 0x38) >> 3; }
		constexpr uint8_t mod() const { return (this->byte & 0xC0) >> 6; }

		constexpr bool present() const { return this->byte != 0; }
		constexpr static ModRM none() { return ModRM(0); }

	private:
		uint8_t byte;
	};

	struct VexPrefix
	{
		// note that we put the second byte in the two-byte form in byte2 instead of byte1
		// because it has the vvvv/L/pp fields, so it's less work.
		constexpr VexPrefix(uint8_t b2) : prefix(0xC5), byte1(0), byte2(b2) { }
		constexpr VexPrefix(uint8_t b1, uint8_t b2) : prefix(0xC4), byte1(b1), byte2(b2) { }

		constexpr uint8_t R() const
		{
			return this->prefix == 0xC5
				? ((~this->byte2 & 0x80) >> 7)
				: ((~this->byte1 & 0x80) >> 7);
		}

		// the X B and W bits are inverted in the bitstream, but we invert them back.
		// for 2-byte opcode form, the implied values of X', B', and W' are 1, 1, and 0;
		// this gives us X=0, B=0, W=1.
		constexpr uint8_t X() const     { return this->prefix == 0xC5 ? 0x0 : ((~this->byte1 & 0x40) >> 6); }
		constexpr uint8_t B() const     { return this->prefix == 0xC5 ? 0x0 : ((~this->byte1 & 0x20) >> 5); }
		constexpr uint8_t W() const     { return this->prefix == 0xC5 ? 0x1 : ((this->byte2 & 0x80) >> 7); }
		constexpr uint8_t map() const   { return this->prefix == 0xC5 ? 0x1 : this->byte1 & 0x1F; }
		constexpr uint8_t vvvv() const  { return (~this->byte2 & 0x78) >> 3; }
		constexpr uint8_t L() const     { return (this->byte2 & 0x4) >> 2; }
		constexpr uint8_t pp() const    { return (this->byte2 & 0x3) >> 0; }

		constexpr bool prefix66() const { return this->pp() == 1; }
		constexpr bool prefixF3() const { return this->pp() == 2; }
		constexpr bool prefixF2() const { return this->pp() == 3; }

		constexpr bool present() const { return this->prefix != 0; }
		constexpr static VexPrefix none() { auto ret = VexPrefix(0); ret.prefix = 0; return ret; }

	private:
		uint8_t prefix;
		uint8_t byte1;
		uint8_t byte2;
	};

	struct InstrModifiers
	{
		uint8_t opcode = 0;

		ModRM modrm = ModRM::none();
		RexPrefix rex = RexPrefix::none();
		VexPrefix vex = VexPrefix::none();

		bool addressSizeOverride = false;
		bool operandSizeOverride = false;

		// if true, we are in 16-bit mode; if false, then 32/64-bit mode.
		bool legacyAddressingMode = false;

		// if true, we are in 32-bit mode; if false, then 64-bit mode.
		bool compatibilityMode = false;

		// low nibble register index.
		bool directRegisterIndex = false;

		static constexpr int SEG_NONE = 0;
		static constexpr int SEG_CS = 1;
		static constexpr int SEG_DS = 2;
		static constexpr int SEG_ES = 3;
		static constexpr int SEG_FS = 4;
		static constexpr int SEG_GS = 5;
		static constexpr int SEG_SS = 6;

		int segmentOverride = SEG_NONE;

		bool lockPrefix = false;
		bool repPrefix = false;
		bool repnzPrefix = false;
	};

	static_assert(sizeof(ModRM) == 1);
	static_assert(sizeof(RexPrefix) == 1);
}
