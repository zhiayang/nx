// secondary.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "entry.h"

namespace instrad::x64::tables
{
	// group 6
	constexpr TableEntry ModRMExt_0F_00[] = {
		/*0*/ entry_1(0x00, ops::SLDT, OpKind::RegMem32),                   // it's not really r/m32, but... it works.
		/*1*/ entry_1(0x00, ops::STR,  OpKind::RegMem32),                   // it's not really r/m32, but... it works.
		/*2*/ entry_1(0x00, ops::LLDT, OpKind::RegMem16),
		/*3*/ entry_1(0x00, ops::LTR,  OpKind::RegMem16),
		/*4*/ entry_1(0x00, ops::VERR, OpKind::RegMem16),
		/*5*/ entry_1(0x00, ops::VERW, OpKind::RegMem16),

		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	// group 7. these fuckers additionally need ModRM.RM bits to differentiate, so make a nested table.
	// each entry here is offset by 1; the extensions are only used when modRM.mod == 3; if it is not, then
	// the first entry is the real instruction. if it == 3, then use the following instructions, but add 1 to
	// modRM.rm to get which instruction to use.
	constexpr TableEntry ModRMExt_0F_01_Mod3_Reg1_RM[] = {
		/*0*/ entry_1(0x01, ops::SIDT, OpKind::Memory),                     // it's actually mem16:32/64, but... it works.

		/*1*/ entry_0_modrm(0x01, ops::MONITOR),                            // modRM == 0xC8
		/*2*/ entry_0_modrm(0x01, ops::MWAIT),                              // modRM == 0xC9

		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_01_Mod3_Reg2_RM[] = {
		/*0*/ entry_1(0x01, ops::LGDT, OpKind::Memory),                     // it's actually mem16:32/64, but... it works.

		/*1*/ entry_0_modrm(0x01, ops::XGETBV),                             // modRM == 0xD0
		/*2*/ entry_0_modrm(0x01, ops::XSETBV),                             // modRM == 0xD1

		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_01_Mod3_Reg3_RM[] = {
		/*0*/ entry_1(0x01, ops::LIDT, OpKind::Memory),                     // it's actually mem16:32/64, but... it works.

		/*1*/ entry_0_modrm(0x01, ops::VMRUN),                              // modRM == 0xD8
		/*2*/ entry_0_modrm(0x01, ops::VMCALL),                             // modRM == 0xD9
		/*3*/ entry_1(0x01, ops::VMLOAD, OpKind::ImplicitRAX),              // modRM == 0xDA
		/*4*/ entry_1(0x01, ops::VMSAVE, OpKind::ImplicitRAX),              // modRM == 0xDB
		/*5*/ entry_0_modrm(0x01, ops::STGI),                               // modRM == 0xDC
		/*6*/ entry_0_modrm(0x01, ops::CLGI),                               // modRM == 0xDD
		/*7*/ entry_1(0x01, ops::SKINIT, OpKind::ImplicitEAX),              // modRM == 0xDE
		/*8*/ entry_2(0x01, ops::INVLPGA, OpKind::ImplicitRAX, OpKind::ImplicitECX), // modRM == 0xDF
	};

	constexpr TableEntry ModRMExt_0F_01_Mod3_Reg7_RM[] = {
		/*0*/ entry_1(0x01, ops::INVLPG, OpKind::Memory),

		/*1*/ entry_0_modrm(0x01, ops::SWAPGS),                             // modRM == 0xF8
		/*2*/ entry_0_modrm(0x01, ops::RDTSCP),                             // modRM == 0xF9
		/*3*/ entry_0_modrm(0x01, ops::MONITORX),                           // modRM == 0xFA
		/*4*/ entry_0_modrm(0x01, ops::MWAITX),                             // modRM == 0xFB

		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_01[] = {
		/*0*/ entry_1(0x01, ops::SGDT, OpKind::Memory),                     // it's actually mem16:32/64, but... it works.
		/*1*/ entry_ext_rm(0x01, &ModRMExt_0F_01_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0x01, &ModRMExt_0F_01_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0x01, &ModRMExt_0F_01_Mod3_Reg3_RM[0]),
		/*4*/ entry_1(0x01, ops::SMSW, OpKind::RegMem32),                   // not really...
		/*5*/ entry_blank,
		/*6*/ entry_1(0x01, ops::LMSW, OpKind::RegMem16),
		/*7*/ entry_ext_rm(0x1, &ModRMExt_0F_01_Mod3_Reg7_RM[0]),
	};

	// group 16
	constexpr TableEntry ModRMExt_0F_18[] = {
		/*0*/ entry_1(0x18, ops::PREFETCH, OpKind::Memory),
		/*1*/ entry_1(0x18, ops::PREFETCH, OpKind::Memory),
		/*2*/ entry_1(0x18, ops::PREFETCH, OpKind::Memory),
		/*3*/ entry_1(0x18, ops::PREFETCH, OpKind::Memory),
		/*4*/ entry_0_modrm(0x18, ops::NOP),
		/*5*/ entry_0_modrm(0x18, ops::NOP),
		/*6*/ entry_0_modrm(0x18, ops::NOP),
		/*7*/ entry_0_modrm(0x18, ops::NOP),
	};

	// group 15. same shit here; we have two different instructions depending on whether
	// modRM.mod == 3 or modRM.mod != 3. we follow the same structure as the nested tables above.
	// since there's no differentiation based on modRM.rm, the first entry is modRM.mod != 3
	// and the second entry is modRM.mod == 3.
	constexpr TableEntry ModRMExt_0F_AE_Mod3_Reg4_RM[] = {
		/*0*/ entry_1(0xAE, ops::XSAVE, OpKind::Memory),                    // modRM.mod != 3
		/*1*/ entry_blank,                                                  // modRM.mod == 3
	};

	constexpr TableEntry ModRMExt_0F_AE_Mod3_Reg5_RM[] = {
		/*0*/ entry_1(0xAE, ops::XRSTOR, OpKind::Memory),                   // modRM.mod != 3
		/*1*/ entry_0_modrm(0xAE, ops::LFENCE),                             // modRM.mod == 3
	};

	constexpr TableEntry ModRMExt_0F_AE_Mod3_Reg6_RM[] = {
		/*0*/ entry_1(0xAE, ops::XSAVEOPT, OpKind::Memory),                 // modRM.mod != 3
		/*1*/ entry_0_modrm(0xAE, ops::MFENCE),                             // modRM.mod == 3
	};

	constexpr TableEntry ModRMExt_0F_AE_Mod3_Reg7_RM[] = {
		/*0*/ entry_1(0xAE, ops::CLFLUSH, OpKind::Memory),                  // modRM.mod != 3
		/*1*/ entry_0_modrm(0xAE, ops::SFENCE),                             // modRM.mod == 3
	};

	// only used when opcode == 0x0F 0xAE, and has no prefix
	constexpr TableEntry ModRMExt_0F_AE_Prefix_None[] = {
		/*0*/ entry_1(0xAE, ops::FXSAVE,  OpKind::Memory),
		/*1*/ entry_1(0xAE, ops::FXRSTOR, OpKind::Memory),
		/*2*/ entry_1(0xAE, ops::LDMXCSR, OpKind::Memory),
		/*3*/ entry_1(0xAE, ops::STMXCSR, OpKind::Memory),
		/*4*/ entry_ext_mod(0xAE, &ModRMExt_0F_AE_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_mod(0xAE, &ModRMExt_0F_AE_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_mod(0xAE, &ModRMExt_0F_AE_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_mod(0xAE, &ModRMExt_0F_AE_Mod3_Reg7_RM[0]),
	};

	// only used when opcode == 0x0F 0xAE, and has prefix 0xF3
	constexpr TableEntry ModRMExt_0F_AE_Prefix_F3[] = {
		/*0*/ entry_1(0xAE, ops::RDFSBASE, OpKind::Reg32_Rm),
		/*1*/ entry_1(0xAE, ops::RDGSBASE, OpKind::Reg32_Rm),
		/*2*/ entry_1(0xAE, ops::WRFSBASE, OpKind::Reg32_Rm),
		/*3*/ entry_1(0xAE, ops::WRGSBASE, OpKind::Reg32_Rm),

		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	// the stupid thing about these dumbass SSE opcodes is that we need to differentiate
	// the opcodes based on their operand type. for example. 0x12: MOVLPS xmm, mem64
	// is also 0x12 MOVHLPS xmm, xmm; they do quite different things, but have the same
	// opcode. how we handle this is, again, to use extensions. the saving grace is
	// that the non-memory version (MOVHLPS) will have modrm.mod == 3, so we can use
	// our existing mechanisms to disambiguate them.
	// we set the first entry as the memory-operand-type (modrm.mod != 3), and
	// the second entry as the register-operand-type (modrm.mod == 3).

	// also, there are some instructions taking two register operands; in these cases we must
	// manually specify which register goes in the modRM.reg slot, and which one in the modRM.rm slot.
	constexpr TableEntry ModRMExt_0F_PrefixNone_12_Mod3[] = {
		/*0*/ entry_2(0x12, ops::MOVLPS,  OpKind::RegXmm, OpKind::Mem64),       // modRM.mod != 3
		/*1*/ entry_2(0x12, ops::MOVHLPS, OpKind::RegXmm, OpKind::RegXmm_Rm),   // modRM.mod == 3
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_16_Mod3[] = {
		/*0*/ entry_2(0x16, ops::MOVHPS,  OpKind::RegXmm, OpKind::Mem64),       // modRM.mod != 3
		/*1*/ entry_2(0x16, ops::MOVLHPS, OpKind::RegXmm, OpKind::RegXmm_Rm),   // modRM.mod == 3
	};

	// very specific table for cmpxchg8B/16B -- if REX.W is present, then it's 16b, if not it's 8b.
	// so far, this is the only instruction that does this -- but we still need to support it.
	constexpr TableEntry ModRMExt_0F_PrefixAny_C7_RexW[] = {
		/*0*/ entry_1(0xC7, ops::CMPXCHG8B, OpKind::Mem64),                     // rex.W == 0
		/*1*/ entry_1(0xC7, ops::CMPXCHG16B, OpKind::Mem128)                    // rex.W == 1
	};

	// "normal" extensions
	constexpr TableEntry ModRMExt_0F_PrefixAny_C7[] = {
		/*0*/ entry_blank,
		/*1*/ entry_ext_rexw(0xC7, &ModRMExt_0F_PrefixAny_C7_RexW[0]),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_1(0xC7, ops::RDRAND, OpKind::Reg32_Rm),
		/*7*/ entry_1(0xC7, ops::RDSEED, OpKind::Reg32_Rm),
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_71[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x71, ops::PSRLW, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*3*/ entry_blank,
		/*4*/ entry_2(0x71, ops::PSRAW, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*5*/ entry_blank,
		/*6*/ entry_2(0x71, ops::PSLLW, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_72[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x72, ops::PSRLD, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*3*/ entry_blank,
		/*4*/ entry_2(0x72, ops::PSRAD, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*5*/ entry_blank,
		/*6*/ entry_2(0x72, ops::PSLLD, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_73[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x73, ops::PSRLQ, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_2(0x73, ops::PSLLQ, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_B9[] = {
		/*0*/ entry_0(0xB9, ops::UD1),
		/*1*/ entry_0(0xB9, ops::UD1),
		/*2*/ entry_0(0xB9, ops::UD1),
		/*3*/ entry_0(0xB9, ops::UD1),
		/*4*/ entry_0(0xB9, ops::UD1),
		/*5*/ entry_0(0xB9, ops::UD1),
		/*6*/ entry_0(0xB9, ops::UD1),
		/*7*/ entry_0(0xB9, ops::UD1),
	};

	constexpr TableEntry ModRMExt_0F_PrefixNone_BA[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_2(0xBA, ops::BT,  OpKind::RegMem32, OpKind::Imm8),
		/*5*/ entry_2(0xBA, ops::BTS, OpKind::RegMem32, OpKind::Imm8),
		/*6*/ entry_2(0xBA, ops::BTR, OpKind::RegMem32, OpKind::Imm8),
		/*7*/ entry_2(0xBA, ops::BTC, OpKind::RegMem32, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_0F_Prefix66_71[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x71, ops::PSRLW, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*3*/ entry_blank,
		/*4*/ entry_2(0x71, ops::PSRAW, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*5*/ entry_blank,
		/*6*/ entry_2(0x71, ops::PSLLW, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_Prefix66_72[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x72, ops::PSRLD, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*3*/ entry_blank,
		/*4*/ entry_2(0x72, ops::PSRAD, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*5*/ entry_blank,
		/*6*/ entry_2(0x72, ops::PSLLD, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*7*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_0F_Prefix66_73[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0x73, ops::PSRLQ, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*3*/ entry_2(0x73, ops::PSRLDQ, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_2(0x73, ops::PSLLQ, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*7*/ entry_2(0x73, ops::PSLLDQ, OpKind::RegXmm_Rm, OpKind::Imm8),
	};

	constexpr TableEntry ModRMExt_0F_Prefix66_78[] = {
		/*0*/ entry_3(0x73, ops::EXTRQ, OpKind::RegMmx_Rm, OpKind::Imm8, OpKind::Imm8),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
	};

	// the order for these are NONE, 0x66, 0xF2, 0xF3
	constexpr TableEntry PrefixExt_0F_10[] = {
		/*0*/ entry_2(0x10, ops::MOVUPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x10, ops::MOVUPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x10, ops::MOVSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x10, ops::MOVSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_11[] = {
		/*0*/ entry_2(0x11, ops::MOVUPS, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*1*/ entry_2(0x11, ops::MOVUPD, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*2*/ entry_2(0x11, ops::MOVSD, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*3*/ entry_2(0x11, ops::MOVSS, OpKind::RegXmmMem32, OpKind::RegXmm),
	};
	constexpr TableEntry PrefixExt_0F_12[] = {
		/*0*/ entry_ext_mod(0x12, &ModRMExt_0F_PrefixNone_12_Mod3[0]),
		/*1*/ entry_2(0x12, ops::MOVLPD, OpKind::RegXmm, OpKind::Mem64),
		/*2*/ entry_2(0x12, ops::MOVDDUP, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x12, ops::MOVSLDUP, OpKind::RegXmm, OpKind::RegXmmMem128),
	};
	constexpr TableEntry PrefixExt_0F_13[] = {
		/*0*/ entry_2(0x13, ops::MOVLPS, OpKind::Mem64, OpKind::RegXmm),
		/*1*/ entry_2(0x13, ops::MOVLPD, OpKind::Mem64, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_14[] = {
		/*0*/ entry_2(0x14, ops::UNPCKLPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x14, ops::UNPCKLPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_15[] = {
		/*0*/ entry_2(0x15, ops::UNPCKHPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x15, ops::UNPCKHPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_16[] = {
		/*0*/ entry_ext_mod(0x16, &ModRMExt_0F_PrefixNone_16_Mod3[0]),
		/*1*/ entry_2(0x16, ops::MOVHPD, OpKind::RegXmm, OpKind::Mem64),
		/*2*/ entry_blank,
		/*3*/ entry_2(0x16, ops::MOVSHDUP, OpKind::RegXmm, OpKind::RegXmmMem128),
	};
	constexpr TableEntry PrefixExt_0F_17[] = {
		/*0*/ entry_2(0x17, ops::MOVHPS, OpKind::Mem64, OpKind::RegXmm),
		/*1*/ entry_2(0x17, ops::MOVHPD, OpKind::Mem64, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_28[] = {
		/*0*/ entry_2(0x28, ops::MOVAPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x28, ops::MOVAPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_29[] = {
		/*0*/ entry_2(0x29, ops::MOVAPS, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*1*/ entry_2(0x29, ops::MOVAPD, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_2A[] = {
		/*0*/ entry_2(0x2A, ops::CVTPI2PS, OpKind::RegXmm, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x2A, ops::CVTPI2PD, OpKind::RegXmm, OpKind::RegMmxMem64),
		/*2*/ entry_2(0x2A, ops::CVTSI2SD, OpKind::RegXmm, OpKind::RegMem32),
		/*3*/ entry_2(0x2A, ops::CVTSI2SS, OpKind::RegXmm, OpKind::RegMem32),
	};
	constexpr TableEntry PrefixExt_0F_2B[] = {
		/*0*/ entry_2(0x2B, ops::MOVNTPS, OpKind::Mem128, OpKind::RegXmm),
		/*1*/ entry_2(0x2B, ops::MOVNTPD, OpKind::Mem128, OpKind::RegXmm),
		/*2*/ entry_2(0x2B, ops::MOVNTSD, OpKind::Mem64, OpKind::RegXmm),
		/*3*/ entry_2(0x2B, ops::MOVNTSS, OpKind::Mem32, OpKind::RegXmm),
	};
	constexpr TableEntry PrefixExt_0F_2C[] = {
		/*0*/ entry_2(0x2C, ops::CVTTPS2PI, OpKind::RegMmx, OpKind::RegXmmMem64),
		/*1*/ entry_2(0x2C, ops::CVTTPD2PI, OpKind::RegMmx, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x2C, ops::CVTTSD2SI, OpKind::Reg32, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x2C, ops::CVTTSS2SI, OpKind::Reg32, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_2D[] = {
		/*0*/ entry_2(0x2D, ops::CVTPS2PI, OpKind::RegMmx, OpKind::RegXmmMem64),
		/*1*/ entry_2(0x2D, ops::CVTPD2PI, OpKind::RegMmx, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x2D, ops::CVTSD2SI, OpKind::Reg32, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x2D, ops::CVTSS2SI, OpKind::Reg32, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_2E[] = {
		/*0*/ entry_2(0x2E, ops::UCOMISS, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*1*/ entry_2(0x2E, ops::UCOMISD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_2F[] = {
		/*0*/ entry_2(0x2F, ops::COMISS, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*1*/ entry_2(0x2F, ops::COMISD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_50[] = {
		/*0*/ entry_2(0x50, ops::MOVMSKPS, OpKind::Reg32, OpKind::RegXmm_Rm),
		/*1*/ entry_2(0x50, ops::MOVMSKPD, OpKind::Reg32, OpKind::RegXmm_Rm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_51[] = {
		/*0*/ entry_2(0x51, ops::SQRTPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x51, ops::SQRTPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x51, ops::SQRTSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x51, ops::SQRTSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_52[] = {
		/*0*/ entry_2(0x52, ops::RSQRTPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_2(0x52, ops::RSQRTSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_53[] = {
		/*0*/ entry_2(0x53, ops::RCPPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_2(0x53, ops::RCPSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_54[] = {
		/*0*/ entry_2(0x54, ops::ANDPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x54, ops::ANDPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_55[] = {
		/*0*/ entry_2(0x55, ops::ANDNPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x55, ops::ANDNPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_56[] = {
		/*0*/ entry_2(0x56, ops::ORPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x56, ops::ORPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_57[] = {
		/*0*/ entry_2(0x57, ops::XORPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x57, ops::XORPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_58[] = {
		/*0*/ entry_2(0x58, ops::ADDPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x58, ops::ADDPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x58, ops::ADDSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x58, ops::ADDSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_59[] = {
		/*0*/ entry_2(0x59, ops::MULPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x59, ops::MULPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x59, ops::MULSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x59, ops::MULSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_5A[] = {
		/*0*/ entry_2(0x5A, ops::CVTPS2PD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*1*/ entry_2(0x5A, ops::CVTPD2PS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x5A, ops::CVTSD2SS, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x5A, ops::CVTSS2SD, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_5B[] = {
		/*0*/ entry_2(0x5B, ops::CVTDQ2PS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x5B, ops::CVTPS2DQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_2(0x5B, ops::CVTTPS2DQ, OpKind::RegXmm, OpKind::RegXmmMem128),
	};
	constexpr TableEntry PrefixExt_0F_5C[] = {
		/*0*/ entry_2(0x5C, ops::SUBPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x5C, ops::SUBPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x5C, ops::SUBSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x5C, ops::SUBSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_5D[] = {
		/*0*/ entry_2(0x5D, ops::MINPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x5D, ops::MINPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x5D, ops::MINSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x5D, ops::MINSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_5E[] = {
		/*0*/ entry_2(0x5E, ops::DIVPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x5E, ops::DIVPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x5E, ops::DIVSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x5E, ops::DIVSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};
	constexpr TableEntry PrefixExt_0F_5F[] = {
		/*0*/ entry_2(0x5F, ops::MAXPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*1*/ entry_2(0x5F, ops::MAXPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x5F, ops::MAXSD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*3*/ entry_2(0x5F, ops::MAXSS, OpKind::RegXmm, OpKind::RegXmmMem32),
	};


	constexpr TableEntry PrefixExt_0F_60[] = {
		/*0*/ entry_2(0x60, ops::PUNPCKLBW, OpKind::RegMmx, OpKind::RegMmxMem32),
		/*1*/ entry_2(0x60, ops::PUNPCKLBW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_61[] = {
		/*0*/ entry_2(0x61, ops::PUNPCKLWD, OpKind::RegMmx, OpKind::RegMmxMem32),
		/*1*/ entry_2(0x61, ops::PUNPCKLWD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_62[] = {
		/*0*/ entry_2(0x62, ops::PUNPCKLDQ, OpKind::RegMmx, OpKind::RegMmxMem32),
		/*1*/ entry_2(0x62, ops::PUNPCKLDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_63[] = {
		/*0*/ entry_2(0x63, ops::PACKSSWB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x63, ops::PACKSSWB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_64[] = {
		/*0*/ entry_2(0x64, ops::PCMPGTB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x64, ops::PCMPGTB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_65[] = {
		/*0*/ entry_2(0x65, ops::PCMPGTW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x65, ops::PCMPGTW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_66[] = {
		/*0*/ entry_2(0x66, ops::PCMPGTD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x66, ops::PCMPGTD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_67[] = {
		/*0*/ entry_2(0x67, ops::PACKUSWB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x67, ops::PACKUSWB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_68[] = {
		/*0*/ entry_2(0x68, ops::PUNPCKHBW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x68, ops::PUNPCKHBW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_69[] = {
		/*0*/ entry_2(0x69, ops::PUNPCKHWD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x69, ops::PUNPCKHWD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6A[] = {
		/*0*/ entry_2(0x6A, ops::PUNPCKHDQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x6A, ops::PUNPCKHDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6B[] = {
		/*0*/ entry_2(0x6B, ops::PACKSSDW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x6B, ops::PACKSSDW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6C[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x6C, ops::PUNPCKLQDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6D[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x6D, ops::PUNPCKHQDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6E[] = {
		/*0*/ entry_2(0x6E, ops::MOVD, OpKind::RegMmx, OpKind::RegMem32),   // this will auto-promote to RegMem64 based on REX.W
		/*1*/ entry_2(0x6E, ops::MOVD, OpKind::RegXmm, OpKind::RegMem32),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_6F[] = {
		/*0*/ entry_2(0x6F, ops::MOVQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x6F, ops::MOVDQA, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_2(0x6F, ops::MOVDQU, OpKind::RegXmm, OpKind::RegXmmMem128),
	};


	constexpr TableEntry PrefixExt_0F_70[] = {
		/*0*/ entry_3(0x70, ops::PSHUFW, OpKind::RegMmx, OpKind::RegMmxMem64, OpKind::Imm8),
		/*1*/ entry_3(0x70, ops::PSHUFD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_3(0x70, ops::PSHUFLW, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*3*/ entry_3(0x70, ops::PSHUFHW, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
	};
	constexpr TableEntry PrefixExt_0F_71[] = {
		/*0*/ entry_ext(0x71, &ModRMExt_0F_PrefixNone_71[0]),
		/*1*/ entry_ext(0x71, &ModRMExt_0F_Prefix66_71[0]),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_72[] = {
		/*0*/ entry_ext(0x72, &ModRMExt_0F_PrefixNone_72[0]),
		/*1*/ entry_ext(0x72, &ModRMExt_0F_Prefix66_72[0]),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_73[] = {
		/*0*/ entry_ext(0x73, &ModRMExt_0F_PrefixNone_73[0]),
		/*1*/ entry_ext(0x73, &ModRMExt_0F_Prefix66_73[0]),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_74[] = {
		/*0*/ entry_2(0x74, ops::PCMPEQB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x74, ops::PCMPEQB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_75[] = {
		/*0*/ entry_2(0x75, ops::PCMPEQW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x75, ops::PCMPEQW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_76[] = {
		/*0*/ entry_2(0x76, ops::PCMPEQD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x76, ops::PCMPEQD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_77[] = {
		/*0*/ entry_0(0x77, ops::EMMS),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_78[] = {
		/*0*/ entry_blank,
		/*1*/ entry_ext(0x78, &ModRMExt_0F_Prefix66_78[0]),
		/*2*/ entry_4(0x78, ops::INSERTQ, OpKind::RegXmm, OpKind::RegXmm_Rm, OpKind::Imm8, OpKind::Imm8),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_79[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x79, ops::EXTRQ, OpKind::RegXmm, OpKind::RegXmm_Rm),
		/*2*/ entry_2(0x79, ops::INSERTQ, OpKind::RegXmm, OpKind::RegXmm_Rm),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_7A[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_7B[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_7C[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x7C, ops::HADDPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x7C, ops::HADDPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_7D[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x7D, ops::HSUBPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0x7D, ops::HSUBPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_7E[] = {
		/*0*/ entry_2(0x7E, ops::MOVD, OpKind::RegMem32, OpKind::RegMmx),    // this will auto-promote to RegMem64 based on REX.W
		/*1*/ entry_2(0x7E, ops::MOVD, OpKind::RegMem32, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_2(0x7E, ops::MOVQ, OpKind::RegXmm, OpKind::RegXmmMem64),
	};
	constexpr TableEntry PrefixExt_0F_7F[] = {
		/*0*/ entry_2(0x7F, ops::MOVQ, OpKind::RegMmxMem64, OpKind::RegMmx),
		/*1*/ entry_2(0x7F, ops::MOVDQA, OpKind::RegXmmMem128, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_2(0x7F, ops::MOVDQU, OpKind::RegXmmMem128, OpKind::RegXmm),
	};


	constexpr TableEntry PrefixExt_0F_AE[] = {
		/*0*/ entry_ext(0xAE, &ModRMExt_0F_AE_Prefix_None[0]),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_ext(0xAE, &ModRMExt_0F_AE_Prefix_F3[0]),
	};


	constexpr TableEntry PrefixExt_0F_B8[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_2(0xB8, ops::POPCNT, OpKind::Reg32, OpKind::RegMem32),
	};
	constexpr TableEntry PrefixExt_0F_B9[] = {
		/*0*/ entry_ext(0xB9, &ModRMExt_0F_PrefixNone_B9[0]),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_BA[] = {
		/*0*/ entry_ext(0xBA, &ModRMExt_0F_PrefixNone_BA[0]),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_BB[] = {
		/*0*/ entry_2(0xBB, ops::BTC, OpKind::RegMem32, OpKind::Reg32),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_BC[] = {
		/*0*/ entry_2(0xBC, ops::BSF, OpKind::Reg32, OpKind::RegMem32),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_2(0xBC, ops::TZCNT, OpKind::Reg32, OpKind::RegMem32),
	};
	constexpr TableEntry PrefixExt_0F_BD[] = {
		/*0*/ entry_2(0xBD, ops::BSR, OpKind::Reg32, OpKind::RegMem32),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_2(0xBD, ops::LZCNT, OpKind::Reg32, OpKind::RegMem32),
	};
	constexpr TableEntry PrefixExt_0F_BE[] = {
		/*0*/ entry_2(0xBE, ops::MOVSX, OpKind::Reg32, OpKind::RegMem8),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_BF[] = {
		/*0*/ entry_2(0xBF, ops::MOVSX, OpKind::Reg32, OpKind::RegMem16),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_C0[] = {
		/*0*/ entry_2(0xC0, ops::XADD, OpKind::RegMem8, OpKind::Reg8),
		/*1*/ entry_2(0xC0, ops::XADD, OpKind::RegMem8, OpKind::Reg8),
		/*2*/ entry_2(0xC0, ops::XADD, OpKind::RegMem8, OpKind::Reg8),
		/*3*/ entry_2(0xC0, ops::XADD, OpKind::RegMem8, OpKind::Reg8),
	};
	constexpr TableEntry PrefixExt_0F_C1[] = {
		/*0*/ entry_2(0xC1, ops::XADD, OpKind::RegMem32, OpKind::Reg32),
		/*1*/ entry_2(0xC1, ops::XADD, OpKind::RegMem32, OpKind::Reg32),
		/*2*/ entry_2(0xC1, ops::XADD, OpKind::RegMem32, OpKind::Reg32),
		/*3*/ entry_2(0xC1, ops::XADD, OpKind::RegMem32, OpKind::Reg32),
	};
	constexpr TableEntry PrefixExt_0F_C2[] = {
		/*0*/ entry_3(0xC2, ops::CMPPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*1*/ entry_3(0xC2, ops::CMPPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_3(0xC2, ops::CMPSD, OpKind::RegXmm, OpKind::RegXmmMem64, OpKind::Imm8),
		/*3*/ entry_3(0xC2, ops::CMPSS, OpKind::RegXmm, OpKind::RegXmmMem32, OpKind::Imm8),
	};
	constexpr TableEntry PrefixExt_0F_C3[] = {
		/*0*/ entry_2(0xC3, ops::MOVNTI, OpKind::Mem32, OpKind::Reg32),
		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_C4[] = {
		/*0*/ entry_3(0xC4, ops::PINSRW, OpKind::RegMmx, OpKind::Reg32Mem16, OpKind::Imm8),
		/*1*/ entry_3(0xC4, ops::PINSRW, OpKind::RegXmm, OpKind::Reg32Mem16, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_C5[] = {
		/*0*/ entry_3(0xC5, ops::PEXTRW, OpKind::Reg32, OpKind::RegMmx_Rm, OpKind::Imm8),
		/*1*/ entry_3(0xC5, ops::PEXTRW, OpKind::Reg32, OpKind::RegXmm_Rm, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_C6[] = {
		/*0*/ entry_3(0xC6, ops::SHUFPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*1*/ entry_3(0xC6, ops::SHUFPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_C7[] = {
		/*0*/ entry_ext(0xC7, &ModRMExt_0F_PrefixAny_C7[0]),
		/*1*/ entry_ext(0xC7, &ModRMExt_0F_PrefixAny_C7[0]),
		/*2*/ entry_ext(0xC7, &ModRMExt_0F_PrefixAny_C7[0]),
		/*3*/ entry_ext(0xC7, &ModRMExt_0F_PrefixAny_C7[0]),
	};


	constexpr TableEntry PrefixExt_0F_D0[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xD0, ops::ADDSUBPD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0xD0, ops::ADDSUBPS, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D1[] = {
		/*0*/ entry_2(0xD1, ops::PSRLW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD1, ops::PSRLW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D2[] = {
		/*0*/ entry_2(0xD2, ops::PSRLD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD2, ops::PSRLD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D3[] = {
		/*0*/ entry_2(0xD3, ops::PSRLQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD3, ops::PSRLQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D4[] = {
		/*0*/ entry_2(0xD4, ops::PADDQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD4, ops::PADDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D5[] = {
		/*0*/ entry_2(0xD5, ops::PMULLW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD5, ops::PMULLW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D6[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xD6, ops::MOVQ, OpKind::RegXmmMem64, OpKind::RegXmm),
		/*2*/ entry_2(0xD6, ops::MOVDQ2Q, OpKind::RegMmx, OpKind::RegXmm_Rm),
		/*3*/ entry_2(0xD6, ops::MOVQ2DQ, OpKind::RegXmm, OpKind::RegMmx_Rm),
	};
	constexpr TableEntry PrefixExt_0F_D7[] = {
		/*0*/ entry_2(0xD7, ops::PMOVMSKB, OpKind::Reg32, OpKind::RegMmx_Rm),
		/*1*/ entry_2(0xD7, ops::PMOVMSKB, OpKind::Reg32, OpKind::RegXmm_Rm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_D8[] = {
		/*0*/ entry_2(0xD8, ops::PSUBUSB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD8, ops::PSUBUSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_D9[] = {
		/*0*/ entry_2(0xD9, ops::PSUBUSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xD9, ops::PSUBUSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DA[] = {
		/*0*/ entry_2(0xDA, ops::PMINUB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDA, ops::PMINUB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DB[] = {
		/*0*/ entry_2(0xDB, ops::PAND, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDB, ops::PAND, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DC[] = {
		/*0*/ entry_2(0xDC, ops::PADDUSB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDC, ops::PADDUSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DD[] = {
		/*0*/ entry_2(0xDD, ops::PADDUSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDD, ops::PADDUSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DE[] = {
		/*0*/ entry_2(0xDE, ops::PMAXUB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDE, ops::PMAXUB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_DF[] = {
		/*0*/ entry_2(0xDF, ops::PANDN, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xDF, ops::PANDN, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_E0[] = {
		/*0*/ entry_2(0xE0, ops::PAVGB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE0, ops::PAVGB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E1[] = {
		/*0*/ entry_2(0xE1, ops::PSRAW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE1, ops::PSRAW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E2[] = {
		/*0*/ entry_2(0xE2, ops::PSRAD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE2, ops::PSRAD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E3[] = {
		/*0*/ entry_2(0xE3, ops::PAVGW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE3, ops::PAVGW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E4[] = {
		/*0*/ entry_2(0xE4, ops::PMULHUW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE4, ops::PMULHUW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E5[] = {
		/*0*/ entry_2(0xE5, ops::PMULHW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE5, ops::PMULHW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E6[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xE6, ops::CVTTPD2DQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_2(0xE6, ops::CVTPD2DQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*3*/ entry_2(0xE6, ops::CVTDQ2PD, OpKind::RegXmm, OpKind::RegXmmMem64),
	};
	constexpr TableEntry PrefixExt_0F_E7[] = {
		/*0*/ entry_2(0xE7, ops::MOVNTQ, OpKind::Mem64, OpKind::RegMmx),
		/*1*/ entry_2(0xE7, ops::MOVNTDQ, OpKind::Mem128, OpKind::RegXmm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_E8[] = {
		/*0*/ entry_2(0xE8, ops::PSUBSB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE8, ops::PSUBSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_E9[] = {
		/*0*/ entry_2(0xE9, ops::PSUBSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xE9, ops::PSUBSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_EA[] = {
		/*0*/ entry_2(0xEA, ops::PMINSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xEA, ops::PMINSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_EB[] = {
		/*0*/ entry_2(0xEB, ops::POR, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xEB, ops::POR, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_EC[] = {
		/*0*/ entry_2(0xEC, ops::PADDSB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xEC, ops::PADDSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_ED[] = {
		/*0*/ entry_2(0xED, ops::PADDSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xED, ops::PADDSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_EE[] = {
		/*0*/ entry_2(0xEE, ops::PMAXSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xEE, ops::PMAXSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_EF[] = {
		/*0*/ entry_2(0xEF, ops::PXOR, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xEF, ops::PXOR, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_F0[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_2(0xF2, ops::LDDQU, OpKind::RegXmm, OpKind::Mem128),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F1[] = {
		/*0*/ entry_2(0xF1, ops::PSLLW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF1, ops::PSLLW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F2[] = {
		/*0*/ entry_2(0xF2, ops::PSLLD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF2, ops::PSLLD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F3[] = {
		/*0*/ entry_2(0xF3, ops::PSLLQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF3, ops::PSLLQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F4[] = {
		/*0*/ entry_2(0xF4, ops::PMULUDQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF4, ops::PMULUDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F5[] = {
		/*0*/ entry_2(0xF5, ops::PMADDWD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF5, ops::PMADDWD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F6[] = {
		/*0*/ entry_2(0xF6, ops::PSADBW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF6, ops::PSADBW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F7[] = {
		/*0*/ entry_2(0xF7, ops::MASKMOVQ, OpKind::RegMmx, OpKind::RegMmx_Rm),
		/*1*/ entry_2(0xF7, ops::MASKMOVDQU, OpKind::RegXmm, OpKind::RegXmm_Rm),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};


	constexpr TableEntry PrefixExt_0F_F8[] = {
		/*0*/ entry_2(0xF8, ops::PSUBB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF8, ops::PSUBB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_F9[] = {
		/*0*/ entry_2(0xF9, ops::PSUBW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xF9, ops::PSUBW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FA[] = {
		/*0*/ entry_2(0xFA, ops::PSUBD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xFA, ops::PSUBD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FB[] = {
		/*0*/ entry_2(0xFB, ops::PSUBQ, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xFB, ops::PSUBQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FC[] = {
		/*0*/ entry_2(0xFC, ops::PADDB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xFC, ops::PADDB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FD[] = {
		/*0*/ entry_2(0xFD, ops::PADDW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xFD, ops::PADDW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FE[] = {
		/*0*/ entry_2(0xFE, ops::PADDD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0xFE, ops::PADDD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_FF[] = {
		/*0*/ entry_0(0xFF, ops::UD0),
		/*1*/ entry_0(0xFF, ops::UD0),
		/*2*/ entry_0(0xFF, ops::UD0),
		/*3*/ entry_0(0xFF, ops::UD0),
	};

	constexpr TableEntry SecondaryOpcodeMap_0F[] = {
		/*00*/ entry_ext(0x00, &ModRMExt_0F_00[0]),
		/*01*/ entry_ext(0x01, &ModRMExt_0F_01[0]),
		/*02*/ entry_2(0x02, ops::LAR, OpKind::Reg32, OpKind::RegMem16),
		/*03*/ entry_2(0x03, ops::LSL, OpKind::Reg32, OpKind::RegMem16),
		/*04*/ entry_blank,
		/*05*/ entry_0(0x05, ops::SYSCALL),
		/*06*/ entry_0(0x06, ops::CLTS),
		/*07*/ entry_0(0x07, ops::SYSRET),

		/*08*/ entry_0(0x08, ops::INVD),
		/*09*/ entry_0(0x09, ops::WBINVD),
		/*0A*/ entry_blank,
		/*0B*/ entry_0(0x0B, ops::UD2),
		/*0C*/ entry_blank,
		/*0D*/ entry_1(0x0D, ops::PREFETCH, OpKind::Memory),
		/*0E*/ entry_0(0x0E, ops::FEMMS),
		/*0F*/ entry_blank,

		/*10*/ entry_ext_prefix(0x10, &PrefixExt_0F_10[0]),
		/*11*/ entry_ext_prefix(0x11, &PrefixExt_0F_11[0]),
		/*12*/ entry_ext_prefix(0x12, &PrefixExt_0F_12[0]),
		/*13*/ entry_ext_prefix(0x13, &PrefixExt_0F_13[0]),
		/*14*/ entry_ext_prefix(0x14, &PrefixExt_0F_14[0]),
		/*15*/ entry_ext_prefix(0x15, &PrefixExt_0F_15[0]),
		/*16*/ entry_ext_prefix(0x16, &PrefixExt_0F_16[0]),
		/*17*/ entry_ext_prefix(0x17, &PrefixExt_0F_17[0]),

		/*18*/ entry_ext(0x18, &ModRMExt_0F_18[0]),
		/*19*/ entry_1(0x19, ops::NOP, OpKind::RegMem32),
		/*1A*/ entry_1(0x1A, ops::NOP, OpKind::RegMem32),
		/*1B*/ entry_1(0x1B, ops::NOP, OpKind::RegMem32),
		/*1C*/ entry_1(0x1C, ops::NOP, OpKind::RegMem32),
		/*1D*/ entry_1(0x1D, ops::NOP, OpKind::RegMem32),
		/*1E*/ entry_1(0x1E, ops::NOP, OpKind::RegMem32),
		/*1F*/ entry_1(0x1F, ops::NOP, OpKind::RegMem32),

		/*20*/ entry_2(0x20, ops::MOV, OpKind::Reg64_Rm, OpKind::ControlReg),  // the special registers always go in the modRM.reg slot
		/*21*/ entry_2(0x21, ops::MOV, OpKind::Reg64_Rm, OpKind::DebugReg),
		/*22*/ entry_2(0x22, ops::MOV, OpKind::ControlReg, OpKind::Reg64_Rm),
		/*23*/ entry_2(0x23, ops::MOV, OpKind::DebugReg, OpKind::Reg64_Rm),
		/*24*/ entry_blank,
		/*25*/ entry_blank,
		/*26*/ entry_blank,
		/*27*/ entry_blank,

		/*28*/ entry_ext_prefix(0x28, &PrefixExt_0F_28[0]),
		/*29*/ entry_ext_prefix(0x29, &PrefixExt_0F_29[0]),
		/*2A*/ entry_ext_prefix(0x2A, &PrefixExt_0F_2A[0]),
		/*2B*/ entry_ext_prefix(0x2B, &PrefixExt_0F_2B[0]),
		/*2C*/ entry_ext_prefix(0x2C, &PrefixExt_0F_2C[0]),
		/*2D*/ entry_ext_prefix(0x2D, &PrefixExt_0F_2D[0]),
		/*2E*/ entry_ext_prefix(0x2E, &PrefixExt_0F_2E[0]),
		/*2F*/ entry_ext_prefix(0x2F, &PrefixExt_0F_2F[0]),

		/*30*/ entry_0(0x30, ops::WRMSR),
		/*31*/ entry_0(0x31, ops::RDTSC),
		/*32*/ entry_0(0x32, ops::RDMSR),
		/*33*/ entry_0(0x33, ops::RDPMC),
		/*34*/ entry_0(0x34, ops::SYSENTER),
		/*35*/ entry_0(0x35, ops::SYSEXIT),
		/*36*/ entry_blank,
		/*37*/ entry_blank,

		/*38*/ entry_blank,
		/*39*/ entry_blank,
		/*3A*/ entry_blank,
		/*3B*/ entry_blank,
		/*3C*/ entry_blank,
		/*3D*/ entry_blank,
		/*3E*/ entry_blank,
		/*3F*/ entry_blank,

		/*40*/ entry_2(0x40, ops::CMOVO,  OpKind::Reg32, OpKind::RegMem32),
		/*41*/ entry_2(0x41, ops::CMOVNO, OpKind::Reg32, OpKind::RegMem32),
		/*42*/ entry_2(0x42, ops::CMOVB,  OpKind::Reg32, OpKind::RegMem32),
		/*43*/ entry_2(0x43, ops::CMOVNB, OpKind::Reg32, OpKind::RegMem32),
		/*44*/ entry_2(0x44, ops::CMOVZ,  OpKind::Reg32, OpKind::RegMem32),
		/*45*/ entry_2(0x45, ops::CMOVNZ, OpKind::Reg32, OpKind::RegMem32),
		/*46*/ entry_2(0x46, ops::CMOVNA, OpKind::Reg32, OpKind::RegMem32),
		/*47*/ entry_2(0x47, ops::CMOVA,  OpKind::Reg32, OpKind::RegMem32),

		/*48*/ entry_2(0x48, ops::CMOVS,  OpKind::Reg32, OpKind::RegMem32),
		/*49*/ entry_2(0x49, ops::CMOVNS, OpKind::Reg32, OpKind::RegMem32),
		/*4A*/ entry_2(0x4A, ops::CMOVP,  OpKind::Reg32, OpKind::RegMem32),
		/*4B*/ entry_2(0x4B, ops::CMOVNP, OpKind::Reg32, OpKind::RegMem32),
		/*4C*/ entry_2(0x4C, ops::CMOVL,  OpKind::Reg32, OpKind::RegMem32),
		/*4D*/ entry_2(0x4D, ops::CMOVGE, OpKind::Reg32, OpKind::RegMem32),
		/*4E*/ entry_2(0x4E, ops::CMOVLE, OpKind::Reg32, OpKind::RegMem32),
		/*4F*/ entry_2(0x4F, ops::CMOVG,  OpKind::Reg32, OpKind::RegMem32),

		/*50*/ entry_ext_prefix(0x50, &PrefixExt_0F_50[0]),
		/*51*/ entry_ext_prefix(0x51, &PrefixExt_0F_51[0]),
		/*52*/ entry_ext_prefix(0x52, &PrefixExt_0F_52[0]),
		/*53*/ entry_ext_prefix(0x53, &PrefixExt_0F_53[0]),
		/*54*/ entry_ext_prefix(0x54, &PrefixExt_0F_54[0]),
		/*55*/ entry_ext_prefix(0x55, &PrefixExt_0F_55[0]),
		/*56*/ entry_ext_prefix(0x56, &PrefixExt_0F_56[0]),
		/*57*/ entry_ext_prefix(0x57, &PrefixExt_0F_57[0]),

		/*58*/ entry_ext_prefix(0x58, &PrefixExt_0F_58[0]),
		/*59*/ entry_ext_prefix(0x59, &PrefixExt_0F_59[0]),
		/*5A*/ entry_ext_prefix(0x5A, &PrefixExt_0F_5A[0]),
		/*5B*/ entry_ext_prefix(0x5B, &PrefixExt_0F_5B[0]),
		/*5C*/ entry_ext_prefix(0x5C, &PrefixExt_0F_5C[0]),
		/*5D*/ entry_ext_prefix(0x5D, &PrefixExt_0F_5D[0]),
		/*5E*/ entry_ext_prefix(0x5E, &PrefixExt_0F_5E[0]),
		/*5F*/ entry_ext_prefix(0x5F, &PrefixExt_0F_5F[0]),

		/*60*/ entry_ext_prefix(0x60, &PrefixExt_0F_60[0]),
		/*61*/ entry_ext_prefix(0x61, &PrefixExt_0F_61[0]),
		/*62*/ entry_ext_prefix(0x62, &PrefixExt_0F_62[0]),
		/*63*/ entry_ext_prefix(0x63, &PrefixExt_0F_63[0]),
		/*64*/ entry_ext_prefix(0x64, &PrefixExt_0F_64[0]),
		/*65*/ entry_ext_prefix(0x65, &PrefixExt_0F_65[0]),
		/*66*/ entry_ext_prefix(0x66, &PrefixExt_0F_66[0]),
		/*67*/ entry_ext_prefix(0x67, &PrefixExt_0F_67[0]),

		/*68*/ entry_ext_prefix(0x68, &PrefixExt_0F_68[0]),
		/*69*/ entry_ext_prefix(0x69, &PrefixExt_0F_69[0]),
		/*6A*/ entry_ext_prefix(0x6A, &PrefixExt_0F_6A[0]),
		/*6B*/ entry_ext_prefix(0x6B, &PrefixExt_0F_6B[0]),
		/*6C*/ entry_ext_prefix(0x6C, &PrefixExt_0F_6C[0]),
		/*6D*/ entry_ext_prefix(0x6D, &PrefixExt_0F_6D[0]),
		/*6E*/ entry_ext_prefix(0x6E, &PrefixExt_0F_6E[0]),
		/*6F*/ entry_ext_prefix(0x6F, &PrefixExt_0F_6F[0]),

		/*70*/ entry_ext_prefix(0x70, &PrefixExt_0F_70[0]),
		/*71*/ entry_ext_prefix(0x71, &PrefixExt_0F_71[0]),
		/*72*/ entry_ext_prefix(0x72, &PrefixExt_0F_72[0]),
		/*73*/ entry_ext_prefix(0x73, &PrefixExt_0F_73[0]),
		/*74*/ entry_ext_prefix(0x74, &PrefixExt_0F_74[0]),
		/*75*/ entry_ext_prefix(0x75, &PrefixExt_0F_75[0]),
		/*76*/ entry_ext_prefix(0x76, &PrefixExt_0F_76[0]),
		/*77*/ entry_ext_prefix(0x77, &PrefixExt_0F_77[0]),

		/*78*/ entry_ext_prefix(0x78, &PrefixExt_0F_78[0]),
		/*79*/ entry_ext_prefix(0x79, &PrefixExt_0F_79[0]),
		/*7A*/ entry_ext_prefix(0x7A, &PrefixExt_0F_7A[0]),
		/*7B*/ entry_ext_prefix(0x7B, &PrefixExt_0F_7B[0]),
		/*7C*/ entry_ext_prefix(0x7C, &PrefixExt_0F_7C[0]),
		/*7D*/ entry_ext_prefix(0x7D, &PrefixExt_0F_7D[0]),
		/*7E*/ entry_ext_prefix(0x7E, &PrefixExt_0F_7E[0]),
		/*7F*/ entry_ext_prefix(0x7F, &PrefixExt_0F_7F[0]),

		/*80*/ entry_1_no_modrm(0x80, ops::JO,  OpKind::Rel32Offset),
		/*81*/ entry_1_no_modrm(0x81, ops::JNO, OpKind::Rel32Offset),
		/*82*/ entry_1_no_modrm(0x82, ops::JB,  OpKind::Rel32Offset),
		/*83*/ entry_1_no_modrm(0x83, ops::JNB, OpKind::Rel32Offset),
		/*84*/ entry_1_no_modrm(0x84, ops::JZ,  OpKind::Rel32Offset),
		/*85*/ entry_1_no_modrm(0x85, ops::JNZ, OpKind::Rel32Offset),
		/*86*/ entry_1_no_modrm(0x86, ops::JNA, OpKind::Rel32Offset),
		/*87*/ entry_1_no_modrm(0x87, ops::JA,  OpKind::Rel32Offset),

		/*88*/ entry_1_no_modrm(0x88, ops::JS,  OpKind::Rel32Offset),
		/*89*/ entry_1_no_modrm(0x89, ops::JNS, OpKind::Rel32Offset),
		/*8A*/ entry_1_no_modrm(0x8A, ops::JP,  OpKind::Rel32Offset),
		/*8B*/ entry_1_no_modrm(0x8B, ops::JNP, OpKind::Rel32Offset),
		/*8C*/ entry_1_no_modrm(0x8C, ops::JL,  OpKind::Rel32Offset),
		/*8D*/ entry_1_no_modrm(0x8D, ops::JGE, OpKind::Rel32Offset),
		/*8E*/ entry_1_no_modrm(0x8E, ops::JLE, OpKind::Rel32Offset),
		/*8F*/ entry_1_no_modrm(0x8F, ops::JG,  OpKind::Rel32Offset),

		/*90*/ entry_1(0x90, ops::SETO,  OpKind::RegMem8),
		/*91*/ entry_1(0x91, ops::SETNO, OpKind::RegMem8),
		/*92*/ entry_1(0x92, ops::SETB,  OpKind::RegMem8),
		/*93*/ entry_1(0x93, ops::SETNB, OpKind::RegMem8),
		/*94*/ entry_1(0x94, ops::SETZ,  OpKind::RegMem8),
		/*95*/ entry_1(0x95, ops::SETNZ, OpKind::RegMem8),
		/*96*/ entry_1(0x96, ops::SETNA, OpKind::RegMem8),
		/*97*/ entry_1(0x97, ops::SETA,  OpKind::RegMem8),

		/*98*/ entry_1(0x98, ops::SETS,  OpKind::RegMem8),
		/*99*/ entry_1(0x99, ops::SETNS, OpKind::RegMem8),
		/*9A*/ entry_1(0x9A, ops::SETP,  OpKind::RegMem8),
		/*9B*/ entry_1(0x9B, ops::SETNP, OpKind::RegMem8),
		/*9C*/ entry_1(0x9C, ops::SETL,  OpKind::RegMem8),
		/*9D*/ entry_1(0x9D, ops::SETGE, OpKind::RegMem8),
		/*9E*/ entry_1(0x9E, ops::SETLE, OpKind::RegMem8),
		/*9F*/ entry_1(0x9F, ops::SETG,  OpKind::RegMem8),

		/*A0*/ entry_1_no_modrm(0xA0, ops::PUSH, OpKind::ImplicitFS),
		/*A1*/ entry_1_no_modrm(0xA1, ops::POP,  OpKind::ImplicitFS),
		/*A2*/ entry_0(0xA2, ops::CPUID),
		/*A3*/ entry_2(0xA3, ops::BT,   OpKind::RegMem32, OpKind::Reg32),
		/*A4*/ entry_3(0xA4, ops::SHLD, OpKind::RegMem32, OpKind::Reg32, OpKind::Imm8),
		/*A5*/ entry_3(0xA5, ops::SHLD, OpKind::RegMem32, OpKind::Reg32, OpKind::ImplicitCL),
		/*A6*/ entry_blank,
		/*A7*/ entry_blank,

		/*A8*/ entry_1_no_modrm(0xA8, ops::PUSH, OpKind::ImplicitGS),
		/*A9*/ entry_1_no_modrm(0xA9, ops::POP,  OpKind::ImplicitGS),
		/*AA*/ entry_0(0xAA, ops::RSM),
		/*AB*/ entry_2(0xAB, ops::BTS,  OpKind::RegMem32, OpKind::Reg32),
		/*AC*/ entry_3(0xAC, ops::SHLD, OpKind::RegMem32, OpKind::Reg32, OpKind::Imm8),
		/*AD*/ entry_3(0xAD, ops::SHLD, OpKind::RegMem32, OpKind::Reg32, OpKind::ImplicitCL),
		/*AE*/ entry_ext_prefix(0xAE, &PrefixExt_0F_AE[0]),
		/*AF*/ entry_2(0xAF, ops::IMUL, OpKind::Reg32, OpKind::RegMem32),

		/*B0*/ entry_2(0xB0, ops::CMPXCHG, OpKind::RegMem8, OpKind::Reg8),
		/*B1*/ entry_2(0xB1, ops::CMPXCHG, OpKind::RegMem32, OpKind::Reg32),
		/*B2*/ entry_2(0xB2, ops::LSS,     OpKind::Reg32, OpKind::Ptr16_32),
		/*B3*/ entry_2(0xB3, ops::BTR,     OpKind::RegMem32, OpKind::Reg32),
		/*B4*/ entry_2(0xB4, ops::LFS,     OpKind::Reg32, OpKind::Ptr16_32),
		/*B5*/ entry_2(0xB5, ops::LGS,     OpKind::Reg32, OpKind::Ptr16_32),
		/*B6*/ entry_2(0xB6, ops::MOVZX,   OpKind::Reg32, OpKind::RegMem8),
		/*B7*/ entry_2(0xB7, ops::MOVZX,   OpKind::Reg32, OpKind::RegMem16),

		/*B8*/ entry_ext_prefix(0xB8, &PrefixExt_0F_B8[0]),
		/*B9*/ entry_ext_prefix(0xB9, &PrefixExt_0F_B9[0]),
		/*BA*/ entry_ext_prefix(0xBA, &PrefixExt_0F_BA[0]),
		/*BB*/ entry_ext_prefix(0xBB, &PrefixExt_0F_BB[0]),
		/*BC*/ entry_ext_prefix(0xBC, &PrefixExt_0F_BC[0]),
		/*BD*/ entry_ext_prefix(0xBD, &PrefixExt_0F_BD[0]),
		/*BE*/ entry_ext_prefix(0xBE, &PrefixExt_0F_BE[0]),
		/*BF*/ entry_ext_prefix(0xBF, &PrefixExt_0F_BF[0]),

		/*C0*/ entry_ext_prefix(0xC0, &PrefixExt_0F_C0[0]),
		/*C1*/ entry_ext_prefix(0xC1, &PrefixExt_0F_C1[0]),
		/*C2*/ entry_ext_prefix(0xC2, &PrefixExt_0F_C2[0]),
		/*C3*/ entry_ext_prefix(0xC3, &PrefixExt_0F_C3[0]),
		/*C4*/ entry_ext_prefix(0xC4, &PrefixExt_0F_C4[0]),
		/*C5*/ entry_ext_prefix(0xC5, &PrefixExt_0F_C5[0]),
		/*C6*/ entry_ext_prefix(0xC6, &PrefixExt_0F_C6[0]),
		/*C7*/ entry_ext_prefix(0xC7, &PrefixExt_0F_C7[0]),

		/*C8*/ entry_lnri_1(0xC8, ops::BSWAP, OpKind::RegMem32),
		/*C9*/ entry_lnri_1(0xC9, ops::BSWAP, OpKind::RegMem32),
		/*CA*/ entry_lnri_1(0xCA, ops::BSWAP, OpKind::RegMem32),
		/*CB*/ entry_lnri_1(0xCB, ops::BSWAP, OpKind::RegMem32),
		/*CC*/ entry_lnri_1(0xCC, ops::BSWAP, OpKind::RegMem32),
		/*CD*/ entry_lnri_1(0xCD, ops::BSWAP, OpKind::RegMem32),
		/*CE*/ entry_lnri_1(0xCE, ops::BSWAP, OpKind::RegMem32),
		/*CF*/ entry_lnri_1(0xCF, ops::BSWAP, OpKind::RegMem32),

		/*D0*/ entry_ext_prefix(0xD0, &PrefixExt_0F_D0[0]),
		/*D1*/ entry_ext_prefix(0xD1, &PrefixExt_0F_D1[0]),
		/*D2*/ entry_ext_prefix(0xD2, &PrefixExt_0F_D2[0]),
		/*D3*/ entry_ext_prefix(0xD3, &PrefixExt_0F_D3[0]),
		/*D4*/ entry_ext_prefix(0xD4, &PrefixExt_0F_D4[0]),
		/*D5*/ entry_ext_prefix(0xD5, &PrefixExt_0F_D5[0]),
		/*D6*/ entry_ext_prefix(0xD6, &PrefixExt_0F_D6[0]),
		/*D7*/ entry_ext_prefix(0xD7, &PrefixExt_0F_D7[0]),

		/*D8*/ entry_ext_prefix(0xD8, &PrefixExt_0F_D8[0]),
		/*D9*/ entry_ext_prefix(0xD9, &PrefixExt_0F_D9[0]),
		/*DA*/ entry_ext_prefix(0xDA, &PrefixExt_0F_DA[0]),
		/*DB*/ entry_ext_prefix(0xDB, &PrefixExt_0F_DB[0]),
		/*DC*/ entry_ext_prefix(0xDC, &PrefixExt_0F_DC[0]),
		/*DD*/ entry_ext_prefix(0xDD, &PrefixExt_0F_DD[0]),
		/*DE*/ entry_ext_prefix(0xDE, &PrefixExt_0F_DE[0]),
		/*DF*/ entry_ext_prefix(0xDF, &PrefixExt_0F_DF[0]),

		/*E0*/ entry_ext_prefix(0xE0, &PrefixExt_0F_E0[0]),
		/*E1*/ entry_ext_prefix(0xE1, &PrefixExt_0F_E1[0]),
		/*E2*/ entry_ext_prefix(0xE2, &PrefixExt_0F_E2[0]),
		/*E3*/ entry_ext_prefix(0xE3, &PrefixExt_0F_E3[0]),
		/*E4*/ entry_ext_prefix(0xE4, &PrefixExt_0F_E4[0]),
		/*E5*/ entry_ext_prefix(0xE5, &PrefixExt_0F_E5[0]),
		/*E6*/ entry_ext_prefix(0xE6, &PrefixExt_0F_E6[0]),
		/*E7*/ entry_ext_prefix(0xE7, &PrefixExt_0F_E7[0]),

		/*E8*/ entry_ext_prefix(0xE8, &PrefixExt_0F_E8[0]),
		/*E9*/ entry_ext_prefix(0xE9, &PrefixExt_0F_E9[0]),
		/*EA*/ entry_ext_prefix(0xEA, &PrefixExt_0F_EA[0]),
		/*EB*/ entry_ext_prefix(0xEB, &PrefixExt_0F_EB[0]),
		/*EC*/ entry_ext_prefix(0xEC, &PrefixExt_0F_EC[0]),
		/*ED*/ entry_ext_prefix(0xED, &PrefixExt_0F_ED[0]),
		/*EE*/ entry_ext_prefix(0xEE, &PrefixExt_0F_EE[0]),
		/*EF*/ entry_ext_prefix(0xEF, &PrefixExt_0F_EF[0]),

		/*F0*/ entry_ext_prefix(0xF0, &PrefixExt_0F_F0[0]),
		/*F1*/ entry_ext_prefix(0xF1, &PrefixExt_0F_F1[0]),
		/*F2*/ entry_ext_prefix(0xF2, &PrefixExt_0F_F2[0]),
		/*F3*/ entry_ext_prefix(0xF3, &PrefixExt_0F_F3[0]),
		/*F4*/ entry_ext_prefix(0xF4, &PrefixExt_0F_F4[0]),
		/*F5*/ entry_ext_prefix(0xF5, &PrefixExt_0F_F5[0]),
		/*F6*/ entry_ext_prefix(0xF6, &PrefixExt_0F_F6[0]),
		/*F7*/ entry_ext_prefix(0xF7, &PrefixExt_0F_F7[0]),

		/*F8*/ entry_ext_prefix(0xF8, &PrefixExt_0F_F8[0]),
		/*F9*/ entry_ext_prefix(0xF9, &PrefixExt_0F_F9[0]),
		/*FA*/ entry_ext_prefix(0xFA, &PrefixExt_0F_FA[0]),
		/*FB*/ entry_ext_prefix(0xFB, &PrefixExt_0F_FB[0]),
		/*FC*/ entry_ext_prefix(0xFC, &PrefixExt_0F_FC[0]),
		/*FD*/ entry_ext_prefix(0xFD, &PrefixExt_0F_FD[0]),
		/*FE*/ entry_ext_prefix(0xFE, &PrefixExt_0F_FE[0]),

		/*FF*/ entry_0(0xFF, ops::UD0),
	};

	constexpr TableEntry PrefixExt_0F_38_00[] = {
		/*0*/ entry_2(0x00, ops::PSHUFB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x00, ops::PSHUFB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_01[] = {
		/*0*/ entry_2(0x01, ops::PHADDW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x01, ops::PHADDW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_02[] = {
		/*0*/ entry_2(0x02, ops::PHADDD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x02, ops::PHADDD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_03[] = {
		/*0*/ entry_2(0x03, ops::PHADDSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x03, ops::PHADDSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_04[] = {
		/*0*/ entry_2(0x04, ops::PMADDUBSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x04, ops::PMADDUBSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_05[] = {
		/*0*/ entry_2(0x05, ops::PHSUBW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x05, ops::PHSUBW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_06[] = {
		/*0*/ entry_2(0x06, ops::PHSUBD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x06, ops::PHSUBD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_07[] = {
		/*0*/ entry_2(0x07, ops::PHSUBSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x07, ops::PHSUBSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_08[] = {
		/*0*/ entry_2(0x08, ops::PSIGNB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x08, ops::PSIGNB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_09[] = {
		/*0*/ entry_2(0x09, ops::PSIGNW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x09, ops::PSIGNW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_0A[] = {
		/*0*/ entry_2(0x0A, ops::PSIGND, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x0A, ops::PSIGND, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_0B[] = {
		/*0*/ entry_2(0x0B, ops::PMULHRSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x0B, ops::PMULHRSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_10[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x10, ops::PBLENDVB, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::ImplicitXMM0),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_14[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x14, ops::BLENDVPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::ImplicitXMM0),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_15[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x15, ops::BLENDVPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::ImplicitXMM0),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_17[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x17, ops::PTEST, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_1C[] = {
		/*0*/ entry_2(0x1C, ops::PABSB, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x1C, ops::PABSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_1D[] = {
		/*0*/ entry_2(0x1D, ops::PABSW, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x1D, ops::PABSW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_1E[] = {
		/*0*/ entry_2(0x1E, ops::PABSD, OpKind::RegMmx, OpKind::RegMmxMem64),
		/*1*/ entry_2(0x1E, ops::PABSD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_20[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x20, ops::PMOVSXBW, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_21[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x21, ops::PMOVSXBD, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_22[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x22, ops::PMOVSXBQ, OpKind::RegXmm, OpKind::RegXmmMem16),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_23[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x23, ops::PMOVSXWD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_24[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x24, ops::PMOVSXWQ, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_25[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x25, ops::PMOVSXDQ, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_28[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x28, ops::PMULDQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_29[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x29, ops::PCMPEQQ, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_2A[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x2A, ops::MOVNTDQA, OpKind::RegXmm, OpKind::Mem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_2B[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x2B, ops::PACKUSDW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_30[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x30, ops::PMOVZXBW, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_31[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x31, ops::PMOVZXBD, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_32[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x32, ops::PMOVZXBQ, OpKind::RegXmm, OpKind::RegXmmMem16),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_33[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x33, ops::PMOVZXWD, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_34[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x34, ops::PMOVZXWQ, OpKind::RegXmm, OpKind::RegXmmMem32),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_35[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x35, ops::PMOVZXDQ, OpKind::RegXmm, OpKind::RegXmmMem64),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_37[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x37, ops::PCMPGTQ,  OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_38[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x38, ops::PMINSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_39[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x39, ops::PMINSD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3A[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3A, ops::PMINUW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3B[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3B, ops::PMINUD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3C[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3C, ops::PMAXSB, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3D[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3D, ops::PMAXSD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3E[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3E, ops::PMAXUW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_3F[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x3F, ops::PMAXUD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_40[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x40, ops::PMULLD, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_41[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0x41, ops::PHMINPOSUW, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_DB[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xDB, ops::AESIMC, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_DC[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xDC, ops::AESENC, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_DD[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xDD, ops::AESENCLAST, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_DE[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xDE, ops::AESDEC, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_DF[] = {
		/*0*/ entry_blank,
		/*1*/ entry_2(0xDF, ops::AESDECLAST, OpKind::RegXmm, OpKind::RegXmmMem128),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_F0[] = {
		/*0*/ entry_2(0xF0, ops::MOVBE, OpKind::Reg32, OpKind::Mem32),
		/*1*/ entry_blank,
		/*2*/ entry_2(0xF0, ops::CRC32, OpKind::Reg32, OpKind::RegMem8),
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_38_F1[] = {
		/*0*/ entry_2(0xF1, ops::MOVBE, OpKind::Mem32, OpKind::Reg32),
		/*1*/ entry_blank,
		/*2*/ entry_2(0xF1, ops::CRC32, OpKind::Reg32, OpKind::RegMem32),
		/*3*/ entry_blank,
	};

	constexpr TableEntry SecondaryOpcodeMap_0F_38[] = {
		/*00*/ entry_ext_prefix(0x00, &PrefixExt_0F_38_00[0]),
		/*01*/ entry_ext_prefix(0x01, &PrefixExt_0F_38_01[0]),
		/*02*/ entry_ext_prefix(0x02, &PrefixExt_0F_38_02[0]),
		/*03*/ entry_ext_prefix(0x03, &PrefixExt_0F_38_03[0]),
		/*04*/ entry_ext_prefix(0x04, &PrefixExt_0F_38_04[0]),
		/*05*/ entry_ext_prefix(0x05, &PrefixExt_0F_38_05[0]),
		/*06*/ entry_ext_prefix(0x06, &PrefixExt_0F_38_06[0]),
		/*07*/ entry_ext_prefix(0x07, &PrefixExt_0F_38_07[0]),

		/*08*/ entry_ext_prefix(0x08, &PrefixExt_0F_38_08[0]),
		/*09*/ entry_ext_prefix(0x09, &PrefixExt_0F_38_09[0]),
		/*0A*/ entry_ext_prefix(0x0A, &PrefixExt_0F_38_0A[0]),
		/*0B*/ entry_ext_prefix(0x0B, &PrefixExt_0F_38_0B[0]),
		/*0C*/ entry_blank,
		/*0D*/ entry_blank,
		/*0E*/ entry_blank,
		/*0F*/ entry_blank,

		/*10*/ entry_ext_prefix(0x10, &PrefixExt_0F_38_10[0]),
		/*11*/ entry_blank,
		/*12*/ entry_blank,
		/*13*/ entry_blank,
		/*14*/ entry_ext_prefix(0x14, &PrefixExt_0F_38_14[0]),
		/*15*/ entry_ext_prefix(0x15, &PrefixExt_0F_38_15[0]),
		/*16*/ entry_blank,
		/*17*/ entry_ext_prefix(0x17, &PrefixExt_0F_38_17[0]),

		/*18*/ entry_blank,
		/*19*/ entry_blank,
		/*1A*/ entry_blank,
		/*1B*/ entry_blank,
		/*1C*/ entry_ext_prefix(0x1C, &PrefixExt_0F_38_1C[0]),
		/*1D*/ entry_ext_prefix(0x1D, &PrefixExt_0F_38_1D[0]),
		/*1E*/ entry_ext_prefix(0x1E, &PrefixExt_0F_38_1E[0]),
		/*1F*/ entry_blank,

		/*20*/ entry_ext_prefix(0x20, &PrefixExt_0F_38_20[0]),
		/*21*/ entry_ext_prefix(0x21, &PrefixExt_0F_38_21[0]),
		/*22*/ entry_ext_prefix(0x22, &PrefixExt_0F_38_22[0]),
		/*23*/ entry_ext_prefix(0x23, &PrefixExt_0F_38_23[0]),
		/*24*/ entry_ext_prefix(0x24, &PrefixExt_0F_38_24[0]),
		/*25*/ entry_ext_prefix(0x25, &PrefixExt_0F_38_25[0]),
		/*26*/ entry_blank,
		/*27*/ entry_blank,

		/*28*/ entry_ext_prefix(0x28, &PrefixExt_0F_38_28[0]),
		/*29*/ entry_ext_prefix(0x29, &PrefixExt_0F_38_29[0]),
		/*2A*/ entry_ext_prefix(0x2A, &PrefixExt_0F_38_2A[0]),
		/*2B*/ entry_ext_prefix(0x2B, &PrefixExt_0F_38_2B[0]),
		/*2C*/ entry_blank,
		/*2D*/ entry_blank,
		/*2E*/ entry_blank,
		/*2F*/ entry_blank,

		/*30*/ entry_ext_prefix(0x30, &PrefixExt_0F_38_30[0]),
		/*31*/ entry_ext_prefix(0x31, &PrefixExt_0F_38_31[0]),
		/*32*/ entry_ext_prefix(0x32, &PrefixExt_0F_38_32[0]),
		/*33*/ entry_ext_prefix(0x33, &PrefixExt_0F_38_33[0]),
		/*34*/ entry_ext_prefix(0x34, &PrefixExt_0F_38_34[0]),
		/*35*/ entry_ext_prefix(0x35, &PrefixExt_0F_38_35[0]),
		/*36*/ entry_blank,
		/*37*/ entry_ext_prefix(0x37, &PrefixExt_0F_38_37[0]),

		/*38*/ entry_ext_prefix(0x38, &PrefixExt_0F_38_38[0]),
		/*39*/ entry_ext_prefix(0x39, &PrefixExt_0F_38_39[0]),
		/*3A*/ entry_ext_prefix(0x3A, &PrefixExt_0F_38_3A[0]),
		/*3B*/ entry_ext_prefix(0x3B, &PrefixExt_0F_38_3B[0]),
		/*3C*/ entry_ext_prefix(0x3C, &PrefixExt_0F_38_3C[0]),
		/*3D*/ entry_ext_prefix(0x3D, &PrefixExt_0F_38_3D[0]),
		/*3E*/ entry_ext_prefix(0x3E, &PrefixExt_0F_38_3E[0]),
		/*3F*/ entry_ext_prefix(0x3F, &PrefixExt_0F_38_3F[0]),

		/*40*/ entry_ext_prefix(0x40, &PrefixExt_0F_38_40[0]),
		/*41*/ entry_ext_prefix(0x41, &PrefixExt_0F_38_41[0]),
		/*42*/ entry_blank,
		/*43*/ entry_blank,
		/*44*/ entry_blank,
		/*45*/ entry_blank,
		/*46*/ entry_blank,
		/*47*/ entry_blank,

		/*48*/ entry_blank,
		/*49*/ entry_blank,
		/*4A*/ entry_blank,
		/*4B*/ entry_blank,
		/*4C*/ entry_blank,
		/*4D*/ entry_blank,
		/*4E*/ entry_blank,
		/*4F*/ entry_blank,
		/*50*/ entry_blank,
		/*51*/ entry_blank,
		/*52*/ entry_blank,
		/*53*/ entry_blank,
		/*54*/ entry_blank,
		/*55*/ entry_blank,
		/*56*/ entry_blank,
		/*57*/ entry_blank,
		/*58*/ entry_blank,
		/*59*/ entry_blank,
		/*5A*/ entry_blank,
		/*5B*/ entry_blank,
		/*5C*/ entry_blank,
		/*5D*/ entry_blank,
		/*5E*/ entry_blank,
		/*5F*/ entry_blank,
		/*60*/ entry_blank,
		/*61*/ entry_blank,
		/*62*/ entry_blank,
		/*63*/ entry_blank,
		/*64*/ entry_blank,
		/*65*/ entry_blank,
		/*66*/ entry_blank,
		/*67*/ entry_blank,
		/*68*/ entry_blank,
		/*69*/ entry_blank,
		/*6A*/ entry_blank,
		/*6B*/ entry_blank,
		/*6C*/ entry_blank,
		/*6D*/ entry_blank,
		/*6E*/ entry_blank,
		/*6F*/ entry_blank,
		/*70*/ entry_blank,
		/*71*/ entry_blank,
		/*72*/ entry_blank,
		/*73*/ entry_blank,
		/*74*/ entry_blank,
		/*75*/ entry_blank,
		/*76*/ entry_blank,
		/*77*/ entry_blank,
		/*78*/ entry_blank,
		/*79*/ entry_blank,
		/*7A*/ entry_blank,
		/*7B*/ entry_blank,
		/*7C*/ entry_blank,
		/*7D*/ entry_blank,
		/*7E*/ entry_blank,
		/*7F*/ entry_blank,
		/*80*/ entry_blank,
		/*81*/ entry_blank,
		/*82*/ entry_blank,
		/*83*/ entry_blank,
		/*84*/ entry_blank,
		/*85*/ entry_blank,
		/*86*/ entry_blank,
		/*87*/ entry_blank,
		/*88*/ entry_blank,
		/*89*/ entry_blank,
		/*8A*/ entry_blank,
		/*8B*/ entry_blank,
		/*8C*/ entry_blank,
		/*8D*/ entry_blank,
		/*8E*/ entry_blank,
		/*8F*/ entry_blank,
		/*90*/ entry_blank,
		/*91*/ entry_blank,
		/*92*/ entry_blank,
		/*93*/ entry_blank,
		/*94*/ entry_blank,
		/*95*/ entry_blank,
		/*96*/ entry_blank,
		/*97*/ entry_blank,
		/*98*/ entry_blank,
		/*99*/ entry_blank,
		/*9A*/ entry_blank,
		/*9B*/ entry_blank,
		/*9C*/ entry_blank,
		/*9D*/ entry_blank,
		/*9E*/ entry_blank,
		/*9F*/ entry_blank,
		/*A0*/ entry_blank,
		/*A1*/ entry_blank,
		/*A2*/ entry_blank,
		/*A3*/ entry_blank,
		/*A4*/ entry_blank,
		/*A5*/ entry_blank,
		/*A6*/ entry_blank,
		/*A7*/ entry_blank,
		/*A8*/ entry_blank,
		/*A9*/ entry_blank,
		/*AA*/ entry_blank,
		/*AB*/ entry_blank,
		/*AC*/ entry_blank,
		/*AD*/ entry_blank,
		/*AE*/ entry_blank,
		/*AF*/ entry_blank,
		/*B0*/ entry_blank,
		/*B1*/ entry_blank,
		/*B2*/ entry_blank,
		/*B3*/ entry_blank,
		/*B4*/ entry_blank,
		/*B5*/ entry_blank,
		/*B6*/ entry_blank,
		/*B7*/ entry_blank,
		/*B8*/ entry_blank,
		/*B9*/ entry_blank,
		/*BA*/ entry_blank,
		/*BB*/ entry_blank,
		/*BC*/ entry_blank,
		/*BD*/ entry_blank,
		/*BE*/ entry_blank,
		/*BF*/ entry_blank,
		/*C0*/ entry_blank,
		/*C1*/ entry_blank,
		/*C2*/ entry_blank,
		/*C3*/ entry_blank,
		/*C4*/ entry_blank,
		/*C5*/ entry_blank,
		/*C6*/ entry_blank,
		/*C7*/ entry_blank,
		/*C8*/ entry_blank,
		/*C9*/ entry_blank,
		/*CA*/ entry_blank,
		/*CB*/ entry_blank,
		/*CC*/ entry_blank,
		/*CD*/ entry_blank,
		/*CE*/ entry_blank,
		/*CF*/ entry_blank,
		/*D0*/ entry_blank,
		/*D1*/ entry_blank,
		/*D2*/ entry_blank,
		/*D3*/ entry_blank,
		/*D4*/ entry_blank,
		/*D5*/ entry_blank,
		/*D6*/ entry_blank,
		/*D7*/ entry_blank,

		/*D8*/ entry_blank,
		/*D9*/ entry_blank,
		/*DA*/ entry_blank,
		/*DB*/ entry_ext_prefix(0xDB, &PrefixExt_0F_38_DB[0]),
		/*DC*/ entry_ext_prefix(0xDC, &PrefixExt_0F_38_DC[0]),
		/*DD*/ entry_ext_prefix(0xDD, &PrefixExt_0F_38_DD[0]),
		/*DE*/ entry_ext_prefix(0xDE, &PrefixExt_0F_38_DE[0]),
		/*DF*/ entry_ext_prefix(0xDF, &PrefixExt_0F_38_DF[0]),

		/*E0*/ entry_blank,
		/*E1*/ entry_blank,
		/*E2*/ entry_blank,
		/*E3*/ entry_blank,
		/*E4*/ entry_blank,
		/*E5*/ entry_blank,
		/*E6*/ entry_blank,
		/*E7*/ entry_blank,
		/*E8*/ entry_blank,
		/*E9*/ entry_blank,
		/*EA*/ entry_blank,
		/*EB*/ entry_blank,
		/*EC*/ entry_blank,
		/*ED*/ entry_blank,
		/*EE*/ entry_blank,
		/*EF*/ entry_blank,

		/*F0*/ entry_ext_prefix(0xF0, &PrefixExt_0F_38_F0[0]),
		/*F1*/ entry_ext_prefix(0xF1, &PrefixExt_0F_38_F1[0]),
		/*F2*/ entry_blank,
		/*F3*/ entry_blank,
		/*F4*/ entry_blank,
		/*F5*/ entry_blank,
		/*F6*/ entry_blank,
		/*F7*/ entry_blank,

		/*F8*/ entry_blank,
		/*F9*/ entry_blank,
		/*FA*/ entry_blank,
		/*FB*/ entry_blank,
		/*FC*/ entry_blank,
		/*FD*/ entry_blank,
		/*FE*/ entry_blank,
		/*FF*/ entry_blank,
	};




	constexpr TableEntry ModRMExt_0F_3A_Prefix66_22_RexW[] = {
		/*0*/ entry_3(0x22, ops::PINSRD, OpKind::RegXmm, OpKind::RegMem32, OpKind::Imm8),   // rex.W == 0
		/*1*/ entry_3(0x22, ops::PINSRQ, OpKind::RegXmm, OpKind::RegMem64, OpKind::Imm8)    // rex.W == 1
	};

	constexpr TableEntry PrefixExt_0F_3A_08[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x08, ops::ROUNDPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_09[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x09, ops::ROUNDPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0A[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x0A, ops::ROUNDSS, OpKind::RegXmm, OpKind::RegXmmMem32, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0B[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x0B, ops::ROUNDSD, OpKind::RegXmm, OpKind::RegXmmMem64, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0C[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x0C, ops::BLENDPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0D[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x0D, ops::BLENDPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0E[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x0E, ops::PBLENDW, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_0F[] = {
		/*0*/ entry_3(0x0F, ops::PALIGNR, OpKind::RegMmx, OpKind::RegMmxMem64, OpKind::Imm8),
		/*1*/ entry_3(0x0F, ops::PALIGNR, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_14[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x14, ops::PEXTRB, OpKind::RegMem8, OpKind::RegXmm, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_15[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x15, ops::PEXTRW, OpKind::RegMem16, OpKind::RegXmm, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_16[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x16, ops::PEXTRD, OpKind::RegMem32, OpKind::RegXmm, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_17[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x17, ops::EXTRACTPS, OpKind::RegMem32, OpKind::RegXmm, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_20[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x20, ops::PINSRB, OpKind::RegXmm, OpKind::Reg32Mem8, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_21[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x21, ops::PINSRW, OpKind::RegXmm, OpKind::RegXmmMem32, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_22[] = {
		/*0*/ entry_blank,
		/*1*/ entry_ext_rexw(0x22, &ModRMExt_0F_3A_Prefix66_22_RexW[0]),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_40[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x40, ops::DPPS, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_41[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x41, ops::DPPD, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_42[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x42, ops::MPSADBW, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_44[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x44, ops::PCLMULQDQ, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_60[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x60, ops::PCMPESTRM, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_61[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x61, ops::PCMPESTRI, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_62[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x62, ops::PCMPISTRM, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_63[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0x63, ops::PCMPISTRI, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry PrefixExt_0F_3A_DF[] = {
		/*0*/ entry_blank,
		/*1*/ entry_3(0xDF, ops::AESKEYGENASSIST, OpKind::RegXmm, OpKind::RegXmmMem128, OpKind::Imm8),
		/*2*/ entry_blank,
		/*3*/ entry_blank,
	};
	constexpr TableEntry SecondaryOpcodeMap_0F_3A[] = {
		/*00*/ entry_blank,
		/*01*/ entry_blank,
		/*02*/ entry_blank,
		/*03*/ entry_blank,
		/*04*/ entry_blank,
		/*05*/ entry_blank,
		/*06*/ entry_blank,
		/*07*/ entry_blank,

		/*08*/ entry_ext_prefix(0x08, &PrefixExt_0F_3A_08[0]),
		/*09*/ entry_ext_prefix(0x09, &PrefixExt_0F_3A_09[0]),
		/*0A*/ entry_ext_prefix(0x0A, &PrefixExt_0F_3A_0A[0]),
		/*0B*/ entry_ext_prefix(0x0B, &PrefixExt_0F_3A_0B[0]),
		/*0C*/ entry_ext_prefix(0x0C, &PrefixExt_0F_3A_0C[0]),
		/*0D*/ entry_ext_prefix(0x0D, &PrefixExt_0F_3A_0D[0]),
		/*0E*/ entry_ext_prefix(0x0E, &PrefixExt_0F_3A_0E[0]),
		/*0F*/ entry_ext_prefix(0x0F, &PrefixExt_0F_3A_0F[0]),

		/*10*/ entry_blank,
		/*11*/ entry_blank,
		/*12*/ entry_blank,
		/*13*/ entry_blank,
		/*14*/ entry_ext_prefix(0x14, &PrefixExt_0F_3A_14[0]),
		/*15*/ entry_ext_prefix(0x15, &PrefixExt_0F_3A_15[0]),
		/*16*/ entry_ext_prefix(0x16, &PrefixExt_0F_3A_16[0]),
		/*17*/ entry_ext_prefix(0x17, &PrefixExt_0F_3A_17[0]),

		/*18*/ entry_blank,
		/*19*/ entry_blank,
		/*1A*/ entry_blank,
		/*1B*/ entry_blank,
		/*1C*/ entry_blank,
		/*1D*/ entry_blank,
		/*1E*/ entry_blank,
		/*1F*/ entry_blank,

		/*20*/ entry_ext_prefix(0x20, &PrefixExt_0F_3A_20[0]),
		/*21*/ entry_ext_prefix(0x21, &PrefixExt_0F_3A_21[0]),
		/*22*/ entry_ext_prefix(0x22, &PrefixExt_0F_3A_22[0]),
		/*23*/ entry_blank,
		/*24*/ entry_blank,
		/*25*/ entry_blank,
		/*26*/ entry_blank,
		/*27*/ entry_blank,

		/*28*/ entry_blank,
		/*29*/ entry_blank,
		/*2A*/ entry_blank,
		/*2B*/ entry_blank,
		/*2C*/ entry_blank,
		/*2D*/ entry_blank,
		/*2E*/ entry_blank,
		/*2F*/ entry_blank,
		/*30*/ entry_blank,
		/*31*/ entry_blank,
		/*32*/ entry_blank,
		/*33*/ entry_blank,
		/*34*/ entry_blank,
		/*35*/ entry_blank,
		/*36*/ entry_blank,
		/*37*/ entry_blank,
		/*38*/ entry_blank,
		/*39*/ entry_blank,
		/*3A*/ entry_blank,
		/*3B*/ entry_blank,
		/*3C*/ entry_blank,
		/*3D*/ entry_blank,
		/*3E*/ entry_blank,
		/*3F*/ entry_blank,

		/*40*/ entry_ext_prefix(0x40, &PrefixExt_0F_3A_40[0]),
		/*41*/ entry_ext_prefix(0x41, &PrefixExt_0F_3A_41[0]),
		/*42*/ entry_ext_prefix(0x42, &PrefixExt_0F_3A_42[0]),
		/*43*/ entry_blank,
		/*44*/ entry_ext_prefix(0x44, &PrefixExt_0F_3A_44[0]),
		/*45*/ entry_blank,
		/*46*/ entry_blank,
		/*47*/ entry_blank,

		/*48*/ entry_blank,
		/*49*/ entry_blank,
		/*4A*/ entry_blank,
		/*4B*/ entry_blank,
		/*4C*/ entry_blank,
		/*4D*/ entry_blank,
		/*4E*/ entry_blank,
		/*4F*/ entry_blank,

		/*50*/ entry_blank,
		/*51*/ entry_blank,
		/*52*/ entry_blank,
		/*53*/ entry_blank,
		/*54*/ entry_blank,
		/*55*/ entry_blank,
		/*56*/ entry_blank,
		/*57*/ entry_blank,
		/*58*/ entry_blank,
		/*59*/ entry_blank,
		/*5A*/ entry_blank,
		/*5B*/ entry_blank,
		/*5C*/ entry_blank,
		/*5D*/ entry_blank,
		/*5E*/ entry_blank,
		/*5F*/ entry_blank,

		/*60*/ entry_ext_prefix(0x60, &PrefixExt_0F_3A_60[0]),
		/*61*/ entry_ext_prefix(0x61, &PrefixExt_0F_3A_61[0]),
		/*62*/ entry_ext_prefix(0x62, &PrefixExt_0F_3A_62[0]),
		/*63*/ entry_ext_prefix(0x63, &PrefixExt_0F_3A_63[0]),
		/*64*/ entry_blank,
		/*65*/ entry_blank,
		/*66*/ entry_blank,
		/*67*/ entry_blank,

		/*68*/ entry_blank,
		/*69*/ entry_blank,
		/*6A*/ entry_blank,
		/*6B*/ entry_blank,
		/*6C*/ entry_blank,
		/*6D*/ entry_blank,
		/*6E*/ entry_blank,
		/*6F*/ entry_blank,
		/*70*/ entry_blank,
		/*71*/ entry_blank,
		/*72*/ entry_blank,
		/*73*/ entry_blank,
		/*74*/ entry_blank,
		/*75*/ entry_blank,
		/*76*/ entry_blank,
		/*77*/ entry_blank,
		/*78*/ entry_blank,
		/*79*/ entry_blank,
		/*7A*/ entry_blank,
		/*7B*/ entry_blank,
		/*7C*/ entry_blank,
		/*7D*/ entry_blank,
		/*7E*/ entry_blank,
		/*7F*/ entry_blank,
		/*80*/ entry_blank,
		/*81*/ entry_blank,
		/*82*/ entry_blank,
		/*83*/ entry_blank,
		/*84*/ entry_blank,
		/*85*/ entry_blank,
		/*86*/ entry_blank,
		/*87*/ entry_blank,
		/*88*/ entry_blank,
		/*89*/ entry_blank,
		/*8A*/ entry_blank,
		/*8B*/ entry_blank,
		/*8C*/ entry_blank,
		/*8D*/ entry_blank,
		/*8E*/ entry_blank,
		/*8F*/ entry_blank,
		/*90*/ entry_blank,
		/*91*/ entry_blank,
		/*92*/ entry_blank,
		/*93*/ entry_blank,
		/*94*/ entry_blank,
		/*95*/ entry_blank,
		/*96*/ entry_blank,
		/*97*/ entry_blank,
		/*98*/ entry_blank,
		/*99*/ entry_blank,
		/*9A*/ entry_blank,
		/*9B*/ entry_blank,
		/*9C*/ entry_blank,
		/*9D*/ entry_blank,
		/*9E*/ entry_blank,
		/*9F*/ entry_blank,
		/*A0*/ entry_blank,
		/*A1*/ entry_blank,
		/*A2*/ entry_blank,
		/*A3*/ entry_blank,
		/*A4*/ entry_blank,
		/*A5*/ entry_blank,
		/*A6*/ entry_blank,
		/*A7*/ entry_blank,
		/*A8*/ entry_blank,
		/*A9*/ entry_blank,
		/*AA*/ entry_blank,
		/*AB*/ entry_blank,
		/*AC*/ entry_blank,
		/*AD*/ entry_blank,
		/*AE*/ entry_blank,
		/*AF*/ entry_blank,
		/*B0*/ entry_blank,
		/*B1*/ entry_blank,
		/*B2*/ entry_blank,
		/*B3*/ entry_blank,
		/*B4*/ entry_blank,
		/*B5*/ entry_blank,
		/*B6*/ entry_blank,
		/*B7*/ entry_blank,
		/*B8*/ entry_blank,
		/*B9*/ entry_blank,
		/*BA*/ entry_blank,
		/*BB*/ entry_blank,
		/*BC*/ entry_blank,
		/*BD*/ entry_blank,
		/*BE*/ entry_blank,
		/*BF*/ entry_blank,
		/*C0*/ entry_blank,
		/*C1*/ entry_blank,
		/*C2*/ entry_blank,
		/*C3*/ entry_blank,
		/*C4*/ entry_blank,
		/*C5*/ entry_blank,
		/*C6*/ entry_blank,
		/*C7*/ entry_blank,
		/*C8*/ entry_blank,
		/*C9*/ entry_blank,
		/*CA*/ entry_blank,
		/*CB*/ entry_blank,
		/*CC*/ entry_blank,
		/*CD*/ entry_blank,
		/*CE*/ entry_blank,
		/*CF*/ entry_blank,
		/*D0*/ entry_blank,
		/*D1*/ entry_blank,
		/*D2*/ entry_blank,
		/*D3*/ entry_blank,
		/*D4*/ entry_blank,
		/*D5*/ entry_blank,
		/*D6*/ entry_blank,
		/*D7*/ entry_blank,
		/*D8*/ entry_blank,
		/*D9*/ entry_blank,
		/*DA*/ entry_blank,
		/*DB*/ entry_blank,
		/*DC*/ entry_blank,
		/*DD*/ entry_blank,
		/*DE*/ entry_blank,
		/*DF*/ entry_ext_prefix(0xDF, &PrefixExt_0F_3A_DF[0]),

		/*E0*/ entry_blank,
		/*E1*/ entry_blank,
		/*E2*/ entry_blank,
		/*E3*/ entry_blank,
		/*E4*/ entry_blank,
		/*E5*/ entry_blank,
		/*E6*/ entry_blank,
		/*E7*/ entry_blank,
		/*E8*/ entry_blank,
		/*E9*/ entry_blank,
		/*EA*/ entry_blank,
		/*EB*/ entry_blank,
		/*EC*/ entry_blank,
		/*ED*/ entry_blank,
		/*EE*/ entry_blank,
		/*EF*/ entry_blank,
		/*F0*/ entry_blank,
		/*F1*/ entry_blank,
		/*F2*/ entry_blank,
		/*F3*/ entry_blank,
		/*F4*/ entry_blank,
		/*F5*/ entry_blank,
		/*F6*/ entry_blank,
		/*F7*/ entry_blank,
		/*F8*/ entry_blank,
		/*F9*/ entry_blank,
		/*FA*/ entry_blank,
		/*FB*/ entry_blank,
		/*FC*/ entry_blank,
		/*FD*/ entry_blank,
		/*FE*/ entry_blank,
		/*FF*/ entry_blank,
	};
}
