//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "instruction_fields.h"
#include <map>
#include <vector>

namespace Instruction::A64 {

    using MaskValuePair = std::pair<InstrA64, InstrA64>;

    MaskValuePair ParseMaskValuePair(std::string mask_str);

    enum InstrTypeA64 {
        Invalid,
        Unallocated,
        AddSubImmediate,
        MovWide,
        PCRelAdr,
        LogicalImmediate,
        BitField,
        Extract,
        Branches,
        ExpGen,
        System,
        StoreRegImm,
        LoadRegImm,
        LoadLiteral,
        DataProcessingRegister,
        DataProcessingSIMDAndFP
    };

#define INST(x, ...) x,
#define Type(x)
    enum class OpcodeA64 : InstrA64 {
        UN_DECODED,
        INVALID,

#include "instructions_table.inl"

        NUM_INSTRUCTIONS,
    };
#undef INST
#undef Type

    template<typename FnT>
    struct VisitorCaller;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4800) // forcing value to bool 'true' or 'false' (performance warning)
#endif

    template<typename Visitor, typename ...Args, typename CallRetT>
    struct VisitorCaller<CallRetT(Visitor::*)(Args...)> {
        template<size_t ...iota>
        static auto Make(std::integer_sequence<size_t, iota...>,
                         CallRetT (Visitor::* const fn)(Args...),
                         const std::array<AArch64Fields::Fields, sizeof...(iota)> f_types) {
            return [fn, f_types](Visitor& v, AArch64Inst &inst) {
                (void)inst;
                (void)f_types;
                return (v.*fn)(static_cast<Args>(GetAArch64FieldGetFunc(f_types[iota])(inst))...);
            };
        }
    };

    template<typename Visitor, typename ...Args, typename CallRetT>
    struct VisitorCaller<CallRetT(Visitor::*)(Args...) const> {
        template<size_t ...iota>
        static auto Make(std::integer_sequence<size_t, iota...>,
                         CallRetT (Visitor::* const fn)(Args...) const,
                         const std::array<AArch64Fields::Fields, sizeof...(iota)> f_types) {
            return [fn, f_types](const Visitor& v, AArch64Inst &inst) {
                (void)inst;
                (void)f_types;
                return (v.*fn)(static_cast<Args>(GetAArch64FieldGetFunc(f_types[iota])(inst))...);
            };
        }
    };

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    class InstrA64Info {
    public:

        InstrA64Info();

        InstrA64Info(OpcodeA64 opcode, MaskValuePair mask_str, std::string name, std::vector<AArch64Fields::Fields> fields, InstrTypeA64 type)
                : opcode_(opcode), name_{name}, mask_pair_{mask_str}, fields_{fields}, type_{type} {
        }

        bool Test(InstrA64 bits) const;

        OpcodeA64 opcode_;
        MaskValuePair mask_pair_;
        std::string name_;
        std::vector<AArch64Fields::Fields> fields_;
        InstrTypeA64 type_;
    };

    template <typename Visitor>
    class Dispatcher {
    public:
        using VisitorReturnType = typename Visitor::instruction_return_type;
        using VisitorCallerFunc = std::function<VisitorReturnType(Visitor&, AArch64Inst&)>;

        void init();

        template<typename FnT, size_t args_count = FunctionInfo<FnT>::args_count>
        static auto GetCaller(FnT fn, std::array<AArch64Fields::Fields, args_count> fields) {
            using Iota = std::make_index_sequence<args_count>;
            const auto proxy_fn = VisitorCaller<FnT>::Make(Iota(), fn, fields);
            return proxy_fn;
        }

    private:
        std::map<OpcodeA64, VisitorCallerFunc> callers_;
    };

    struct DefaultDissamVisitor final {
        using instruction_return_type = void;
#define Type(...)
#define FIELD1(f1) u32 f1
#define FIELD2(f1, f2) u32 f1, u32 f2
#define FIELD3(f1, f2, f3) u32 f1, u32 f2, u32 f3
#define INST(code, name, fields, ...) \
    void code(fields);

#include "instructions_table.inl"

#undef FIELD1
#undef FIELD2
#undef FIELD3
#undef INST
#undef Type
    };

    class InstructionTableA64 {
    public:

        InstructionTableA64();

        static InstructionTableA64 &Get();

        void init();

        InstrTypeA64 Type(OpcodeA64 opcode);

        const std::map<OpcodeA64, InstrA64Info> &GetInstrTable() const;

        std::map<OpcodeA64, InstrA64Info> &GetInstrTable(InstrTypeA64 type);

        InstrA64Info &GetInstrInfo(OpcodeA64 opcode);

    private:
        InstrTypeA64 cur_type_;
        std::map<OpcodeA64, InstrA64Info> instr_table_;
        std::map<InstrTypeA64, std::map<OpcodeA64, InstrA64Info>> instr_type_table_;
    };

}
