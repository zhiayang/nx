// x87.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "entry.h"

namespace instrad::x64::tables
{
	// ok, so this is quite messy. the x87 instruction set will need 2-level extension tables
	// for most opcodes; from the AMD manuals, the row hinges on modRM.reg, and the column
	// hinges on modRM.rm
	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg0_RM[] = {
		/*0*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FADD, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FMUL, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg4_RM[] = {
		/*0*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FSUB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg5_RM[] = {
		/*0*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FSUBR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg6_RM[] = {
		/*0*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FDIV, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::Mem32), // modRM.mod != 3

		/*1*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD8, ops::FDIVR, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D8[] = {
		/*0*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xD8, &ModRMExt_x87_D8_Mod3_Reg7_RM[0]),
	};










	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg0_RM[] = {
		/*0*/ entry_1(0xD9, ops::FLD, OpKind::Mem32),

		/*1*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*2*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*3*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*4*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*5*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*6*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*7*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
		/*8*/ entry_1(0xD9, ops::FLD, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg1_RM[] = {
		/*0*/ entry_blank,

		/*1*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xD9, ops::FXCH, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xD9, ops::FST, OpKind::Mem32, OpKind::ImplicitST0),

		/*1*/ entry_0(0xD9, ops::FNOP),

		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xD9, ops::FSTP, OpKind::Mem32, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg4_RM[] = {
		/*0*/ entry_1(0xD9, ops::FLDENV, OpKind::Memory),

		/*1*/ entry_1(0xD9, ops::FCHS, OpKind::ImplicitST0),
		/*2*/ entry_1(0xD9, ops::FABS, OpKind::ImplicitST0),
		/*3*/ entry_blank,
		/*4*/ entry_1(0xD9, ops::FTST, OpKind::ImplicitST0),
		/*5*/ entry_1(0xD9, ops::FXAM, OpKind::ImplicitST0),

		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg5_RM[] = {
		/*0*/ entry_1(0xD9, ops::FLDCW, OpKind::Memory),

		/*1*/ entry_0(0xD9, ops::FLD1),
		/*2*/ entry_0(0xD9, ops::FLDL2T),
		/*3*/ entry_0(0xD9, ops::FLDL2E),
		/*4*/ entry_0(0xD9, ops::FLDPI),
		/*5*/ entry_0(0xD9, ops::FLDLG2),
		/*6*/ entry_0(0xD9, ops::FLDLN2),
		/*7*/ entry_0(0xD9, ops::FLDZ),

		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg6_RM[] = {
		/*0*/ entry_1(0xD9, ops::FNSTENV, OpKind::Memory),

		/*1*/ entry_1(0xD9, ops::F2XM1, OpKind::ImplicitST0),
		/*2*/ entry_1(0xD9, ops::FYL2X, OpKind::ImplicitST1),
		/*3*/ entry_1(0xD9, ops::FPTAN, OpKind::ImplicitST0),
		/*4*/ entry_1(0xD9, ops::FPATAN, OpKind::ImplicitST0),
		/*5*/ entry_1(0xD9, ops::FXTRACT, OpKind::ImplicitST0),
		/*6*/ entry_2(0xD9, ops::FPREM1, OpKind::ImplicitST0, OpKind::ImplicitST1),
		/*7*/ entry_0(0xD9, ops::FDECSTP),
		/*8*/ entry_0(0xD9, ops::FINCSTP),
	};

	constexpr TableEntry ModRMExt_x87_D9_Mod3_Reg7_RM[] = {
		/*0*/ entry_1(0xD9, ops::FNSTCW, OpKind::Memory),

		/*1*/ entry_2(0xD9, ops::FPREM, OpKind::ImplicitST0, OpKind::ImplicitST1),
		/*2*/ entry_1(0xD9, ops::FYL2XP1, OpKind::ImplicitST1),
		/*3*/ entry_1(0xD9, ops::FSQRT, OpKind::ImplicitST0),
		/*4*/ entry_1(0xD9, ops::FSINCOS, OpKind::ImplicitST0),
		/*5*/ entry_1(0xD9, ops::FRNDINT, OpKind::ImplicitST0),
		/*6*/ entry_1(0xD9, ops::FSCALE, OpKind::ImplicitST0),
		/*7*/ entry_1(0xD9, ops::FSIN, OpKind::ImplicitST0),
		/*8*/ entry_1(0xD9, ops::FCOS, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_D9[] = {
		/*0*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xD9, &ModRMExt_x87_D9_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg0_RM[] = {
		/*0*/ entry_2(0xDA, ops::FIADD, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDA, ops::FCMOVB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDA, ops::FIMUL, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDA, ops::FCMOVE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDA, ops::FICOM, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDA, ops::FCMOVBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDA, ops::FICOMP, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDA, ops::FCMOVU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg4_RM[] = {
		/*0*/ entry_2(0xDA, ops::FISUB, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg5_RM[] = {
		/*0*/ entry_2(0xDA, ops::FISUBR, OpKind::ImplicitST0, OpKind::Mem32),
		/*1*/ entry_blank,
		/*2*/ entry_2(0xDA, ops::FUCOMPP, OpKind::ImplicitST0, OpKind::ImplicitST1),

		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg6_RM[] = {
		/*0*/ entry_2(0xDA, ops::FIDIV, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DA_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xDA, ops::FIDIVR, OpKind::ImplicitST0, OpKind::Mem32),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};


	constexpr TableEntry ModRMExt_x87_DA[] = {
		/*0*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDA, &ModRMExt_x87_DA_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg0_RM[] = {
		/*0*/ entry_1(0xDB, ops::FILD, OpKind::Mem32),

		/*1*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FCMOVNB, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDB, ops::FISTTP, OpKind::Mem32, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FCMOVNE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDB, ops::FIST, OpKind::Mem32, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FCMOVNBE, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDB, ops::FISTP, OpKind::Mem32, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FCMOVNU, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg4_RM[] = {
		/*0*/ entry_blank,
		/*1*/ entry_blank,
		/*2*/ entry_blank,

		/*3*/ entry_0(0xDB, ops::FNCLEX),
		/*4*/ entry_0(0xDB, ops::FNINIT),

		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg5_RM[] = {
		/*0*/ entry_1(0xDB, ops::FLD, OpKind::Mem80),

		/*1*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FUCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg6_RM[] = {
		/*0*/ entry_blank,

		/*1*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDB, ops::FCOMI, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DB_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xDB, ops::FSTP, OpKind::Mem80, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DB[] = {
		/*0*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDB, &ModRMExt_x87_DB_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg0_RM[] = {
		/*0*/ entry_2(0xDC, ops::FADD, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FADD, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDC, ops::FMUL, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FMUL, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDC, ops::FCOM, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDC, ops::FCOMP, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg4_RM[] = {
		/*0*/ entry_2(0xDC, ops::FSUB, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FSUBR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg5_RM[] = {
		/*0*/ entry_2(0xDC, ops::FSUBR, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FSUB, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg6_RM[] = {
		/*0*/ entry_2(0xDC, ops::FDIV, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FDIVR, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xDC, ops::FDIVR, OpKind::ImplicitST0, OpKind::Mem64),

		/*1*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDC, ops::FDIV, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DC[] = {
		/*0*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDC, &ModRMExt_x87_DC_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg0_RM[] = {
		/*0*/ entry_1(0xDD, ops::FLD, OpKind::Mem64),

		/*1*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*2*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*3*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*4*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*5*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*6*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*7*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
		/*8*/ entry_1(0xDD, ops::FFREE, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDD, ops::FISTTP, OpKind::Mem64, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDD, ops::FST, OpKind::Mem64, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDD, ops::FST, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDD, ops::FSTP, OpKind::Mem64, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDD, ops::FSTP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg4_RM[] = {
		/*0*/ entry_1(0xDD, ops::FRSTOR, OpKind::Memory),

		/*1*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDD, ops::FUCOM, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg5_RM[] = {
		/*0*/ entry_blank,

		/*1*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDD, ops::FUCOMP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg6_RM[] = {
		/*0*/ entry_1(0xDD, ops::FNSAVE, OpKind::Memory),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DD_Mod3_Reg7_RM[] = {
		/*0*/ entry_1(0xDD, ops::FNSTSW, OpKind::Memory),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DD[] = {
		/*0*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDD, &ModRMExt_x87_DD_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg0_RM[] = {
		/*0*/ entry_2(0xDE, ops::FIADD, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FADDP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDE, ops::FIMUL, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FMULP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDE, ops::FICOM, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDE, ops::FICOMP, OpKind::ImplicitST0, OpKind::Mem16),
		/*1*/ entry_blank,
		/*2*/ entry_2(0xDE, ops::FCOMPP, OpKind::ImplicitST0, OpKind::ImplicitST1),

		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg4_RM[] = {
		/*0*/ entry_2(0xDE, ops::FISUB, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FSUBRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg5_RM[] = {
		/*0*/ entry_2(0xDE, ops::FISUBR, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FSUBP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg6_RM[] = {
		/*0*/ entry_2(0xDE, ops::FIDIV, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FDIVRP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xDE, ops::FIDIVR, OpKind::ImplicitST0, OpKind::Mem16),

		/*1*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*2*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*3*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*4*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*5*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*6*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*7*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
		/*8*/ entry_2(0xDE, ops::FDIVP, OpKind::RegX87_Rm, OpKind::ImplicitST0),
	};

	constexpr TableEntry ModRMExt_x87_DE[] = {
		/*0*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDE, &ModRMExt_x87_DE_Mod3_Reg7_RM[0]),
	};



	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg0_RM[] = {
		/*0*/ entry_1(0xDF, ops::FILD, OpKind::Mem16),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg1_RM[] = {
		/*0*/ entry_2(0xDF, ops::FISTTP, OpKind::Mem16, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg2_RM[] = {
		/*0*/ entry_2(0xDF, ops::FIST, OpKind::Mem16, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg3_RM[] = {
		/*0*/ entry_2(0xDF, ops::FISTP, OpKind::Mem16, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg4_RM[] = {
		/*0*/ entry_1(0xDF, ops::FBLD, OpKind::Mem80),

		/*1*/ entry_1(0xDF, ops::FNSTSW, OpKind::ImplicitAX),

		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg5_RM[] = {
		/*0*/ entry_1(0xDF, ops::FILD, OpKind::Mem64),

		/*1*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDF, ops::FUCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg6_RM[] = {
		/*0*/ entry_2(0xDF, ops::FBSTP, OpKind::Mem80, OpKind::ImplicitST0),

		/*1*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*2*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*3*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*4*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*5*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*6*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*7*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
		/*8*/ entry_2(0xDF, ops::FCOMIP, OpKind::ImplicitST0, OpKind::RegX87_Rm),
	};

	constexpr TableEntry ModRMExt_x87_DF_Mod3_Reg7_RM[] = {
		/*0*/ entry_2(0xDF, ops::FISTP, OpKind::Mem64, OpKind::ImplicitST0),

		/*1*/ entry_blank,
		/*2*/ entry_blank,
		/*3*/ entry_blank,
		/*4*/ entry_blank,
		/*5*/ entry_blank,
		/*6*/ entry_blank,
		/*7*/ entry_blank,
		/*8*/ entry_blank,
	};

	constexpr TableEntry ModRMExt_x87_DF[] = {
		/*0*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg0_RM[0]),
		/*1*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg1_RM[0]),
		/*2*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg2_RM[0]),
		/*3*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg3_RM[0]),
		/*4*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg4_RM[0]),
		/*5*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg5_RM[0]),
		/*6*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg6_RM[0]),
		/*7*/ entry_ext_rm(0xDF, &ModRMExt_x87_DF_Mod3_Reg7_RM[0]),
	};
}
