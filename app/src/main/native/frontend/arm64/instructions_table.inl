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
Type(PCRelAdr)
INST(ADR, "ADR", (Xd, imm_adr),                      "0xx10000xxxxxxxxxxxxxxxxxxxxxxxx")
INST(ADRP, "ADRP", (Xd, imm_adrp),                   "1xx10000xxxxxxxxxxxxxxxxxxxxxxxx")
// Add/subtract (immediate)
Type(AddSubImmediate)
INST(ADD_imm, "ADD", (Wd_WSP, Wn_WSP, imm_addsub),   "x00100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(ADDS_imm, "ADDS", (Wd, Wn_WSP, imm_addsub),     "x01100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUB_imm, "SUB", (Wd_WSP, Wn_WSP, imm_addsub),   "x10100010xxxxxxxxxxxxxxxxxxxxxxx")
INST(SUBS_imm, "SUBS", (Wd, Wn_WSP, imm_addsub),     "x11100010xxxxxxxxxxxxxxxxxxxxxxx")
// Move wide (immediate)
Type(MovWide)
INST(MOVN, "MOVN", (Xd, imm_mov),                    "x00100101xxxxxxxxxxxxxxxxxxxxxxx")
INST(MOVZ, "MOVZ", (Xd, imm_mov),                    "x10100101xxxxxxxxxxxxxxxxxxxxxxx")
INST(MOVK, "MOVK", (Xd, imm_mov),                    "x11100101xxxxxxxxxxxxxxxxxxxxxxx")

// Logical (Immediate)
Type(LogicalImmediate)
INST(AND_imm, "AND",  (),                            "x00100100xxxxxxxxxxxxxxxxxxxxxxx")
INST(ORR_imm, "ORR",   (),                           "x01100100xxxxxxxxxxxxxxxxxxxxxxx")
INST(EOR_imm, "EOR",   (),                           "x10100100xxxxxxxxxxxxxxxxxxxxxxx")
INST(ANDS_imm, "ANDS",  (),                          "x11100100xxxxxxxxxxxxxxxxxxxxxxx")

// Immediate - Bitfield
INST(SBFM, "SBFM", (),                               "x00100110xxxxxxxxxxxxxxxxxxxxxxx")
INST(BFM,  "BFM",  (),                               "x01100110xxxxxxxxxxxxxxxxxxxxxxx")
INST(UBFM, "UBFM", (),                               "x10100110xxxxxxxxxxxxxxxxxxxxxxx")

// Immediate - Extract
INST(EXTR,  "EXTR",(),                               "x00100111x0xxxxxxxxxxxxxxxxxxxxx")



