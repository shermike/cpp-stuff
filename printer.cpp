#include "printer.h"

#include <iostream>
#include <string>

struct Printer
{
    void Puts(const std::string& str) {
        std::cout << str;
    }

    std::ostream& Stream() {
        return std::cout;
    }
};

template <typename T>
static constexpr bool HasPrintInfo(...) { return false; }

template <typename T>
static constexpr bool HasPrintInfo(int, decltype((std::declval<T>().PrintInfo(nullptr)))* = 0) { return true; }

template<typename InstT>
void PrintI(InstT* inst, Printer* printer) {
    if constexpr (HasPrintInfo<InstT>(0)) {
        static_cast<InstT*>(inst)->PrintInfo(printer);
    }
}

template<typename InstT, typename... Args>
void PrintInst(InstT* inst, Printer* printer) {
    if constexpr (HasPrintInfo<InstT>(0)) {
        inst->PrintInfo(printer);
    }
    (PrintI(static_cast<Args*>(inst), printer), ...);
}

struct Base {
    virtual void Print(Printer* printer) {
        printer->Stream() << opcode << std::endl;
    }

    std::string opcode;
};

#define DECLARE_INST(name, BaseT, ...) \
    using Supers = std::tuple<BaseT, __VA_ARGS__>; \
    void Print(Printer* printer) { \
        BaseT::Print(printer); \
        PrintInst<name, BaseT, __VA_ARGS__>(this, printer); \
    }

struct InstA : public Base {

    DECLARE_INST(InstA, Base);
    void PrintInfo(Printer* printer) {
        printer->Stream() << " InstA" << std::endl;
    }
};

struct MixinA {
    void PrintInfo(Printer* printer) {
        printer->Stream() << " MixinA" << std::endl;
    }
};

struct MixinNoPrint {
    
};

template<typename T>
struct MixinB : public T {
    void PrintInfo(Printer* printer) {
        printer->Stream() << " MixinB" << std::endl;
    }
};

struct InstB : public MixinB<InstA>, public MixinA, public MixinNoPrint {
    DECLARE_INST(InstB, MixinB<InstA>, MixinA, MixinNoPrint);

    void PrintInfo(Printer* printer) {
        printer->Stream() << " InstB" << std::endl;
    }
};

struct InstC : public Base{
    DECLARE_INST(InstC, Base);
};

struct InstD : public InstC, public MixinNoPrint, public MixinA {
    DECLARE_INST(InstD, InstC, MixinNoPrint, MixinA);

    void PrintInfo(Printer* printer) {
        printer->Stream() << " InstD" << std::endl;
    }
};

void test_printer() {
    //std::cout << HasPrintInfo<InstA>(0) << std::endl;
    InstB b;
    Printer printer;
    b.Print(&printer);
    InstA().Print(&printer);
    InstD().Print(&printer);
}