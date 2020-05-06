// primary.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "entry.h"
#include "x87.h"

namespace instrad::x64::tables
{
	// the extension tables are indexed by the ModRM.REG value.
	constexpr TableEntry ModRMExt_80[] = {
		/*0*/ entry_2(0x80, ops::ADD, OpKind::RegMem8, OpKind::Imm8),
		/*1*/ entry_2(0x80, ops::OR,  OpKind::RegMem8, OpKind::Imm8),
		/*2*/ entry_2(0x80, ops::ADC, OpKind::RegMem8, OpKind::Imm8),
		/*3*/ entry_2(0x80, ops::SBB, OpKind::RegMem8, OpKind::Imm8),
		/*4*/ entry_2(0x80, ops::AND, OpKind::RegMem8, OpKind::Imm8),
		/*5*/ entry_2(0x80, ops::SUB, OpKind::RegMem8, OpKind::Imm8),
		/*6*/ entry_2(0x80, ops::XOR, OpKind::RegMem8, OpKind::Imm8),
		/*7*/ entry_2(0x80, ops::CMP, OpKind::RegMem8, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_81[] = {
		/*0*/ entry_2(0x81, ops::ADD, OpKind::RegMem32, OpKind::Imm32),
		/*1*/ entry_2(0x81, ops::OR,  OpKind::RegMem32, OpKind::Imm32),
		/*2*/ entry_2(0x81, ops::ADC, OpKind::RegMem32, OpKind::Imm32),
		/*3*/ entry_2(0x81, ops::SBB, OpKind::RegMem32, OpKind::Imm32),
		/*4*/ entry_2(0x81, ops::AND, OpKind::RegMem32, OpKind::Imm32),
		/*5*/ entry_2(0x81, ops::SUB, OpKind::RegMem32, OpKind::Imm32),
		/*6*/ entry_2(0x81, ops::XOR, OpKind::RegMem32, OpKind::Imm32),
		/*7*/ entry_2(0x81, ops::CMP, OpKind::RegMem32, OpKind::Imm32),
	};

	// these are apparently invalid in 64-bit mode.
	constexpr TableEntry ModRMExt_82[] = {
		/*0*/ entry_2(0x82, ops::ADD, OpKind::RegMem8, OpKind::Imm8),
		/*1*/ entry_2(0x82, ops::OR,  OpKind::RegMem8, OpKind::Imm8),
		/*2*/ entry_2(0x82, ops::ADC, OpKind::RegMem8, OpKind::Imm8),
		/*3*/ entry_2(0x82, ops::SBB, OpKind::RegMem8, OpKind::Imm8),
		/*4*/ entry_2(0x82, ops::AND, OpKind::RegMem8, OpKind::Imm8),
		/*5*/ entry_2(0x82, ops::SUB, OpKind::RegMem8, OpKind::Imm8),
		/*6*/ entry_2(0x82, ops::XOR, OpKind::RegMem8, OpKind::Imm8),
		/*7*/ entry_2(0x82, ops::CMP, OpKind::RegMem8, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_83[] = {
		/*0*/ entry_2(0x83, ops::ADD, OpKind::RegMem32, OpKind::SignExtImm8),
		/*1*/ entry_2(0x83, ops::OR,  OpKind::RegMem32, OpKind::SignExtImm8),
		/*2*/ entry_2(0x83, ops::ADC, OpKind::RegMem32, OpKind::SignExtImm8),
		/*3*/ entry_2(0x83, ops::SBB, OpKind::RegMem32, OpKind::SignExtImm8),
		/*4*/ entry_2(0x83, ops::AND, OpKind::RegMem32, OpKind::SignExtImm8),
		/*5*/ entry_2(0x83, ops::SUB, OpKind::RegMem32, OpKind::SignExtImm8),
		/*6*/ entry_2(0x83, ops::XOR, OpKind::RegMem32, OpKind::SignExtImm8),
		/*7*/ entry_2(0x83, ops::CMP, OpKind::RegMem32, OpKind::SignExtImm8),
	};

	constexpr TableEntry ModRMExt_8F[] = {
		/*0*/ entry_1(0x8F, ops::POP, OpKind::RegMem64),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_C0[] = {
		/*0*/ entry_2(0xC0, ops::ROL, OpKind::RegMem8, OpKind::Imm8),
		/*1*/ entry_2(0xC0, ops::ROR, OpKind::RegMem8, OpKind::Imm8),
		/*2*/ entry_2(0xC0, ops::RCL, OpKind::RegMem8, OpKind::Imm8),
		/*3*/ entry_2(0xC0, ops::RCR, OpKind::RegMem8, OpKind::Imm8),
		/*4*/ entry_2(0xC0, ops::SHL, OpKind::RegMem8, OpKind::Imm8),
		/*5*/ entry_2(0xC0, ops::SHR, OpKind::RegMem8, OpKind::Imm8),
		/*6*/ entry_2(0xC0, ops::SAL, OpKind::RegMem8, OpKind::Imm8),
		/*7*/ entry_2(0xC0, ops::SAR, OpKind::RegMem8, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_C1[] = {
		/*0*/ entry_2(0xC1, ops::ROL, OpKind::RegMem32, OpKind::Imm8),
		/*1*/ entry_2(0xC1, ops::ROR, OpKind::RegMem32, OpKind::Imm8),
		/*2*/ entry_2(0xC1, ops::RCL, OpKind::RegMem32, OpKind::Imm8),
		/*3*/ entry_2(0xC1, ops::RCR, OpKind::RegMem32, OpKind::Imm8),
		/*4*/ entry_2(0xC1, ops::SHL, OpKind::RegMem32, OpKind::Imm8),
		/*5*/ entry_2(0xC1, ops::SHR, OpKind::RegMem32, OpKind::Imm8),
		/*6*/ entry_2(0xC1, ops::SAL, OpKind::RegMem32, OpKind::Imm8),
		/*7*/ entry_2(0xC1, ops::SAR, OpKind::RegMem32, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_C6[] = {
		/*0*/ entry_2(0xC6, ops::MOV, OpKind::RegMem8, OpKind::Imm8),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_C7[] = {
		/*0*/ entry_2(0xC7, ops::MOV, OpKind::RegMem32, OpKind::Imm32),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_D0[] = {
		/*0*/ entry_1(0xD0, ops::ROL, OpKind::RegMem8),
		/*1*/ entry_1(0xD0, ops::ROR, OpKind::RegMem8),
		/*2*/ entry_1(0xD0, ops::RCL, OpKind::RegMem8),
		/*3*/ entry_1(0xD0, ops::RCR, OpKind::RegMem8),
		/*4*/ entry_1(0xD0, ops::SHL, OpKind::RegMem8),
		/*5*/ entry_1(0xD0, ops::SHR, OpKind::RegMem8),
		/*6*/ entry_1(0xD0, ops::SAL, OpKind::RegMem8),
		/*7*/ entry_1(0xD0, ops::SAR, OpKind::RegMem8),
	};

	constexpr TableEntry ModRMExt_D1[] = {
		/*0*/ entry_1(0xD1, ops::ROL, OpKind::RegMem32),
		/*1*/ entry_1(0xD1, ops::ROR, OpKind::RegMem32),
		/*2*/ entry_1(0xD1, ops::RCL, OpKind::RegMem32),
		/*3*/ entry_1(0xD1, ops::RCR, OpKind::RegMem32),
		/*4*/ entry_1(0xD1, ops::SHL, OpKind::RegMem32),
		/*5*/ entry_1(0xD1, ops::SHR, OpKind::RegMem32),
		/*6*/ entry_1(0xD1, ops::SAL, OpKind::RegMem32),
		/*7*/ entry_1(0xD1, ops::SAR, OpKind::RegMem32),
	};

	constexpr TableEntry ModRMExt_D2[] = {
		/*0*/ entry_2(0xD2, ops::ROL, OpKind::RegMem8, OpKind::ImplicitCL),
		/*1*/ entry_2(0xD2, ops::ROR, OpKind::RegMem8, OpKind::ImplicitCL),
		/*2*/ entry_2(0xD2, ops::RCL, OpKind::RegMem8, OpKind::ImplicitCL),
		/*3*/ entry_2(0xD2, ops::RCR, OpKind::RegMem8, OpKind::ImplicitCL),
		/*4*/ entry_2(0xD2, ops::SHL, OpKind::RegMem8, OpKind::ImplicitCL),
		/*5*/ entry_2(0xD2, ops::SHR, OpKind::RegMem8, OpKind::ImplicitCL),
		/*6*/ entry_2(0xD2, ops::SAL, OpKind::RegMem8, OpKind::ImplicitCL),
		/*7*/ entry_2(0xD2, ops::SAR, OpKind::RegMem8, OpKind::ImplicitCL),
	};

	constexpr TableEntry ModRMExt_D3[] = {
		/*0*/ entry_2(0xD3, ops::ROL, OpKind::RegMem32, OpKind::ImplicitCL),
		/*1*/ entry_2(0xD3, ops::ROR, OpKind::RegMem32, OpKind::ImplicitCL),
		/*2*/ entry_2(0xD3, ops::RCL, OpKind::RegMem32, OpKind::ImplicitCL),
		/*3*/ entry_2(0xD3, ops::RCR, OpKind::RegMem32, OpKind::ImplicitCL),
		/*4*/ entry_2(0xD3, ops::SHL, OpKind::RegMem32, OpKind::ImplicitCL),
		/*5*/ entry_2(0xD3, ops::SHR, OpKind::RegMem32, OpKind::ImplicitCL),
		/*6*/ entry_2(0xD3, ops::SAL, OpKind::RegMem32, OpKind::ImplicitCL),
		/*7*/ entry_2(0xD3, ops::SAR, OpKind::RegMem32, OpKind::ImplicitCL),
	};


	constexpr TableEntry ModRMExt_F6[] = {
		/*0*/ entry_2(0xF6, ops::TEST, OpKind::RegMem8, OpKind::Imm8),
		/*1*/ entry_2(0xF6, ops::TEST, OpKind::RegMem8, OpKind::Imm8),
		/*2*/ entry_1(0xF6, ops::NOT,  OpKind::RegMem8),
		/*3*/ entry_1(0xF6, ops::NEG,  OpKind::RegMem8),
		/*4*/ entry_1(0xF6, ops::MUL,  OpKind::RegMem8),
		/*5*/ entry_1(0xF6, ops::IMUL, OpKind::RegMem8),
		/*6*/ entry_1(0xF6, ops::DIV,  OpKind::RegMem8),
		/*7*/ entry_1(0xF6, ops::IDIV, OpKind::RegMem8),
	};

	constexpr TableEntry ModRMExt_F7[] = {
		/*0*/ entry_2(0xF7, ops::TEST, OpKind::RegMem32, OpKind::Imm32),
		/*1*/ entry_2(0xF7, ops::TEST, OpKind::RegMem32, OpKind::Imm32),
		/*2*/ entry_1(0xF7, ops::NOT,  OpKind::RegMem32),
		/*3*/ entry_1(0xF7, ops::NEG,  OpKind::RegMem32),
		/*4*/ entry_1(0xF7, ops::MUL,  OpKind::RegMem32),
		/*5*/ entry_1(0xF7, ops::IMUL, OpKind::RegMem32),
		/*6*/ entry_1(0xF7, ops::DIV,  OpKind::RegMem32),
		/*7*/ entry_1(0xF7, ops::IDIV, OpKind::RegMem32),
	};

	constexpr TableEntry ModRMExt_FE[] = {
		/*0*/ entry_1(0xFE, ops::INC, OpKind::RegMem8),
		/*1*/ entry_1(0xFE, ops::DEC, OpKind::RegMem8),

		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_FF[] = {
		/*0*/ entry_1(0xFF, ops::INC,  OpKind::RegMem32),
		/*1*/ entry_1(0xFF, ops::DEC,  OpKind::RegMem32),
		/*2*/ entry_1(0xFF, ops::CALL, OpKind::RegMem64),
		/*3*/ entry_1(0xFF, ops::CALL, OpKind::Ptr16_32),
		/*4*/ entry_1(0xFF, ops::JMP,  OpKind::RegMem64),
		/*5*/ entry_1(0xFF, ops::JMP,  OpKind::Ptr16_32),
		/*6*/ entry_1(0xFF, ops::PUSH, OpKind::RegMem64),

		/*7*/ entry_blank,
	};



	constexpr TableEntry PrimaryOpcodeMap[] = {
		/*00*/ entry_2(0x00, ops::ADD,  OpKind::RegMem8, OpKind::Reg8),
		/*01*/ entry_2(0x01, ops::ADD,  OpKind::RegMem32, OpKind::Reg32),
		/*02*/ entry_2(0x02, ops::ADD,  OpKind::Reg8, OpKind::RegMem8),
		/*03*/ entry_2(0x03, ops::ADD,  OpKind::Reg32, OpKind::RegMem32),
		/*04*/ entry_2_no_modrm(0x04, ops::ADD,  OpKind::ImplicitAL, OpKind::Imm8),
		/*05*/ entry_2_no_modrm(0x05, ops::ADD,  OpKind::ImplicitEAX, OpKind::Imm32),
		/*06*/ entry_1_no_modrm(0x06, ops::PUSH, OpKind::ImplicitES),     // push es; invalid in 64-bit
		/*07*/ entry_1_no_modrm(0x07, ops::POP,  OpKind::ImplicitES),     // pop es; invalid in 64-bit

		/*08*/ entry_2(0x08, ops::OR, OpKind::RegMem8, OpKind::Reg8),
		/*09*/ entry_2(0x09, ops::OR, OpKind::RegMem32, OpKind::Reg32),
		/*0A*/ entry_2(0x0A, ops::OR, OpKind::Reg8, OpKind::RegMem8),
		/*0B*/ entry_2(0x0B, ops::OR, OpKind::Reg32, OpKind::RegMem32),
		/*0C*/ entry_2_no_modrm(0x0C, ops::OR,   OpKind::ImplicitAL, OpKind::Imm8),
		/*0D*/ entry_2_no_modrm(0x0D, ops::OR,   OpKind::ImplicitEAX, OpKind::Imm32),
		/*0E*/ entry_1_no_modrm(0x0E, ops::PUSH, OpKind::ImplicitCS),     // push cs; invalid in 64-bit
		/*0F*/ entry_blank,

		/*10*/ entry_2(0x10, ops::ADC, OpKind::RegMem8, OpKind::Reg8),
		/*11*/ entry_2(0x11, ops::ADC, OpKind::RegMem32, OpKind::Reg32),
		/*12*/ entry_2(0x12, ops::ADC, OpKind::Reg8, OpKind::RegMem8),
		/*13*/ entry_2(0x13, ops::ADC, OpKind::Reg32, OpKind::RegMem32),
		/*14*/ entry_2_no_modrm(0x14, ops::ADC,  OpKind::ImplicitAL, OpKind::Imm8),
		/*15*/ entry_2_no_modrm(0x15, ops::ADC,  OpKind::ImplicitEAX, OpKind::Imm32),
		/*16*/ entry_1_no_modrm(0x16, ops::PUSH, OpKind::ImplicitSS),     // push ss; invalid in 64-bit
		/*17*/ entry_1_no_modrm(0x17, ops::POP,  OpKind::ImplicitSS),     // pop ss; invalid in 64-bit

		/*18*/ entry_2(0x18, ops::SBB, OpKind::RegMem8, OpKind::Reg8),
		/*19*/ entry_2(0x19, ops::SBB, OpKind::RegMem32, OpKind::Reg32),
		/*1A*/ entry_2(0x1A, ops::SBB, OpKind::Reg8, OpKind::RegMem8),
		/*1B*/ entry_2(0x1B, ops::SBB, OpKind::Reg32, OpKind::RegMem32),
		/*1C*/ entry_2_no_modrm(0x1C, ops::SBB,  OpKind::ImplicitAL, OpKind::Imm8),
		/*1D*/ entry_2_no_modrm(0x1D, ops::SBB,  OpKind::ImplicitEAX, OpKind::Imm32),
		/*1E*/ entry_1_no_modrm(0x1E, ops::PUSH, OpKind::ImplicitDS),     // push ds; invalid in 64-bit
		/*1F*/ entry_1_no_modrm(0x1F, ops::POP,  OpKind::ImplicitDS),     // pop ds; invalid in 64-bit

		/*20*/ entry_2(0x20, ops::AND, OpKind::RegMem8, OpKind::Reg8),
		/*21*/ entry_2(0x21, ops::AND, OpKind::RegMem32, OpKind::Reg32),
		/*22*/ entry_2(0x22, ops::AND, OpKind::Reg8, OpKind::RegMem8),
		/*23*/ entry_2(0x23, ops::AND, OpKind::Reg32, OpKind::RegMem32),
		/*24*/ entry_2_no_modrm(0x24, ops::AND, OpKind::ImplicitAL, OpKind::Imm8),
		/*25*/ entry_2_no_modrm(0x25, ops::AND, OpKind::ImplicitEAX, OpKind::Imm32),
		/*26*/ entry_blank,                                               // ES segment override
		/*27*/ entry_0(0x27, ops::DAA),                                   // invalid in 64-bit mode

		/*28*/ entry_2(0x28, ops::SUB, OpKind::RegMem8, OpKind::Reg8),
		/*29*/ entry_2(0x29, ops::SUB, OpKind::RegMem32, OpKind::Reg32),
		/*2A*/ entry_2(0x2A, ops::SUB, OpKind::Reg8, OpKind::RegMem8),
		/*2B*/ entry_2(0x2B, ops::SUB, OpKind::Reg32, OpKind::RegMem32),
		/*2C*/ entry_2_no_modrm(0x2C, ops::SUB, OpKind::ImplicitAL, OpKind::Imm8),
		/*2D*/ entry_2_no_modrm(0x2D, ops::SUB, OpKind::ImplicitEAX, OpKind::Imm32),
		/*2E*/ entry_blank,                                               // CS segment override
		/*2F*/ entry_0(0x2F, ops::DAS),                                   // invalid in 64-bit mode

		/*30*/ entry_2(0x30, ops::XOR, OpKind::RegMem8, OpKind::Reg8),
		/*31*/ entry_2(0x31, ops::XOR, OpKind::RegMem32, OpKind::Reg32),
		/*32*/ entry_2(0x32, ops::XOR, OpKind::Reg8, OpKind::RegMem8),
		/*33*/ entry_2(0x33, ops::XOR, OpKind::Reg32, OpKind::RegMem32),
		/*34*/ entry_2_no_modrm(0x34, ops::XOR, OpKind::ImplicitAL, OpKind::Imm8),
		/*35*/ entry_2_no_modrm(0x35, ops::XOR, OpKind::ImplicitEAX, OpKind::Imm32),
		/*36*/ entry_blank,                                               // SS segment override
		/*37*/ entry_0(0x37, ops::AAA),                                   // invalid in 64-bit mode

		/*38*/ entry_2(0x38, ops::CMP, OpKind::RegMem8, OpKind::Reg8),
		/*39*/ entry_2(0x39, ops::CMP, OpKind::RegMem32, OpKind::Reg32),
		/*3A*/ entry_2(0x3A, ops::CMP, OpKind::Reg8, OpKind::RegMem8),
		/*3B*/ entry_2(0x3B, ops::CMP, OpKind::Reg32, OpKind::RegMem32),
		/*3C*/ entry_2_no_modrm(0x3C, ops::CMP, OpKind::ImplicitAL, OpKind::Imm8),
		/*3D*/ entry_2_no_modrm(0x3D, ops::CMP, OpKind::ImplicitEAX, OpKind::Imm32),
		/*3E*/ entry_blank,                                               // DS segment override
		/*3F*/ entry_0(0x3F, ops::AAS),                                   // invalid in 64-bit mode

		// in 64-bit mode, this is the REX prefix. but we want to support x86 also, so these are the inc/dec opcodes
		/*40*/ entry_lnri_1(0x40, ops::INC, OpKind::Reg32),
		/*41*/ entry_lnri_1(0x41, ops::INC, OpKind::Reg32),
		/*42*/ entry_lnri_1(0x42, ops::INC, OpKind::Reg32),
		/*43*/ entry_lnri_1(0x43, ops::INC, OpKind::Reg32),
		/*44*/ entry_lnri_1(0x44, ops::INC, OpKind::Reg32),
		/*45*/ entry_lnri_1(0x45, ops::INC, OpKind::Reg32),
		/*46*/ entry_lnri_1(0x46, ops::INC, OpKind::Reg32),
		/*47*/ entry_lnri_1(0x47, ops::INC, OpKind::Reg32),

		/*48*/ entry_lnri_1(0x48, ops::DEC, OpKind::Reg32),
		/*49*/ entry_lnri_1(0x49, ops::DEC, OpKind::Reg32),
		/*4A*/ entry_lnri_1(0x4A, ops::DEC, OpKind::Reg32),
		/*4B*/ entry_lnri_1(0x4B, ops::DEC, OpKind::Reg32),
		/*4C*/ entry_lnri_1(0x4C, ops::DEC, OpKind::Reg32),
		/*4D*/ entry_lnri_1(0x4D, ops::DEC, OpKind::Reg32),
		/*4E*/ entry_lnri_1(0x4E, ops::DEC, OpKind::Reg32),
		/*4F*/ entry_lnri_1(0x4F, ops::DEC, OpKind::Reg32),

		/*50*/ entry_lnri_1(0x50, ops::PUSH, OpKind::Reg64),
		/*51*/ entry_lnri_1(0x51, ops::PUSH, OpKind::Reg64),
		/*52*/ entry_lnri_1(0x52, ops::PUSH, OpKind::Reg64),
		/*53*/ entry_lnri_1(0x53, ops::PUSH, OpKind::Reg64),
		/*54*/ entry_lnri_1(0x54, ops::PUSH, OpKind::Reg64),
		/*55*/ entry_lnri_1(0x55, ops::PUSH, OpKind::Reg64),
		/*56*/ entry_lnri_1(0x56, ops::PUSH, OpKind::Reg64),
		/*57*/ entry_lnri_1(0x57, ops::PUSH, OpKind::Reg64),

		/*58*/ entry_lnri_1(0x58, ops::POP, OpKind::Reg64),
		/*59*/ entry_lnri_1(0x59, ops::POP, OpKind::Reg64),
		/*5A*/ entry_lnri_1(0x5A, ops::POP, OpKind::Reg64),
		/*5B*/ entry_lnri_1(0x5B, ops::POP, OpKind::Reg64),
		/*5C*/ entry_lnri_1(0x5C, ops::POP, OpKind::Reg64),
		/*5D*/ entry_lnri_1(0x5D, ops::POP, OpKind::Reg64),
		/*5E*/ entry_lnri_1(0x5E, ops::POP, OpKind::Reg64),
		/*5F*/ entry_lnri_1(0x5F, ops::POP, OpKind::Reg64),

		/*60*/ entry_0(0x60, ops::PUSHAD),
		/*61*/ entry_0(0x61, ops::POPAD),
		/*62*/ entry_2(0x62, ops::BOUND, OpKind::Reg32, OpKind::RegMem32),
		/*63*/ entry_2(0x63, ops::MOVSXD, OpKind::Reg64, OpKind::RegMem64),
		/*64*/ entry_blank,                                               // FS segment override
		/*65*/ entry_blank,                                               // GS segment override
		/*66*/ entry_blank,                                               // operand size override
		/*67*/ entry_blank,                                               // address size override

		/*68*/ entry_1_no_modrm(0x68, ops::PUSH, OpKind::SignExtImm32),
		/*69*/ entry_3(0x69, ops::IMUL, OpKind::Reg32, OpKind::RegMem32, OpKind::Imm32),
		/*6A*/ entry_1_no_modrm(0x6A, ops::PUSH, OpKind::Imm8),
		/*6B*/ entry_3(0x6B, ops::IMUL, OpKind::Reg32, OpKind::RegMem32, OpKind::Imm8),
		/*6C*/ entry_2(0x6C, ops::INSB, OpKind::ImplicitMem8_ES_EDI, OpKind::ImplicitDX),
		/*6D*/ entry_2(0x6D, ops::INS, OpKind::ImplicitMem32_ES_EDI, OpKind::ImplicitDX),
		/*6E*/ entry_2(0x6E, ops::OUTSB, OpKind::ImplicitDX, OpKind::ImplicitMem8_ES_EDI),
		/*6F*/ entry_2(0x6F, ops::OUTS, OpKind::ImplicitDX, OpKind::ImplicitMem32_ES_EDI),

		/*70*/ entry_1_no_modrm(0x70, ops::JO,  OpKind::Rel8Offset),
		/*71*/ entry_1_no_modrm(0x71, ops::JNO, OpKind::Rel8Offset),
		/*72*/ entry_1_no_modrm(0x72, ops::JB,  OpKind::Rel8Offset),
		/*73*/ entry_1_no_modrm(0x73, ops::JNB, OpKind::Rel8Offset),
		/*74*/ entry_1_no_modrm(0x74, ops::JZ,  OpKind::Rel8Offset),
		/*75*/ entry_1_no_modrm(0x75, ops::JNZ, OpKind::Rel8Offset),
		/*76*/ entry_1_no_modrm(0x76, ops::JNA, OpKind::Rel8Offset),
		/*77*/ entry_1_no_modrm(0x77, ops::JA,  OpKind::Rel8Offset),

		/*78*/ entry_1_no_modrm(0x78, ops::JS,  OpKind::Rel8Offset),
		/*79*/ entry_1_no_modrm(0x79, ops::JNS, OpKind::Rel8Offset),
		/*7A*/ entry_1_no_modrm(0x7A, ops::JP,  OpKind::Rel8Offset),
		/*7B*/ entry_1_no_modrm(0x7B, ops::JNP, OpKind::Rel8Offset),
		/*7C*/ entry_1_no_modrm(0x7C, ops::JL,  OpKind::Rel8Offset),
		/*7D*/ entry_1_no_modrm(0x7D, ops::JGE, OpKind::Rel8Offset),
		/*7E*/ entry_1_no_modrm(0x7E, ops::JLE, OpKind::Rel8Offset),
		/*7F*/ entry_1_no_modrm(0x7F, ops::JG,  OpKind::Rel8Offset),

		/*80*/ entry_ext(0x80, &ModRMExt_80[0]),
		/*81*/ entry_ext(0x81, &ModRMExt_81[0]),
		/*82*/ entry_ext(0x82, &ModRMExt_82[0]),
		/*83*/ entry_ext(0x83, &ModRMExt_83[0]),
		/*84*/ entry_2(0x84, ops::TEST, OpKind::RegMem8,  OpKind::Reg8),
		/*85*/ entry_2(0x85, ops::TEST, OpKind::RegMem32, OpKind::Reg32),
		/*86*/ entry_2(0x86, ops::XCHG, OpKind::RegMem8,  OpKind::Reg8),
		/*87*/ entry_2(0x87, ops::XCHG, OpKind::RegMem32, OpKind::Reg32),

		/*88*/ entry_2(0x88, ops::MOV, OpKind::RegMem8,    OpKind::Reg8),
		/*89*/ entry_2(0x89, ops::MOV, OpKind::RegMem32,   OpKind::Reg32),
		/*8A*/ entry_2(0x8A, ops::MOV, OpKind::Reg8,       OpKind::RegMem8),
		/*8B*/ entry_2(0x8B, ops::MOV, OpKind::Reg32,      OpKind::RegMem32),
		/*8C*/ entry_2(0x8C, ops::MOV, OpKind::RegMem16,   OpKind::SegmentReg),
		/*8D*/ entry_2(0x8D, ops::LEA, OpKind::Reg32,      OpKind::Memory),
		/*8E*/ entry_2(0x8E, ops::MOV, OpKind::SegmentReg, OpKind::RegMem16),

		// XOP prefix (but also POP with ModRM.reg=0), for whatever fucking reason. handle this separately again.
		/*8F*/ entry_ext(0x8F, &ModRMExt_8F[0]),

		// all of these XCHGs are implicitly exchanging with RAX.
		/*90*/ entry_lnri_2(0x90, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*91*/ entry_lnri_2(0x91, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*92*/ entry_lnri_2(0x92, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*93*/ entry_lnri_2(0x93, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*94*/ entry_lnri_2(0x94, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*95*/ entry_lnri_2(0x95, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*96*/ entry_lnri_2(0x96, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),
		/*97*/ entry_lnri_2(0x97, ops::XCHG, OpKind::ImplicitEAX, OpKind::Reg32),

		/*98*/ entry_0(0x98, ops::CWDE),
		/*99*/ entry_0(0x99, ops::CDQ),
		/*9A*/ entry_1_no_modrm(0x9A, ops::CALLF, OpKind::Ptr16_32),      // far call, invalid in 64-bit
		/*9B*/ entry_0(0x9B, ops::FWAIT),
		/*9C*/ entry_0(0x9C, ops::PUSHF),
		/*9D*/ entry_0(0x9D, ops::POPF),
		/*9E*/ entry_0(0x9E, ops::SAHF),
		/*9F*/ entry_0(0x9F, ops::LAHF),

		/*A0*/ entry_2_no_modrm(0xA0, ops::MOV, OpKind::ImplicitAL,  OpKind::MemoryOfs8),
		/*A1*/ entry_2_no_modrm(0xA1, ops::MOV, OpKind::ImplicitEAX, OpKind::MemoryOfs32),
		/*A2*/ entry_2_no_modrm(0xA2, ops::MOV, OpKind::MemoryOfs8,  OpKind::ImplicitAL),
		/*A3*/ entry_2_no_modrm(0xA3, ops::MOV, OpKind::MemoryOfs32, OpKind::ImplicitEAX),
		/*A4*/ entry_2_no_modrm(0xA4, ops::MOVSB, OpKind::ImplicitMem8_ES_EDI, OpKind::ImplicitMem8_ESI),
		/*A5*/ entry_2_no_modrm(0xA5, ops::MOVS,  OpKind::ImplicitMem32_ES_EDI, OpKind::ImplicitMem32_ESI),
		/*A6*/ entry_2_no_modrm(0xA6, ops::CMPSB, OpKind::ImplicitMem8_ES_EDI, OpKind::ImplicitMem8_ESI),
		/*A7*/ entry_2_no_modrm(0xA7, ops::CMPS,  OpKind::ImplicitMem32_ES_EDI, OpKind::ImplicitMem32_ESI),

		/*A8*/ entry_2_no_modrm(0xA8, ops::TEST, OpKind::ImplicitAL,  OpKind::Imm8),
		/*A9*/ entry_2_no_modrm(0xA9, ops::TEST, OpKind::ImplicitEAX, OpKind::Imm32),
		/*AA*/ entry_2_no_modrm(0xAA, ops::STOSB, OpKind::ImplicitMem8_ES_EDI, OpKind::ImplicitAL),
		/*AB*/ entry_2_no_modrm(0xAB, ops::STOS,  OpKind::ImplicitMem32_ES_EDI, OpKind::ImplicitEAX),
		/*AC*/ entry_2_no_modrm(0xAC, ops::LODSB, OpKind::ImplicitAL, OpKind::ImplicitMem8_ESI),
		/*AD*/ entry_2_no_modrm(0xAD, ops::LODS,  OpKind::ImplicitEAX, OpKind::ImplicitMem32_ESI),
		/*AE*/ entry_2_no_modrm(0xAE, ops::SCASB, OpKind::ImplicitAL, OpKind::ImplicitMem8_ESI),
		/*AF*/ entry_2_no_modrm(0xAF, ops::SCAS,  OpKind::ImplicitEAX, OpKind::ImplicitMem32_ESI),

		/*B0*/ entry_lnri_2(0xB0, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B1*/ entry_lnri_2(0xB1, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B2*/ entry_lnri_2(0xB2, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B3*/ entry_lnri_2(0xB3, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B4*/ entry_lnri_2(0xB4, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B5*/ entry_lnri_2(0xB5, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B6*/ entry_lnri_2(0xB6, ops::MOV, OpKind::Reg8, OpKind::Imm8),
		/*B7*/ entry_lnri_2(0xB7, ops::MOV, OpKind::Reg8, OpKind::Imm8),

		// encode the immediate size as 64-bits; the handling code will need to know how
		// to downgrade to either 32 or 16 bits depending on the prefix.
		/*B8*/ entry_lnri_2(0xB8, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*B9*/ entry_lnri_2(0xB9, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BA*/ entry_lnri_2(0xBA, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BB*/ entry_lnri_2(0xBB, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BC*/ entry_lnri_2(0xBC, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BD*/ entry_lnri_2(0xBD, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BE*/ entry_lnri_2(0xBE, ops::MOV, OpKind::Reg32, OpKind::Imm64),
		/*BF*/ entry_lnri_2(0xBF, ops::MOV, OpKind::Reg32, OpKind::Imm64),

		/*C0*/ entry_ext(0xC0, &ModRMExt_C0[0]),                          // the whole "group 2 extensions" of shift/rotate opcodes
		/*C1*/ entry_ext(0xC1, &ModRMExt_C1[0]),
		/*C2*/ entry_1_no_modrm(0xC2, ops::RET, OpKind::Imm16),
		/*C3*/ entry_0(0xC3, ops::RET),
		/*C4*/ entry_blank,                                               // vector extension escape sequence
		/*C5*/ entry_blank,                                               // vector extension escape sequence
		/*C6*/ entry_ext(0xC6, &ModRMExt_C6[0]),
		/*C7*/ entry_ext(0xC7, &ModRMExt_C7[0]),

		/*C8*/ entry_2_no_modrm(0xC8, ops::ENTER, OpKind::Imm16, OpKind::Imm8),
		/*C9*/ entry_0(0xC9, ops::LEAVE),
		/*CA*/ entry_1_no_modrm(0xCA, ops::RETF, OpKind::Imm16),
		/*CB*/ entry_0(0xCB, ops::RETF),
		/*CC*/ entry_0(0xCC, ops::INT3),
		/*CD*/ entry_1_no_modrm(0xCD, ops::INT, OpKind::Imm8),
		/*CE*/ entry_0(0xCE, ops::INTO),                                  // invalid in 64-bit
		/*CF*/ entry_0(0xCF, ops::IRET),

		/*D0*/ entry_ext(0xD0, &ModRMExt_D0[0]),
		/*D1*/ entry_ext(0xD1, &ModRMExt_D1[0]),
		/*D2*/ entry_ext(0xD2, &ModRMExt_D2[0]),
		/*D3*/ entry_ext(0xD3, &ModRMExt_D3[0]),
		/*D4*/ entry_1_no_modrm(0xD4, ops::AAM, OpKind::Imm8),            // invalid in 64-bit
		/*D5*/ entry_1_no_modrm(0xD5, ops::AAD, OpKind::Imm8),            // invalid in 64-bit
		/*D6*/ entry_blank,
		/*D7*/ entry_0(0xD7, ops::XLAT),


		// x87 fpu instructions, 0xD8 to 0xDF
		/*D8*/ entry_ext(0xD8, &ModRMExt_x87_D8[0]),
		/*D9*/ entry_ext(0xD9, &ModRMExt_x87_D9[0]),
		/*DA*/ entry_ext(0xDA, &ModRMExt_x87_DA[0]),
		/*DB*/ entry_ext(0xDB, &ModRMExt_x87_DB[0]),
		/*DC*/ entry_ext(0xDC, &ModRMExt_x87_DC[0]),
		/*DD*/ entry_ext(0xDD, &ModRMExt_x87_DD[0]),
		/*DE*/ entry_ext(0xDE, &ModRMExt_x87_DE[0]),
		/*DF*/ entry_ext(0xDF, &ModRMExt_x87_DF[0]),


		/*E0*/ entry_1_no_modrm(0xE0, ops::LOOPNZ, OpKind::Rel8Offset),
		/*E1*/ entry_1_no_modrm(0xE1, ops::LOOPZ,  OpKind::Rel8Offset),
		/*E2*/ entry_1_no_modrm(0xE2, ops::LOOP,   OpKind::Rel8Offset),
		/*E3*/ entry_1_no_modrm(0xE3, ops::JECXZ,  OpKind::Rel8Offset),
		/*E4*/ entry_2_no_modrm(0xE4, ops::IN,     OpKind::ImplicitAL, OpKind::Imm8),
		/*E5*/ entry_2_no_modrm(0xE5, ops::IN,     OpKind::ImplicitEAX, OpKind::Imm8),
		/*E6*/ entry_2_no_modrm(0xE6, ops::OUT,    OpKind::Imm8, OpKind::ImplicitAL),
		/*E7*/ entry_2_no_modrm(0xE7, ops::OUT,    OpKind::Imm8, OpKind::ImplicitEAX),

		/*E8*/ entry_1_no_modrm(0xE8, ops::CALL, OpKind::Rel32Offset),
		/*E9*/ entry_1_no_modrm(0xE9, ops::JMP,  OpKind::Rel32Offset),
		/*EA*/ entry_blank,
		/*EB*/ entry_1_no_modrm(0xEB, ops::JMP, OpKind::Rel8Offset),
		/*EC*/ entry_2_no_modrm(0xEC, ops::IN,  OpKind::ImplicitAL, OpKind::ImplicitDX),
		/*ED*/ entry_2_no_modrm(0xED, ops::IN,  OpKind::ImplicitEAX, OpKind::ImplicitDX),
		/*EE*/ entry_2_no_modrm(0xEE, ops::OUT, OpKind::ImplicitDX, OpKind::ImplicitAL),
		/*EF*/ entry_2_no_modrm(0xEF, ops::OUT, OpKind::ImplicitDX, OpKind::ImplicitEAX),

		/*F0*/ entry_blank,                                               // lock prefix
		/*F1*/ entry_0(0xF1, ops::ICEBP),
		/*F2*/ entry_blank,                                               // repne prefix
		/*F3*/ entry_blank,                                               // rep prefix
		/*F4*/ entry_0(0xF4, ops::HLT),
		/*F5*/ entry_0(0xF5, ops::CMC),
		/*F6*/ entry_ext(0xF6, &ModRMExt_F6[0]),
		/*F7*/ entry_ext(0xF7, &ModRMExt_F7[0]),

		/*F8*/ entry_0(0xF8, ops::CLC),
		/*F9*/ entry_0(0xF9, ops::STC),
		/*FA*/ entry_0(0xFA, ops::CLI),
		/*FB*/ entry_0(0xFB, ops::STI),
		/*FC*/ entry_0(0xFC, ops::CLD),
		/*FD*/ entry_0(0xFD, ops::STD),
		/*FE*/ entry_ext(0xFE, &ModRMExt_FE[0]),
		/*FF*/ entry_ext(0xFF, &ModRMExt_FF[0]),
	};
}
