//
// Created by 甘尧 on 2019-09-24.
//

#pragma once

#include "instruction_fields.h"
#include <map>
#include <vector>

namespace Instructions::A64 {

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

#include "instructions_table_all.inl"

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

        void Init() {
            using namespace AArch64Fields;

#define FIELD1(...) __VA_ARGS__
#define FIELD2(...) __VA_ARGS__
#define FIELD3(...) __VA_ARGS__
#define FIELD4(...) __VA_ARGS__
#define Type(x)
#define INST(x, name, fields, mask, ...) \
    callers_[static_cast<int>(OpcodeA64::x)] = Dispatcher<Visitor>::GetCaller(&Visitor::x, {fields});

#include "instructions_table_all.inl"

#undef INST
#undef ARG_LIST
#undef Type
#undef FIELD1
#undef FIELD2
#undef FIELD3
#undef FIELD4
        }

        Dispatcher(Visitor &visitor) : visitor(visitor) {}

        template<typename FnT, size_t args_count = FunctionInfo<FnT>::args_count>
        auto GetCaller(FnT fn, std::array<AArch64Fields::Fields, args_count> fields) {
            using Iota = std::make_index_sequence<args_count>;
            const auto proxy_fn = VisitorCaller<FnT>::Make(Iota(), fn, fields);
            return proxy_fn;
        }

        FORCE_INLINE VisitorReturnType Call(OpcodeA64 opcodeA64, AArch64Inst &inst) {
            visitor.inst = &inst;
            callers_[static_cast<int>(opcodeA64)](visitor, inst);
        }

    private:
        Visitor &visitor;
        VisitorCallerFunc callers_[static_cast<int>(OpcodeA64::NUM_INSTRUCTIONS)];
    };

    struct BaseVisitor {
        AArch64Inst *inst;
        // maybe a so or exe load base
        VAddr module_base{0};
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
