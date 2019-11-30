//INST(OpCode, Ret, args...)

//Frontend Register Get/Set
INST2(A64SetX, RetVoid, A64XReg, RegU64)
INST2(A64SetW, RetVoid, A64WReg, RegU32)
INST2(A64SetV, RetVoid, A64VReg, RegU128)

INST1(A64GetX, RetU64, A64XReg)
INST1(A64GetW, RetU32, A64WReg)
INST1(A64GetV, RetU128, A64VReg)

