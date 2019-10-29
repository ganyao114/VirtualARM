//INST(OpCode, Ret, args...)

INST0(Nop, RetVoid)
INST0(BreakPoint, RetVoid)

// Calculations
INST2(AddReg32, RetU32, RegU32, RegU32)
INST2(AddReg64, RetU64, RegU64, RegU64)
INST2(SubReg32, RetU32, RegU32, RegU32)
INST2(SubReg64, RetU64, RegU64, RegU64)
INST2(MulReg32, RetU32, RegU32, RegU32)
INST2(MulReg64, RetU64, RegU64, RegU64)
INST2(AndReg32, RetU32, RegU32, RegU32)
INST2(AndReg64, RetU64, RegU64, RegU64)
INST2(EorReg32, RetU32, RegU32, RegU32)
INST2(EorReg64, RetU64, RegU64, RegU64)
INST2(OrReg32, RetU32, RegU32, RegU32)
INST2(OrReg64, RetU64, RegU64, RegU64)
INST1(NotReg32, RetU32, RegU32)
INST1(NotReg64, RetU64, RegU64)

INST1(CheckZero32, RetU1, RegU32)
INST1(CheckZero64, RetU1, RegU64)
INST2(TestBit, RetU1, RegU64, Imm8)