//cond
FIELD(cond, 0, 3); // condition for branch
FIELD(cond_comp, 12, 15); // condition for conditional compare

//L1 Type
FIELD(instr_type_1, 25, 28);

//branch
FIELD(sys_bch_op0, 29, 31);
FIELD(sys_bch_op1, 22, 25);
FIELD(bch_cond_type, 22, 22);
FIELD(bch_ucond_opc, 21, 24);
FIELD(bch_ucond_op2, 16, 20);
FIELD(bch_ucond_op3, 10, 15);
FIELD(bch_ucond_op4, 0, 4);
FIELD(bch_ucond_offset, 0, 25);
FIELD(bch_cond_offset, 5, 23);
//exception
FIELD(exp_gen_ll, 0, 1);
//system
FIELD(system_register, 5, 19); // System register?
FIELD(o0, 19, 19); // System register?
FIELD(op1, 16, 18); // System register?
FIELD(CRn, 12, 15); // System register?
FIELD(CRm, 8, 11); // System register? Also used for barriers
FIELD(op2, 5, 7); // System register?

//data process type
FIELD(dp_imm, 10, 21);

FIELD(dp_type, 23, 25);
FIELD(addsub_imm_op, 29, 30);
FIELD(addsub_imm_update_flag, 29, 29);
FIELD(addsub_imm_update_sub, 30, 30);
FIELD(addsub_imm_update_64bit, 30, 30);
FIELD(addsub_imm_shift, 22, 22);
FIELD(mov_imm_64bit, 31, 31);
FIELD(mov_imm_opc, 29, 30);
FIELD(mov_imm_hw, 21, 22);
//adrp
FIELD(adr_page, 31, 31);

//load and store ops
FIELD(ldrstr_op0, 31, 31);
FIELD(ldrstr_op1, 28, 29);
FIELD(ldrstr_op2, 26, 26);
FIELD(ldrstr_op3, 23, 24);
FIELD(ldrstr_op4, 16, 21);
FIELD(ldrstr_op5, 10, 11);

FIELD(size, 30, 31);
FIELD(is_simd, 26, 26);
FIELD(ldrstr_opc, 22, 23);
FIELD(addr_mode, 10, 11);
FIELD(wback, 23, 23);

FIELD(_5_11, 5, 11);
FIELD(_5_18, 5, 18);
FIELD(_8_11, 8, 11);
FIELD(_10_12, 10, 12); // immediate value for some DPReg operations
FIELD(_10_15, 10, 15); // immediate value for some DPReg operations
FIELD(_12_20, 12, 20);
FIELD(_13_20, 13, 20); // immediate value for some float operations
FIELD(_15_21, 15, 21); // immediate value for some Load/Store pair operations
FIELD(_16_20, 16, 20); // immediate value for some DPReg operations
FIELD(_22_23, 22, 23); // size value for some SIMD operations
FIELD(option, 13, 15); // extend option for extened register instructions
FIELD(hw, 21, 22); // shift amount for mov instructions
FIELD(opc, 29, 30); // additional opcode for instructions
FIELD(shift, 22, 23); // shift type
FIELD(sf, 31, 31); // sf x / w register

//Registers
FIELD(Rt, 0, 4); // register to test against/store
FIELD(Rd, 0, 4); // destination register for data processing operations
FIELD(Rn, 5, 9); // input register 1
FIELD(Rm, 16, 20); // input register 2
FIELD(Ra, 10, 14); // input register 3
FIELD(Rt2, 10, 14); // 2nd register to store
FIELD(Rs, 16, 20); // status register for load/store exclusive

//imme
FIELD(immlo, 29, 30); // low portion of immediate for PC-rel. addressing
FIELD(immhi, 5, 23); // high portion of immediate for PC-rel. addressing
FIELD(imms, 10, 15); // part of immediate for DPImm operations
FIELD(immr, 16, 21); // part of immediate for DPImm operations
FIELD(imm16, 5, 20); // u16 immediate
FIELD(imm7, 15, 21); // u16 immediate
FIELD(imm8, 13, 20); // u16 immediate
FIELD(imm9, 12, 20); // u16 immediate
FIELD(N, 22, 22); // part of immediate for DPImm operations
FIELD(L, 22, 22);
FIELD(b40, 19, 23); // immediate for TB(N)Z
FIELD(b5, 31, 31); // immediate for TB(N)Z
FIELD(nzcv, 0, 3); // flags set for cond compare