//
// Created by 甘尧 on 2019-09-24.
//

// Branch
Type(Branches)
INST(B_cond,  "B", (cond_branch, label_cond_branch), "01010100xxxxxxxxxxxxxxxxxxx0xxxx")
INST(BR,      "BR", (Xn),                            "1101011000011111000000xxxxx00000")
INST(BLR,     "BLR", (Xn),                           "1101011000111111000000xxxxx00000")
INST(RET,     "RET", (Xn),                           "1101011001011111000000xxxxx00000")
INST(ERET,    "ERET", (Xn),                          "11010110100111110000001111100000")
INST(DRPS,    "DRPS", (Xn),                          "11010110101111110000001111100000")
INST(B,       "B",    (Xn),                          "000101xxxxxxxxxxxxxxxxxxxxxxxxxx")
INST(BL,      "BL",   (Xn),                          "100101xxxxxxxxxxxxxxxxxxxxxxxxxx")
INST(CBZ_32,  "CBZ", (Wt, label_cond_branch),        "00110100xxxxxxxxxxxxxxxxxxxxxxxx")
INST(CBNZ_32, "CBNZ", (Wt, label_cond_branch),       "00110101xxxxxxxxxxxxxxxxxxxxxxxx")
INST(CBZ_64,  "CBZ", (Xt, label_cond_branch),        "10110100xxxxxxxxxxxxxxxxxxxxxxxx")
INST(CBNZ_64, "CBNZ", (Xt, label_cond_branch),       "10110101xxxxxxxxxxxxxxxxxxxxxxxx")
INST(TBZ,     "TBZ", (Xt, imm_tbz, label_tbz),       "00110110xxxxxxxxxxxxxxxxxxxxxxxx")
INST(TBNZ,    "TBNZ", (Xt, imm_tbz, label_tbz),      "00110111xxxxxxxxxxxxxxxxxxxxxxxx")


// Exception Generating
Type(ExpGen)
INST(SVC, "SVC", (imm_exceptions),                   "110101000000xxxxxxxxxxxxxxx00001")
INST(HVC, "HVC", (imm_exceptions),                   "110101000000xxxxxxxxxxxxxxx00010")
INST(SMC, "SMC", (imm_exceptions),                   "110101000000xxxxxxxxxxxxxxx00011")
INST(BRK, "BRK", (imm_exceptions),                   "110101000010xxxxxxxxxxxxxxx00000")
INST(HLT, "HLT", (imm_exceptions),                   "110101000100xxxxxxxxxxxxxxx00000")
INST(DCPS1, "DCPS1", (imm_exceptions),               "110101001010xxxxxxxxxxxxxxx00001")
INST(DCPS2, "DCPS2", (imm_exceptions),               "110101001010xxxxxxxxxxxxxxx00010")
INST(DCPS3, "DCPS3", (imm_exceptions),               "110101001010xxxxxxxxxxxxxxx00011")


//System
Type(System)
INST(MRS, "MRS", (TODO),                             "110101010011xxxxxxxxxxxxxxxxxxxx")
INST(MSR_imm, "MSR", (TODO),                         "1101010100000xxx0100xxxxxxx11111")
INST(MSR_reg, "MSR", (TODO),                         "110101010001xxxxxxxxxxxxxxxxxxxx")
INST(NOP, "NOP", (),                                 "11010101000000110010000000011111")
INST(YIELD, "YIELD", (),                             "11010101000000110010000000111111")


// Data Proccessing Immediate
// PC-rel. addressing
Type(DataprocessingImmediate)
INST(ADR, "ADR", (Xd, imm_adr),                      "0xx10000xxxxxxxxxxxxxxxxxxxxxxxx")
INST(ADRP, "ADRP", (Xd, imm_adrp),                   "1xx10000xxxxxxxxxxxxxxxxxxxxxxxx")
// Add/subtract (immediate)
INST(ADD_32_imm, "ADD", (Wd_WSP, Wn_WSP, imm_addsub),"000100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(ADDS_32_imm, "ADDS", (Wd, Wn_WSP, imm_addsub),  "001100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUB_32_imm, "SUB", (Wd_WSP, Wn_WSP, imm_addsub),"010100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUBS_32_imm, "SUBS", (Wd, Wn_WSP, imm_addsub),  "011100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(ADD_64_imm, "ADD", (Xd_SP, Xn_SP, imm_addsub),  "100100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(ADDS_64_imm, "ADDS", (Xd, Xn_SP, imm_addsub),   "101100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUB_64_imm, "SUB", (Xd_SP, Xn_SP, imm_addsub),  "110100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUBS_64_imm, "SUBS", (Xd_SP, Xn_SP, imm_addsub),"111100010xxxxxxxxxxxxxxxxxxxxxxx")
// Move wide (immediate)
INST(MOVN_32, "MOVN", (Wd, imm_mov),                 "0001001010xxxxxxxxxxxxxxxxxxxxxx")
INST(MOVZ_32, "MOVZ", (Wd, imm_mov),                 "0101001010xxxxxxxxxxxxxxxxxxxxxx")
INST(MOVK_32, "MOVK", (Wd, imm_mov),                 "0111001010xxxxxxxxxxxxxxxxxxxxxx")
INST(MOVN_64, "MOVN", (Xd, imm_mov),                 "100100101xxxxxxxxxxxxxxxxxxxxxxx")
INST(MOVZ_64, "MOVZ", (Xd, imm_mov),                 "110100101xxxxxxxxxxxxxxxxxxxxxxx")
INST(MOVK_64, "MOVK", (Xd, imm_mov),                 "111100101xxxxxxxxxxxxxxxxxxxxxxx")


