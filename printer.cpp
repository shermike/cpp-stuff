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

template<typename T> struct Foo {};

template <typename T>
static constexpr bool HasPrintInfo(...) { return false; }

template <typename T>
static constexpr bool HasPrintInfo(int, decltype((std::declval<T>().PrintInfo(nullptr, Foo<T>())))* = 0) { return true; }

template<typename InstT>
void PrintI(InstT* inst, Printer* printer) {
    if constexpr (HasPrintInfo<InstT>(0)) {
        static_cast<InstT*>(inst)->PrintInfo(printer, Foo<InstT>());
    }
}

template<typename InstT, typename... Args>
void PrintInst(InstT* inst, Printer* printer) {
    if constexpr (HasPrintInfo<InstT>(0)) {
        inst->PrintInfo(printer, Foo<InstT>());
    }
    (PrintI(static_cast<Args*>(inst), printer), ...);
}

struct Base {
    virtual void Print(Printer* printer) {
        printer->Stream() << GetName() << std::endl;
    }

    virtual std::string GetName() {
        return "Base";
    }

    std::string opcode;
}; 

#define DECLARE_INST(name, BaseT, ...) \
    using Supers = std::tuple<BaseT, __VA_ARGS__>; \
    std::string GetName() override { \
        return #name; \
    } \
    void Print(Printer* printer) override { \
        printer->Stream() << GetName() << std::endl; \
        PrintInst<name, BaseT, __VA_ARGS__>(this, printer); \
    }

struct InstA : public Base {

    DECLARE_INST(InstA, Base);
    void PrintInfo(Printer* printer, Foo<InstA>) {
        printer->Stream() << "Info InstA" << std::endl;
    }
};

struct MixinA {
    void PrintInfo(Printer* printer, Foo<MixinA>) {
        printer->Stream() << "Info MixinA" << std::endl;
    }
};

struct MixinNoPrint {
    
};

template<typename T>
struct MixinB : public T {
    void PrintInfo(Printer* printer, Foo<MixinB>) {
        printer->Stream() << "Info MixinB" << std::endl;
    }
};

struct InstB : public MixinB<InstA>, public MixinA, public MixinNoPrint {
    DECLARE_INST(InstB, MixinB<InstA>, MixinA, MixinNoPrint);

    void PrintInfo(Printer* printer, Foo<InstB>) {
        printer->Stream() << "Info InstB" << std::endl;
    }
};

struct InstC : public InstA{
    DECLARE_INST(InstC, InstA);
};

struct InstD : public InstC, public MixinNoPrint, public MixinA {
    DECLARE_INST(InstD, InstC, MixinNoPrint, MixinA);

    void PrintInfo(Printer* printer, Foo<InstD>) {
        printer->Stream() << "Info InstD" << std::endl;
    }
};



void test_printer() {
    Printer printer;
    InstA().Print(&printer);
    InstB().Print(&printer);
    InstC().Print(&printer);
    InstD().Print(&printer);
}
