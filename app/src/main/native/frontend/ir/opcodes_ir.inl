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


//Load And Store
INST2(Str8, RetVoid, RegAddr, RegU8)
INST2(Str16, RetVoid, RegAddr, RegU16)
INST2(Str32, RetVoid, RegAddr, RegU32)
INST2(Str64, RetVoid, RegAddr, RegU64)
INST2(Str128, RetVoid, RegAddr, RegU128)
INST1(Ldr8, RetU8, RegAddr)
INST1(Ldr16, RetU16, RegAddr)
INST1(Ldr32, RetU32, RegAddr)
INST1(Ldr64, RetU64, RegAddr)
INST1(Ldr128, RetU128, RegAddr)
//Load And Store (Exclusive)
INST2(Strx8, RetVoid, RegAddr, RegU8)
INST2(Strx16, RetVoid, RegAddr, RegU16)
INST2(Strx32, RetVoid, RegAddr, RegU32)
INST2(Strx64, RetVoid, RegAddr, RegU64)
INST2(Strx128, RetVoid, RegAddr, RegU128)
INST1(Ldrx8, RetU8, RegAddr)
INST1(Ldrx16, RetU16, RegAddr)
INST1(Ldrx32, RetU32, RegAddr)
INST1(Ldrx64, RetU64, RegAddr)
INST1(Ldrx128, RetU128, RegAddr)


//STATE
INST1(GetCarry, RetU1, State)
INST1(GetOverflow, RetU1, State)
INST1(GetGE, RetU32, State)