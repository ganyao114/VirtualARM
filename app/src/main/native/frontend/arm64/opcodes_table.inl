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
//INST(HINT, "HINT", (imm_hint),    0b11111111111111111111000000011111, 0b11010101000000110010000000011111)
//INST(WFE, "WFE", (),              0b11111111111111111111111111111111, 0b11010101000000110010000001011111)
//INST(WFI, "WFI", (),              0b11111111111111111111111111111111, 0b11010101000000110010000001111111)
//INST(SEV, "SEV", (),              0b11111111111111111111111111111111, 0b11010101000000110010000010011111)
//INST(SEVL, "SEVL", (),            0b11111111111111111111111111111111, 0b11010101000000110010000010111111)
//INST(CLREX, "CLREX", (imm_clrex), 0b11111111111111111111000011111111, 0b11010101000000110011000001011111)
//INST(DSB, "DSB", (barrier_type),  0b11111111111111111111000011111111, 0b11010101000000110011000010011111)
//INST(DMB, "DMB", (barrier_type),  0b11111111111111111111000011111111, 0b11010101000000110011000010111111)
//INST(ISB, "ISB", (barrier_type),  0b11111111111111111111000011111111, 0b11010101000000110011000011011111)
//INST(SYS, "SYS", (TODO),          0b11111111111110000000000000000000, 0b11010101000010000000000000000000)
//INST(SYSL, "SYSL", (TODO),        0b11111111111110000000000000000000, 0b11010101001010000000000000000000)


