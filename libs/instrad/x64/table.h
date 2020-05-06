// table.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "tables/entry.h"

#include "tables/primary.h"
#include "tables/secondary.h"
#include "tables/3dnow.h"
#include "tables/x87.h"
#include "tables/avx.h"


namespace instrad::x64::tables
{
	static_assert(ArrayLength(PrimaryOpcodeMap) == 256, "table invalid");
	static_assert(ArrayLength(ModRMExt_80) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_81) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_82) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_83) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_8F) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_C0) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_C1) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_C6) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_C7) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_D0) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_D1) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_D2) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_D3) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_F6) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_F7) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_FE) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_FF) == 8, "table invalid");


	static_assert(ArrayLength(ModRMExt_0F_Prefix66_71) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_Prefix66_72) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_Prefix66_73) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_Prefix66_78) == 8, "table invalid");

	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_12_Mod3) == 2, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_16_Mod3) == 2, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixAny_C7) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_71) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_72) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_73) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_B9) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_PrefixNone_BA) == 8, "table invalid");

	static_assert(ArrayLength(PrefixExt_0F_10) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_11) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_12) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_13) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_14) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_15) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_16) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_17) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_28) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_29) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_2F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_50) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_51) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_52) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_53) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_54) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_55) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_56) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_57) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_58) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_59) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_5F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_60) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_61) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_62) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_63) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_64) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_65) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_66) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_67) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_68) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_69) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_6F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_70) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_71) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_72) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_73) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_74) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_75) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_76) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_77) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_78) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_79) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_7F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_AE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_B8) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_B9) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BA) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BB) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BC) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BD) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_BF) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C0) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C1) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C2) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C3) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C4) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C5) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C6) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_C7) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D0) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D1) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D2) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D3) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D4) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D5) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D6) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D7) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D8) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_D9) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DA) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DB) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DC) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DD) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_DF) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E0) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E1) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E2) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E3) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E4) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E5) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E6) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E7) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E8) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_E9) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_EA) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_EB) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_EC) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_ED) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_EE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_EF) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F0) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F1) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F2) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F3) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F4) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F5) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F6) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F7) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F8) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_F9) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FA) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FB) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FC) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FD) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_FF) == 4, "table invalid");


	static_assert(ArrayLength(ModRMExt_0F_00) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_01) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_18) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_AE_Prefix_None) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_AE_Prefix_F3) == 8, "table invalid");

	// these need 9 entries; the first entry is used when modRM.mod != 3, and the next 8 are
	// used (and indexed) by modRM.rm when modrm.mod == 3.
	static_assert(ArrayLength(ModRMExt_0F_01_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_01_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_01_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_01_Mod3_Reg7_RM) == 9, "table invalid");

	// these only need two entries; the first one for when modRM.mod != 3, and
	// the second one for when modRM.mod == 3.
	static_assert(ArrayLength(ModRMExt_0F_AE_Mod3_Reg4_RM) == 2, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_AE_Mod3_Reg5_RM) == 2, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_AE_Mod3_Reg6_RM) == 2, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_AE_Mod3_Reg7_RM) == 2, "table invalid");

	static_assert(ArrayLength(SecondaryOpcodeMap_0F) == 256, "table invalid");


	static_assert(ArrayLength(PrefixExt_0F_38_00) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_01) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_02) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_03) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_04) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_05) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_06) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_07) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_08) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_09) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_0A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_0B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_10) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_14) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_15) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_17) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_1C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_1D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_1E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_20) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_21) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_22) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_23) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_24) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_25) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_28) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_29) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_2A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_2B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_30) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_31) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_32) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_33) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_34) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_35) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_37) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_38) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_39) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_3F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_40) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_41) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_DB) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_DC) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_DD) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_DE) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_DF) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_F0) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_38_F1) == 4, "table invalid");
	static_assert(ArrayLength(SecondaryOpcodeMap_0F_38) == 256, "table invalid");


	static_assert(ArrayLength(PrefixExt_0F_3A_08) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_09) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0A) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0B) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0C) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0D) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0E) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_0F) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_14) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_15) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_16) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_17) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_20) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_21) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_22) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_40) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_41) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_42) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_44) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_60) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_61) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_62) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_63) == 4, "table invalid");
	static_assert(ArrayLength(PrefixExt_0F_3A_DF) == 4, "table invalid");
	static_assert(ArrayLength(ModRMExt_0F_3A_Prefix66_22_RexW) == 2, "table invalid");
	static_assert(ArrayLength(SecondaryOpcodeMap_0F_3A) == 256, "table invalid");



	static_assert(ArrayLength(SecondaryOpcodeMap_0F_0F_3DNow) == 256, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_D8) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D8_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_D9) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_D9_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DA) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DA_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DB) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DB_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DC) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DC_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DD) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DD_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DE) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DE_Mod3_Reg7_RM) == 9, "table invalid");

	static_assert(ArrayLength(ModRMExt_x87_DF) == 8, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg0_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg1_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg2_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg3_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg4_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg5_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg6_RM) == 9, "table invalid");
	static_assert(ArrayLength(ModRMExt_x87_DF_Mod3_Reg7_RM) == 9, "table invalid");


	static_assert(ArrayLength(Vex_Map_1_ModRMExt_71) == 8, "table invalid");
	static_assert(ArrayLength(Vex_Map_1_ModRMExt_72) == 8, "table invalid");
	static_assert(ArrayLength(Vex_Map_1_ModRMExt_73) == 8, "table invalid");
	static_assert(ArrayLength(Vex_Map_1_ModRMExt_AE) == 8, "table invalid");
	static_assert(ArrayLength(Vex_Map_2_ModRMExt_F3) == 8, "table invalid");
	static_assert(ArrayLength(VEX_Map_1) == 256, "table invalid");
	static_assert(ArrayLength(VEX_Map_2) == 256, "table invalid");
	static_assert(ArrayLength(VEX_Map_3) == 256, "table invalid");

}
